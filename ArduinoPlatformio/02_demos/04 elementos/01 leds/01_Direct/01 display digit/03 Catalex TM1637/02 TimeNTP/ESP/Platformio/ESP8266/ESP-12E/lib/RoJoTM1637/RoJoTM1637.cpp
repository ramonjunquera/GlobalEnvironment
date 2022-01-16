/*
  Nombre de la librería: RoJoTM1637.h
  Versión: 20170914
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión del chip TM1637 para display de leds de 4 dígitos (como el Catalex).

  Descripción detallada del funcionamiento del chip TM1637
  
    El chip TM1637 está especialmente pensado para gestionar displays de led de dígitos de 7
    segmentos, hasta un máximo de 6 dígitos.
    En la definición de la clase existe la constante digitsCount con valor 4, que indica el 
    número de dígitos que tiene el display, porque el ejemplo está hecho para un Catalex de 4 dígitos.

    También es capaz de gestionar un teclado, pero en el caso de este ejemplo, sólo tenemos el display.
    Lo que le hace tan popular es su precio y su sencillez de manejo.
  
    Los 4 pines que podemos ver están etiquetados de la siguiente manera:
    - CLK clock
    - DIO data I/O
    - VCC 3.3V-5V
    - GND tierra
  
    Por lo tanto, sólo necesitaremos dos pines para gestionarlo. El protocolo utilizado no es
    el de un puerto serie, ni I2C, sino un propio llamado TWSI.
  
    Según el fabricante, el pin del reloj (CLK) que es el que marca la frecuencia, tiene un
    límite de 250KHz, a partir del cual se comienzan a cometer errores.
    Queremos averigüar cual es el periodo (el tiempo que transcurre entre dos ciclos) de una
    frecuencia de 250KHz.
      250KHz * 1000Hz/1KHz = 250000Hz
      Periodo en segundos = 1/frecuencia en Hz = 1/250000 = 0,000004s
      Periodo en milisegundos = 0,000004s * 1000ms/1s = 0,004ms
      Periodo en microsegundos = 0,004ms * 1000microssegundos/1ms = 4 microsegundos
    Por lo tanto el periodo de una frecuencia de 250Hz es de 4 microsegundos.
    En un ciclo tenemos dos estado, porque hablamos de ondas digitales que pasan de estado
    bajo a estado alto. Así que el tiempo que se mantiene un estado en un ciclo es la mitad
    del ciclo. Concretamente 4/2 = 2 microsegundos.
    2 microsegundos es el mínimo tiempo que debe mantenerse un estado en un ciclo para que
    la frecuencia sea soportada por el TM1637.
    Para comprender mejor el funcionamiento de protocolo de comunicación se incluye en la
    librería una imagen con un gráfico explicativo.
    Nuestro objetivo es comunicarnos con el chip a la máxima velocidad posible para ahorrar
    tiempo, siempre sin superar los límites del fabricante para que funcione correctamente.
    Así que, aunque ya sospechamos que si forzamos con una demora de 2 microsegundos cada
    estado probablemente no funcionará por ser exáctamente el límite... lo probamos.
    En la sección privada de la clase tenemos la constante stateDelay. Si la fijamos a 2
    microsegundo podemos comprobar empíricamente que no funciona, así que nos conformaremos
    con 3 microsegundos que es el mínimo valor admitido.
  
    Para mantener la frecuencia del reloj tenemos tres métodos:
    - Llamamos a la rutina contínuamente, anotamos el tiempo y asi sabemos en qué momento
      se han sobrepasado los dos microsegundos calculados y entonces tomar la siguiente acción...
    - Creamos una interrupción que se ejecute por tiempo, al menos cada 2 microsegundos para
      que compruebe lo que hay pendiente y lo ejecute...
    - Sólamente tenemos en cuenta la frecuencia de reloj cuando queramos enviar datos y en
      ese caso, esperamos (haciendo nada) a que pasen los 2 microsegundos calculados (que
      hemos visto que en la práctica deben ser 3).
    Nosotros hemos tomado la tercera opción. Es más simple y sencilla de implementar. No es
    tan eficiente, porque perdemos el tiempo esperando. Pero teniendo en cuenta que tampoco
    estamos contínuamente enviando datos, tampoco es preocupante.  
  
    El pin del reloj (CLK) lo configuraremos como salida y escribiremos sus estados según
    nos convenga.
    El pin de datos lo utilizamos tanto para enviar información como para recibirla.
    Por esta razón trataremos de una forma especial sus estados.
    Para escribir un estado bajo lo configuraremos como salida.
    Para escribir un estado alto lo configuraremos como estrada con las resistencias de
    pullup activas.

    Recordemos que los segmentos del dígitos se identifican de la siguiente manera:
 
            a
          ███
        █      █
      f █      █ b
        █  g   █
          ███
        █      █
      e █      █ c
        █      █
          ███     █ h
            d 
 
    Y su posición es la siguiente:
    a = B00000001
    b = B00000010
    c = B00000100
    d = B00001000  
    e = B00010000
    f = B00100000
    g = B01000000
    h = B10000000
  
    Notas:
    - Los dos puntos centrales corresponden al segmento del punto decimal del segundo dígito.
    - Los dos puntos siempre van juntos. No se puede encender cada punto por separado.
    - La librería de definición de caracteres puede ampliarse muy fácilmente.
    - Se han incluido los caracteres para representar números hexadecimales, negativos y grados centígrados. 
*/

#ifndef RoJoTM1637_cpp
#define RoJoTM1637_cpp

#include "RoJoTM1637.h"
#include "Arduino.h"

void RoJoTM1637::begin(byte pinCLK, byte pinDIO)
{
  //Anotamos los valores de los pines en las variables privadas
  _pinCLK = pinCLK;
  _pinDIO = pinDIO;

  //El pin del reloj siempre será de salida
  pinMode(_pinCLK, OUTPUT);
  //Informamos que estamos en estado de comunicaciones inactivas
  //Consiste en un ciclo completo con CLK=HIGH & DIO=HIGH
  digitalWrite(_pinCLK,HIGH);
  pinMode(_pinDIO, INPUT_PULLUP);
  delayMicroseconds(stateDelay * 2);
  //Activamos el display
  enable();
  //Lo limpiamos
  clear();
}

bool RoJoTM1637::enable()
{
  //Activa el chip. Lo saca del estado inactivo.
  //Aprovechamos para fijar también el brillo
  //Devuelve true ante cualquier error

  //Enviamos marca de inicio de transmisión
  _sendStartMark();
  //Enviamos el comando de inicialización que al mismo tiempo sirve para fijar el brillo.
  //El brillo utiliza los 3 últimos bits
  //A continuación los unimos con el comando de inicialización TM1637_start=0x88=B10001000
  //Enviamos el dato y tomamos nota del resultado
  bool errorCode = _sendData(0x88 | _brightness);
  //Enviamos marca de fin de transmisión  
  _sendEndMark();
  //Si hubo algún error...hemos terminado
  if(errorCode) return true;

  //Siempre pediremos mostrar dígitos.
  //Tenemos dos comandos:
  // - TM1637_writeDigit 0x44
  // - TM1637_writeDigits 0x40
  //El primero sirve para mostrar un sólo dígito
  //El segundo mostrará tandos dígitos como pasemos, en posiciones consecutivas.
  //Siempre utilizaremos este segundo método

  //Enviamos marca de inicio de datos
  _sendStartMark();
  //Enviamos comando que indica que se escribirán los segmentos de un dígito
  //Tomamos nota del estado del envío
  errorCode = _sendData(0x40); //TM1637_writeDigit=0x40
  //Enviamos marca de fin de datos
  _sendEndMark();

  //Devolvemos el resultado obtenido
  return errorCode;
}

bool RoJoTM1637::disable()
{
  //Ponemos en chip en modo desactivado
  //Devuelve true ante cualquier error
  
  //Enviamos marca de inicio
  _sendStartMark();
  //Enviamos comando de desactivación y tomamos nota del resultado
  bool errorCode = _sendData(0x80); //TM1637_end=0x80
  //Enviamos marca de fin
  _sendEndMark();
  //Devolvemos el resultado obtenido
  return errorCode;
}

RoJoTM1637::~RoJoTM1637()
{
  //Definición de destructor
  //Antes de finalizar, desactivamos el chip
  
  disable();
}

void RoJoTM1637::_sendStartMark()
{
  //Enviamos la marca de inicio
  //Dura un ciclo con CLK=HIGH y DIO=LOW
  digitalWrite(_pinCLK, HIGH);
  pinMode(_pinDIO, OUTPUT);
  delayMicroseconds(stateDelay * 2);
}

void RoJoTM1637::_sendEndMark()
{
  //Enviamos la marca de fin
  //El primer medio ciclo tendrá CLK=LOW y DIO=LOW
  digitalWrite(_pinCLK, LOW);
  pinMode(_pinDIO, OUTPUT);
  //Esperamos el tiempo de medio ciclo
  delayMicroseconds(stateDelay);
  //El segundo medio ciclo tendrá CLX=HIGH y DIO=HIGH
  digitalWrite(_pinCLK, HIGH);
  pinMode(_pinDIO, INPUT);
  //Esperamos el tiempo de medio ciclo
  delayMicroseconds(stateDelay);
  //La marca de fin deja en estado HIGH ambos pines, indicando que pasa a modo inactivo
  //Hasta que se vuelva a enviar la marca de inicio
}

bool RoJoTM1637::_sendData(byte value)
{
  //Envía un dato al chip
  //Suponemos que ya se ha enviado la marca de inicio con _sendStartMark
  //Descomponemos el byte en sus bits y los enviamos de uno en uno
  //El envío de un bit se hace en un ciclo
  //El primer medio ciclo tendremos el pin de reloj en estado bajo
  //El segundo medio ciclo estará en alto
  //En todo el ciclo se debe mantener el valor en el pin de datos
  //Devuelve true ante cualquier error

  //Recorremos cada uno de los bits del byte de datos
  for(byte bit=0;bit<8;bit++)
  {
    //El primer medio ciclo mantenemos CLK a LOW
    digitalWrite(_pinCLK, LOW);
    //Cambiamos el estado del pin de datos en función del bit a enviar
    value & (1<<bit)?pinMode(_pinDIO, INPUT) : pinMode(_pinDIO, OUTPUT);
    //Esperamos el tiempo de medio ciclo
    delayMicroseconds(stateDelay);
    //Activamos el pin del reloj para el segundo medio ciclo
    digitalWrite(_pinCLK, HIGH);
    //Esperamos el tiempo del segundo medio ciclo
    delayMicroseconds(stateDelay);
  }
  //Leemos la respuesta de TM1637 sobre la transmisión
  //En el primer medio ciclo el reloj se pone en estado bajo (como antes)
  digitalWrite(_pinCLK, LOW);
  //Esperamos el tiempo del medio ciclo
  delayMicroseconds(stateDelay);
  //Leemos el estado de la transmisión del byte
  //Ponemos el pin de datos como entrada
  pinMode(_pinDIO, INPUT);
  //Ponemos el reloj en estado alto para el segundo medio ciclo
  digitalWrite(_pinCLK, HIGH);
  //Esperamos el medio ciclo
  delayMicroseconds(stateDelay);
  //Devolvemos el valor de la respuesta
  //Si hubo algún error en la transmisión, el estado será alto
  return digitalRead(_pinDIO);
}

bool RoJoTM1637::setDoubleDot(bool status)
{
  //Fija el valor de los dos puntos centrales
  
  //Guardamos el valor del estado de los dos puntos
  _doubleDot=status;
  //Escribimos de nuevo el segundo dígito y devolvemos el estado
  return set1DigitChar(1,_charDigit1);
}

bool RoJoTM1637::set1DigitChar(byte digit,byte charCode)
{
  //Escribe un carácter en la posición de un dígito
  //El código de carácter procede de la tabla _chars definida en el archivo .h
  //El dígito corresponde a la posición del dígito en la pantalla. El primero es el 0
  //Devuelve true ante cualquier error
  
  //Si la posición del dígito es mayor que el número de dígitos del display...error
  if(digit>=digitsCount) return true;
  //Enviamos marca de inicio de datos
  _sendStartMark();
  //Calculamos el valor de los segmentos que vamos a activar
  byte segmentsValue=_chars[charCode];
  //Si vamos a escribir sobre el dígito 1 (segunda posición)...
  if(digit==1)
  {
    //...es el dígito que también controla los dos puntos centrales
    //Tomamos nota del carácter que tendrá esta posición
    _charDigit1=charCode;
    //Si los dos puntos deben estar activados, le sumamos el valor del segmento del punto decimal
    segmentsValue+=_doubleDot*128;
  }
  //Enviamos el comando que indica de qué dígito se trara
  //Tomamos nota del estado del envío
  bool errorCode = _sendData(0xC0 | digit); //TM1637_selectDigit=0xC0
  //Si no hubo errores...
  if(!errorCode)
  {
    //Enviamos la configuración de segmentos a activar y tomamos nota del estado del envío
    errorCode = _sendData(segmentsValue);
  }
  //Enviamos marca de fin de datos  
  _sendEndMark();
  //Devolvemos el estado
  return errorCode;
}

bool RoJoTM1637::set2DigitChar(byte digit,byte charCode1,byte charCode2)
{
  //Escribe dos caracteres a partir de la posición indicada
  //El código de carácter procede de la tabla chars definida en el archivo .h
  //El dígito corresponde a la posición del dígito en la pantalla. El primero es el 0
  //Devuelve true ante cualquier error
  
  //Si la posición del dígito es mayor que el número de dígitos del display...error
  if(digit>=digitsCount-1) return true;
  //Dibujamos el primer carácter y si hay errores...terminamos
  if (set1DigitChar(digit,charCode1)) return true;
  //Dibujamos el segundo carácter y devolvemos el resultado
  return set1DigitChar(digit+1,charCode2);
} 

bool RoJoTM1637::setBrightness(byte brightness)
{
  //Fija el nivel de brillo. El rango de valores es de [0,7]
  //Devuelve true ante cualquier error

  //Tomamos nota del nivel de brillo indicado para utilizarlo en futuras llamadas a la función Enabled()
  //Sólo tenemos en cuenta los 3 últimos bits [0,7]
  _brightness=brightness & 7;
  //Enviamos marca de inicio de transmisión
  _sendStartMark();
  //Activamos el chip. Esta función también fija el nivel de brillo. Tomamos nota del resultado
  bool errorCode = enable();
  //Enviamos marca de fin de transmisión
  _sendEndMark();
  //Devolvemos el resultado obtenido
  return errorCode;
}

bool RoJoTM1637::set2DigitValue(byte digit,byte value)
{
  //Escribe un valor de 2 dígitos a partir de una posición
  //Devuelve true ante cualquier error
  
  //Con 2 dígitos nunca podremos representar un valor superior a 99
  value %= 100;
  //Motramos las decenas y las unidades
  //Devolvemos el estado
  return set2DigitChar(digit,value/10,value%10);
}

bool RoJoTM1637::clear()
{
  //Limpiamos el display. Apagamos todos los segmentos. Incluidos los 2 dos puntos
  //Devuelve true ante cualquier error
  
  //Apagamos los dos puntos y tomamos nota del resultado
  bool errorCode =  setDoubleDot(false);
  //Si hubo errores...hemos terminado
  if(errorCode) return true;
  //Escribimos espacios en los 2 primeros dígitos y tomamos nota del resultado
  errorCode = set2DigitChar(0,16,16);
  //Si hubo errores...hemos terminado
  if(errorCode) return true;
  //Escribimos espacios en los 2 últimos dígitos y devolvemos el resultado
  return set2DigitChar(2,16,16);
}

#endif
