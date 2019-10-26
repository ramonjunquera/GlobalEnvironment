/*
  Nombre de la librería: RoJoTimerAVR4.h
  Versión: 20191006
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones para placas Arduino. Timer 4. Sólo MEGA

  Interrupciones disponibles:
    Arduino UNO & Nano sólo tienen 3 interrupciones: 0, 1 y 2.
      La 0 se utiliza para funciones internas.
      La 1 está libre.
      La 2 se utiliza por la función tone
    Arduino Mega tiene 6. Las mismas que UNO y 3 más.
  Estructura:
    Es impresincindible que cada timer tenga su propia librería porque no se puede saber
    si se ha definido anteriormente la función de interrupción (porque se está usando).
    Si es el caso daría un error de declaración duplicada.
  Descripción:
    La librería crea un objeto con nombre fijo con la sintaxis timerAVRx, donde x
    es el número de la interrupción. Ejemplo: timerAVR1
    Para guardar compatibilidad, los métodos son los mismos que la clase Ticker.
      void detach()
      void attach(float seconds, void (*callback)())
      void attach_ms(uint32_t milliseconds, void (*callback)())
      void once(float seconds, void (*callback)())
      void once_ms(uint32_t milliseconds, void (*callback)())
      public: bool active()
  Tipos de timers:
    Los timers 1, 3, 4 y 5 son iguales. Con la misma resolución y prescalers. El 2 es distinto
    porque tiene una resolución mayor. Pero la clase permite gestionarlos por igual.
    La librería se encarga de seleccionar el prescaler más adecuado para el periodo requerido.
    Si el periodo es mayor que el ofrecido por el mayor prescaler sin CTC, se utiliza un CTC
    adicional que permite extenderlo de manera transparente al usuario.
  Periodos largos:
    Cuando los periodos solicitados exceden el máximo que puede ofrecer el timer por hardware, comienzan a perder
    exactitud.
    Por ejemplo, si fijamos un periodo de 1 segundo (1000000 microsegundos), los timers 1,3,4 y 5 son capaces de 
    abordarlo por hardware y su exactitud es bastante buena. En cambio el 2, al tener más resolución, no puede y
    debe utilizar un CTC (contador) por software. Por eso se contabiliza un poco más de un segundo (aproximadamente 
    1.015 segundos).
*/

//Comprobamos que la placa es compatible
#ifndef ARDUINO_ARCH_AVR
  #error Library RoJoTimerAVR is only compatible with Arduino family devices
#endif

#ifndef RoJoTimerAVR4_h
#define RoJoTimerAVR4_h

#ifdef ARDUINO_AVR_MEGA2560

#include <Arduino.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

class RoJoTimerAVR4 {
  private:
    const byte _timerId=3;
    bool _once; //Es un timer de una sola ejecución?
    uint64_t _myCTCcounter; //Contador actual de nuestro CTC
    uint64_t _myCTC; //CTC propio de la clase
    void (*_functionTimer)(); //Función de llamada del timer
    uint64_t _period; //Periodo en microsegundos
    bool _active=false; //Está activo el timer?

  //Inicia las interrupciones
  private: void _start() {
    const uint16_t _prescalers[2][7]={{1024,256,64,8,1,0,0},{1024,256,128,64,32,8,1}}; //Series de valores de los prescalers
    const byte _prescalersCount[2]={5,7}; //Número de prescalers de cada serie
    const byte _prescalersSerieTimer[5]={0,1,0,0,0}; //Serie de prescalers a utilizar por cada timer
    const uint16_t _maxCTC[2]={65535,255}; //Máximo valor del contador del timer para una serie de prescalers

    uint64_t prescalerPeriod; //Periodo del prescaler sin CTC
    const uint64_t _clockFreq=16; //Frecuencia del reloj en MHz
    byte prescalerIndex=0xFF; //Índice del prescaler seleccionado. Inicialmente ninguno.
    uint64_t CTC=0; //CTC seleccionado
    uint64_t myCTC=1; //CTC propio seleccionado. Inicialmente cada interrupción
    byte prescalerSerie=_prescalersSerieTimer[_timerId-1]; //Serie del prescaler

    //Recorremos los prescalers
    for(byte p=0;p<_prescalersCount[prescalerSerie];p++) {
      //Calculamos el periodo del prescaler sin CTC
      prescalerPeriod=(((uint64_t)_prescalers[prescalerSerie][p])*((uint64_t)_maxCTC[prescalerSerie]))/_clockFreq;
      //Si el prescaler no acepta un periodo tan grande...no buscamos más
      if(prescalerPeriod<_period) break;
      //Con este prescaler podemos obtener el periodo buscado...anotamos que es un prescaler válido
      else prescalerIndex=p;
    }
    //Hemos terminado de comprobar si nos sirve algún prescaler para obtener el periodo indicado

    //Si vale alguno...
    if(prescalerIndex<0xFF) {
      //No utilizaremos CTC propio. Con el CTC adecuado podemos obtener el periodo indicado
      //Calculamos el valor de CTC con la siguiente fórmula:
      //CTC = (periodo / resolución timer) -1 = (tiempo deseado * frecuencia / prescaler) -1
      CTC=((_period*_clockFreq)/((uint64_t)_prescalers[prescalerSerie][prescalerIndex]))-1;
    }
    else { //Ningún prescaler nos sirve. Son todos demasiado pequeños
      //Tomaremos el prescaler más grande como válido
      prescalerIndex=0;
      //Calculamos el periodo del mayor prescaler
      prescalerPeriod=(((uint64_t)_prescalers[prescalerSerie][0])*((uint64_t)_maxCTC[prescalerSerie]))/_clockFreq;
      //Debemos calcular con qué combinación de CTC y myCTC obtenemos el periodo indicado
      //Hasta ahora conocemos el periodo deseado y el periodo del mayor prescaler sin CTC
      //Y que el periodo seleccionado excede al del prescaler
      //Calculamos cuántas veces entra el periodo del prescaler en el que nos interesa
      //Siempre le sumamos uno por si tiene resto
      myCTC=_period/prescalerPeriod+1;
      //Ya tenemos el CTC propio
      //Necesitamos calcular un CTC, no para el periodo total, sino para el que trascurre en un CTC propio.
      //Este tiempo es periodo/myCTC
      //La fórmula es:
      //  CTC = (tiempo deseado * frecuencia / prescaler) -1 = (periodo / myCTC * frecuencia / prescaler) -1 =
      //  CTC = (periodo * frecuencia / myCTC / prescaler) -1
      CTC=(_period*_clockFreq)/(((uint64_t)_prescalers[prescalerSerie][0])*myCTC)-1;
    }
    //Ya tenemos todos los valores que necesitamos para configurar el timer

    //Deshabilitamos las interrupciones globales
    cli();
    //Fijamos el registro TCCR4A a 0 para que se mantengan los valores por defecto de la forma de onda
    TCCR4A=0;
    //Activamos el prescaler seleccionado que corresponde con _prescalersCount[prescalerSerie]-prescalerIndex
    //Además activamos el bit 4 (0x1000) para indicar que usaremos CTC
    TCCR4B=_prescalersCount[prescalerSerie]-prescalerIndex+8;
    //En el registro TIMSK4 activamos el segundo bit (OCIE4A) para que salte la interrupción al llegar al CTC
    //definido en el registro OCR4A
    TIMSK4=0b10;
    //Definimos el valor del CTC para el timer
    OCR4A=CTC;
    //Reseteamos el contador actual de nuestro CTC
    _myCTCcounter=0;
    //Asignamos el valor a nuestro CTC
    _myCTC=myCTC;
    //Volvemos a activar las interrupciones
    sei();
    //El timer está activo
    _active=true;
  }

  //Detiene las interrupciones
  public: void detach() {
    //Deshabilitamos las interrupciones globales
    cli();
    //Fijamos el registro TCCR4A a 0 para que se mantengan los valores por defecto de la forma de onda
    TCCR4A=0;
    //Desactivamos cualquier prescaler
    TCCR4B=0;
    //Desactivamos cualquier salto por interrupción
    TIMSK4=0b10;
    //Volvemos a activar las interrupciones
    sei();
    //El timer no está activo
    _active=false;
  }

  //Activa el timer indefinidamente
  //Parámetros: periodo en segundos, función de llamada
  public: void attach(float seconds, void (*callback)()) {
    //La función a la que se llamará cuando salte la interrupción no debe tener 
    //parámetros y no puede devolver nada

    //Detenemos cualquier timer con el mismo identificador
    detach();
    //No es un timer de una sola ejecución
    _once=false;
    //Anotamos el periodo en microsegundos en la variable interna
    _period=seconds*1000000.0;
    //Anotamos la función de llamada en el array de funciones
    _functionTimer=callback;
    //Iniciamos el timer
    _start();
  }

  //Activa el timer indefinidamente
  //Parámetros: periodo en milisegundos, función de llamada
  public: void attach_ms(uint32_t milliseconds, void (*callback)()) {
    //La función a la que se llamará cuando salte la interrupción no debe tener 
    //parámetros y no puede devolver nada

    //Detenemos cualquier timer con el mismo identificador
    detach();
    //No es un timer de una sola ejecución
    _once=false;
    //Anotamos el periodo en microsegundos en la variable interna
    _period=milliseconds*1000;
    //Anotamos la función de llamada en el array de funciones
    _functionTimer=callback;
    //Iniciamos el timer
    _start();
  }

  //Actualiza el estado de la interrupción
  public: void _update() {
    //Se ha producido una interrupción
    //Aumentamos el contador de nuestro CTC. Si ya debemos ejecutar la función...
    if(++_myCTCcounter>=_myCTC) {
      _myCTCcounter=0; //Reseteamos el contador
      _functionTimer(); //Llamamos a la función indicada
      if(_once) detach(); //Si es de una sola ejecución...lo paramos
    }
  }

  //Lanza la función una sola vez
  //Parámetros: retardo en segundos, función de llamada
  public: void once(float seconds, void (*callback)()) {
    //La función a la que se llamará cuando salte la interrupción no debe tener 
    //parámetros y no puede devolver nada

    //Detenemos cualquier timer con el mismo identificador
    detach();
    //Es un timer de una sola ejecución
    _once=true;
    //Anotamos el periodo en microsegundos en la variable interna
    _period=seconds*1000000.0;
    //Anotamos la función de llamada en el array de funciones
    _functionTimer=callback;
    //Iniciamos el timer
    _start();
  }

  //Lanza la función una sola vez
  //Parámetros: retardo en milisegundos, función de llamada
  public: void once_ms(uint32_t milliseconds, void (*callback)()) {
    //La función a la que se llamará cuando salte la interrupción no debe tener 
    //parámetros y no puede devolver nada

    //Detenemos cualquier timer con el mismo identificador
    detach();
    //Es un timer de una sola ejecución
    _once=true;
    //Anotamos el periodo en microsegundos en la variable interna
    _period=milliseconds*1000;
    //Anotamos la función de llamada en el array de funciones
    _functionTimer=callback;
    //Iniciamos el timer
    _start();
  }

  //Está activo el timer?
  public: bool active() {
    return _active;
  }
}; //Punto y coma obligatorio para que no de error

RoJoTimerAVR4 timerAVR4; //Creamos el objeto del timer con nombre fijo

//Función a la que se llama cuando se produce una interrupción del timer 1
//Con el parámetro TIMER1_COMPA_vect indicamos que se llama cuando se alcanza el CTC definido del timer 1
ISR(TIMER4_COMPA_vect) {
  timerAVR4._update(); //Actualizamos el estado de la interrupción
}

#endif //ARDUINO_AVR_MEGA2560

#endif //RoJoTimerAVR4_h
