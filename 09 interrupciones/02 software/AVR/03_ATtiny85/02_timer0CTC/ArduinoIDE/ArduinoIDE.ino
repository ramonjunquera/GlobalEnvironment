/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar el timer0 en ATtiny85
  Material adicional: ATtiny85

  Descripción:
  En el ejercicio anterior utilizábamos un prescaler del timer0 para generar interrupciones.
  La interrupción se producía siempre que el contador se desbordaba (alcanzaba su máximo valor).

  En este ejercicio aprenderemos a seleccionar un valor que no sea el máximo para que haga
  saltar la interrupción.

  Repetiremos y ampliaremos la documentación para no tener que consultar ejercicios anteriores.

  Registros para gestionar el timer0:
    TCNT0 = Timer/Counter 0
    TCCR0A = Timer/Counter Control Register A
    TCCR0B = Timer/Counter Control Register B
    OCR0A = Timer/Counter0 Output Compare Register A    
    TIMSK = Timer/Counter Interrupt Mask Register

  Registro TCNT0 : Timer/Counter 0
    Contiene el valor actual del contador. Es accesible su lectura o escritura en cualquier momento.

  Registro TCCR0A = Timer/Counter Control Register A
    El registro TCCR0A se compone de 8 bits con la siguiente denominación:
      bit 0 : WGM00 : Waveform Generation Mode, bit 0
      bit 1 : WGM01 : Waveform Generation Mode, bit 1
      bit 2 : reservado (siempre 0)
      bit 3 : reservado (siempre 0)
      bit 4 : COM0B0 : Compare Match Output B Mode, bit 0
      bit 5 : COM0B1 : Compare Match Output B Mode, bit 1
      bit 6 : COM0A0 : Compare Match Output A Mode, bit 0
      bit 7 : COM0A1 : Compare Match Output A Mode, bit 1

    Este registro permite configurar el comportamiento de las interrupciones del timer0.

  Registro TCCR0B = Timer/Counter Control Register B
    El registro TCCR0B se compone de 8 bits con la siguiente denominación:
      bit 0 : CS00 : Clock Select, bit 0
      bit 1 : CS01 : Clock Select, bit 1
      bit 2 : CS02 : Clock Select, bit 2
      bit 3 : WGM02 : Waveform Generation Mode, bit 2
      bit 4 : reservado (siempre 0)
      bit 5 : reservado (siempre 0)
      bit 6 : FOC0B : Force Output Compare B
      bit 7 : FOC0A : Force Output Compare A  

    El valor por defecto de este registro es 0 (sus 8 bits a 0). Esto implica que el
    timer0 estará desactivado.
    Sus 3 bits más bajos (0, 1 y 2) son los que definen el prescaler teniendo en cuenta
    la siguiente tabla:
  
       bit   2    1    0
    Nombre CS02 CS01 CS00 Funcionalidad
             0    0    0  No clock source (Timer/Counter stopped)
             0    0    1  clock/1 (No prescaling)
             0    1    0  clock/8 (From prescaler)
             0    1    1  clock/64 (From prescaler)
             1    0    0  clock/256 (From prescaler)
             1    0    1  clock/1024 (From prescaler)
             1    1    0  External clock source on T1 pin. Clock on falling edge
             1    1    1  External clock source on T1 pin. Clock on rising edge

    La primera de las posibilidades sirve para que el contador no incremente su valor (y no salte nunca la interrupción)
    Las dos últimas no nos interesan demasiado porque sirven para utilizar un reloj externo para incrementar el contador.

    Grupo WGM : Waveform Generation Mode
    Entre los registros TCCR0A y TCCR0B nos encontramos con 3 bits del grupo WGM.
    Esta es la tabla de funcionalidades:

      Modo WGM02 WGM01 WGM00 Funcionalidad (condición de salto de interrupción)
        0    0     0     0   Normal. Cuando se alcanza el máximo valor pemitido (255)
        1    0     0     1   PWM Phase Correct
        2    0     1     0   CTC. Cuando el contador es igual que OCR0A
        3    0     1     1   Fast PWM
        4    1     0     0   Reserved
        5    1     0     1   PWM Phase Correct
        6    1     1     0   Reserved
        7    1     1     1   Fast PWM

  Registro OCR0A : Timer/Counter0 Output Compare Register A
    Cuando el valor del contador (TCNT0) es igual al de este registro se produce una interrupción
    
  Registro TIMSK : Timer/Counter Interrupt Mask Register
    El registro TIMSK se compone de 8 bits con la siguiente denominación:
      bit 0 : reservado (siempre 0)
      bit 1 : TOIE0 : Timer/Counter0 Overflow Interrupt Enable
      bit 2 : TOIE1 : Timer/Counter1 Overflow Interrupt Enable
      bit 3 : OCIE0B : Timer/Counter0 Output Compare Interrupt Enable
      bit 4 : OCIE0A : Timer/Counter0 Output Compare Interrupt Enable
      bit 5 : OCIE1B : Timer/Counter1 Output Compare Interrupt Enable
      bit 6 : OCIE1A : Timer/Counter1 Output Compare Interrupt Enable
      bit 7 : reservado (siempre 0)

    El registro TIMSK define qué interrupciones deben saltar y ante qué eventos.
    
    bit 1 : TOIE0
      Indica si el contador del timer0 debe hacer saltar la interrupción cuando llegue al máximo
      valor que puede contener.
    bit 4 : OCIE0A
      Indica que se debe producir una interrupción si el contador es igual que el registro OCR0A

  En este ejercicio también haremos parpadear el led integrado cada segundo.
  En el ejercicio anterior habíamos calculado que con un prescaler de 1024 conseguíamos un periodo
  de unos 16ms.
  Ahora intentaremos conseguir un periodo de 10ms.
  Así pondremos como límite a nuestro contador el valor 100 para obtener el segundo.

  El problema está en calcular el valor adecuado hasta el que debe subir el contador con un prescaler
  de 1024, para que tengamos un periodo de 10ms...

  La fórmula es la siguiente:
  valor límite = frecuencia CPU / prescaler * periodo deseado - 1
  valor límite = 16.500.000 / 1.024 * 0,01 - 1 = 161,13 ~ 161
  
  Resultado:
  El led integrado en placa parpadea cada segundo (aproximadamente).
*/

//Como trabajaremos directamente con los registros del procesador, incluiremos las librerías de gestión del fabricante
//del procesador
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Declaración de variables globales
const byte pinLed = 1; //Pin al que está conectado el led
volatile byte myCTC=0; //Contador para retardar el periodo de parpadeo

void setup() {
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Deshabilitamos las interrupciones globales. No queremos que salte ninguna interrupción mientras las configuramos.
  cli();
  //Usaremos el modo CTC = la interrupción salta cuando el contador es igual a un valor (OCR0A). Modo 2.
  //WGM00=0 + WGM01=1 + WGM02=0
  //TCCR0A=(1<<WGM01);
  TCCR0A=B10;
  //Fijaremos un prescaler de 1024: CS02=1 + CS01=0 + CS00=1
  TCCR0B=B101;
  //Fijamos el valor que se comparará con el contador y si es igual saltará la interrupción
  OCR0A = 161;
  //Registro TIMSK. bits referentes a interrupción 0:
  //  bit 1 : TOIE0 : timer0 Overflow Interrupt Enable        = 0 = salta cuando el contador llega al valor máximo
  //  bit 3 : OCIE0B : timer0 Output Compare Interrupt Enable = 0 = salta cuando el contador sea igual a OCR0B
  //  bit 4 : OCIE0A : timer0 Output Compare Interrupt Enable = 1 = salta cuando el contador esa igual a OCR0A
  //              76543210     76543210
  TIMSK=(TIMSK & B11100101) | B00010000;
  //Volvemos a activar las interrupciones
  sei(); 
}

void loop() {
  //No tenemos nada que ejecutar en el bucle principal
}

//Función a la que se llama cuando se produce una interrupción
//Con el parámetro TIMER0_COMPA_vect indicamos que se llama cuando se el contador es igual que OCR0A
ISR(TIMER0_COMPA_vect) {
  //Aumentamos el valor del nuestro contador y si es superior al valor calculado...
  if(++myCTC>100) {
    //...lo reseteamos
    myCTC=0;
    //Cambiamos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
  }
}
