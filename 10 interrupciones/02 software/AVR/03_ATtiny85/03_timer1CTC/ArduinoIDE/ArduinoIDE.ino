/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar el timer1 en ATtiny85
  Material adicional: ATtiny85

  Descripción:
  En ejercicio anterior hemos aprendido a utilizar el timer0 del ATtiny85.
  Ahora aprenderemos cómo funciona el timer1.

  El timer1 también tiene una resolución de 8 bits.

  La versión de core que estamos utilizando es arduino-tiny core que utiliza la
  interrupción TIMER1_OVF_vect para funciones como delay, millis, etc.
  Cuando intentamos utilizar la interrupción sin CTC (por desbordamiento del contador),
  obtenemos un error del compilador indicando que esta interrupción ya está siendo
  utilizada. Es correcto. Como hemos dicho, lo utiliza Arduino para delay y millis.
  Por lo tanto, no podremos utilizar este método.
  Pasamos directamente al modo de interrupción por CTC.
  Esto significa que las funciones timer y millis no funcionarán correctamente, porque 
  antes de llegar al desbordamiento de contador, alcanzará el valor de CTC que indiquemos
  y nunca llegará a desbordarse.

  Una de las grandes diferencias entre el timer0 y el 1 es la variedad de prescalers.
  En el timer1 tenemos prescales de todas las potencias de 2 hasta llegar a 16384.
  
  Repetiremos y ampliaremos la documentación para no tener que consultar ejercicios anteriores.
  
  Registros para gestionar los timers:
    TCNT0 = timer0 counter
    TCNT1 = timer1 counter
    GTCCR = General Timer/Counter Control Register
    TCCR0A = timer0 Control Register A
    TCCR0B = timer0 Control Register B
    TCCR1 = timer1 Control Register
    OCR0A = timer0 Output Compare Register A
    OCR1A = timer1 Output Compare Register A
    OCR1B = timer1 Output Compare Register B
    OCR1C = timer1 Output Compare Register C    
    TIMSK = Timer/Counter Interrupt Mask Register

  Registro TCNT0 : timer0 counter
    Contiene el valor actual del contador del timer0. Es accesible su lectura o escritura en cualquier momento.

  Registro TCNT1 : timer1 counter
    Contiene el valor actual del contador del timer1. Es accesible su lectura o escritura en cualquier momento.  

  Registro GTCCR : General Timer/Counter Control Register
    El registro GTCCR se compone de 8 bits con la siguiente denominación:
      bit 0 : PSR0 : timer0 Prescaler Reset
      bit 1 : PSR1 : timer1 Prescaler Reset
      bit 2 : FOC1A : Force Output Compare Match 1A
      bit 3 : FOC1B : Force Output Compare Match 1B
      bit 4 : COM1B0 : Comparator B Output Mode, bit 0
      bit 5 : COM1B1 : Comparator B Output Mode, bit 1
      bit 6 : PWM1B : Pulse Width Modulator B Enable
      bit 7 : TSM

    Bit 0 : PSR0
      El activarlo, reseteamos el contador del prescaler del timer0
    Bit 1 : PSR1
      El activarlo, reseteamos el contador del prescaler del timer1

  Registro TCCR0A = timer0 Control Register A
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

  Registro TCCR0B = timer0 Control Register B
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

  Registro TCCR1 = timer1 Control Register
    El registro TCCR1 se compone de 8 bits con la siguiente denominación:
      bit 0 : CS10 : Clock Select, bit 0
      bit 1 : CS11 : Clock Select, bit 1
      bit 2 : CS12 : Clock Select, bit 2
      bit 3 : CS13 : Clock Select, bit 3
      bit 4 : COM1A0 : Comparator A Output Mode, bit 0
      bit 5 : COM1A1 : Comparator A Output Mode, bit 1
      bit 6 : PWM1A : Pulse Width Modulator A Enable
      bit 7 : CTC1 : Clear Timer/Counter on Compare Match

    Bits CS : Clock Select
      Los pimeros 4 bits son los que definen el valor del prescaler teniendo en cuenta la siguiente tabla:
  
         bit   3    2    1    0
      Nombre CS13 CS12 CS11 CS10 Funcionalidad
               0    0    0    0  No clock source (Timer/Counter stopped)
               0    0    0    1  clock/1 (No prescaling)
               0    0    1    0  clock/2 (From prescaler)
               0    0    1    1  clock/4 (From prescaler)
               0    1    0    0  clock/8 (From prescaler)
               0    1    0    1  clock/16 (From prescaler)
               0    1    1    0  clock/32 (From prescaler)
               0    1    1    1  clock/64 (From prescaler)
               1    0    0    0  clock/128 (From prescaler)
               1    0    0    1  clock/256 (From prescaler)
               1    0    1    0  clock/512 (From prescaler)
               1    0    1    1  clock/1024 (From prescaler)
               1    1    0    0  clock/2048 (From prescaler)
               1    1    0    1  clock/4096 (From prescaler)
               1    1    1    0  clock/8192 (From prescaler)
               1    1    1    1  clock/16384 (From prescaler)

      La primera de las posibilidades sirve para que el contador no incremente su valor (y no salte nunca la interrupción).

    El bit 7 - CTC1 (Clear Timer/Counter on Compare Match).
      Si activamos este bit, el contador se reseteará a 0 después de que alcance el valor del registro OCR1C.
      Si no lo activamos, el contador no se verá afectado al alcanzar el valor del registro OCR1C y seguirá
      incrementando su valor indefinidamente.

    El valor por defecto del registro TCCR1 es 0 (sin prescaler).

  Registro OCR0A = timer0 Output Compare Register A
    Cuando el valor del contador (TCNT0) es igual al de este registro se produce una interrupción

  Registro OCR1A : timer1 Output Compare Register A
    Cuando el valor del contador (TCNT1) es igual al de este registro, activa el flag de interrupción OCF1A

  Registro OCR1B : timer1 Output Compare Register B
    Cuando el valor del contador (TCNT1) es igual al de este registro, activa el flag de interrupción OCF1B

  Registro OCR1C : timer1 Output Compare Register C
    Cuando el valor del contador (TCNT1) es igual al de este registro y hemos teníamos activado el bit
    CTC1 del registro TCCR1, reseteará el contador TCNT1.

  Registro TIMSK = Timer/Counter Interrupt Mask Register
    El registro TIMSK se compone de 8 bits con la siguiente denominación:
      bit 0 : reservado (siempre 0)
      bit 1 : TOIE0 : timer0 Overflow Interrupt Enable, bit 0
      bit 2 : TOIE1 : timer1 Overflow Interrupt Enable, bit 1
      bit 3 : OCIE0B : timer0 Output Compare Interrupt Enable
      bit 4 : OCIE0A : timer0 Output Compare Interrupt Enable
      bit 5 : OCIE1B : timer1 Output Compare Interrupt Enable
      bit 6 : OCIE1A : timer1 Output Compare Interrupt Enable
      bit 7 : reservado (siempre 0)

    El registro TIMSK define qué interrupciones deben saltar y ante qué eventos.
    
    bit 1 : TOIE0
      Indica si el contador del timer0 debe hacer saltar la interrupción cuando llegue al máximo
      valor que puede contener.
    bit 2 : TOIE1
      Indica si el contador del timer1 debe hacer saltar la interrupción cuando llegue al máximo
      valor que puede contener.
    bit 3 : OCIE0B
      Indica que se debe producir una interrupción si el contador es igual que el registro OCR0B
    bit 4 : OCIE0A
      Indica que se debe producir una interrupción si el contador es igual que el registro OCR0A
    bit 5 : OCIE1B
      Indica que se debe producir una interrupción si el contador es igual que el registro OCR1B
    Bit 6 : OCIE1A
      Indica que se debe producir una interrupción si el contador es igual que el registro OCR1A
      
  En este ejercicio también haremos parpadear el led integrado cada segundo aproximadamente.
  Utilizaremos el prescaler más alto: 16384.
  Intentaremos conseguir que la interrupción se produzca cada décima de segundo.
  Así fijaremos como límite de nuestro contador 10, para obtener el segundo de parpadeo.
  
  Calculamos el valor límite:
    valor límite = frecuencia CPU / prescaler * periodo deseado - 1
    valor límite = 16.500.000 / 16.384 * 0,1 - 1 = 99.7080 ~ 100

  Importante!.
  Según la documentación, el registro TCNT1 contiene el contador del timer1 (el que estamos utilizando).
  Una vez que salta la interrupción, el contador sigue intacto. Y con la próxima interrupción se incrementará
  su valor en una unidad.
  NO se resetea sólo!.
  Si no nos encargamos nosotros de esto, estaremos recorriendo los 256 valores del contador.
  Esta es la razón por la que en la función de interrupción, lo primero que hacemos es resetear el contador 
  del timer1.
  
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
  //Activamos los cuatro primeros bits(CS10, CS11, CS12 y CS13) del registro TCCR1 para que se utilice como valor de prescaler 16384
  //Tambien se suele utilizar la siguiente nomenclatura: TCCR1=(1<<CS10) | (1<<CS11) | (1<<CS12) | (1<<CS13);
  TCCR1=B00001111;
  //Fijamos el valor de comparación 
  OCR1A = 100;
  //Registro TIMSK. bits referentes a interrupción 1:
  //  bit 2 : TOIE1 : timer1 Overflow Interrupt Enable, bit 1 = 0 = salta cuando el contador llega al valor máximo
  //  bit 5 : OCIE1B : timer1 Output Compare Interrupt Enable = 0 = salta cuando el contador sea igual a OCR1B
  //  bit 6 : OCIE1A : timer1 Output Compare Interrupt Enable = 1 = salta cuando el contador esa igual a OCR1A
  //              76543210     76543210
  TIMSK=(TIMSK & B10011011) | B01000000;
  //Volvemos a activar las interrupciones
  sei(); 
}

void loop() {
  //No tenemos nada que ejecutar en el bucle principal
}

//Función a la que se llama cuando se produce una interrupción
//Con el parámetro TIMER1_COMPA_vect indicamos que se llama cuando se el contador es igual que OCR1A
ISR(TIMER1_COMPA_vect) {
  //Reseteamos el contador del timer1
  TCNT1=0;
  
  //Aumentamos el valor del nuestro contador y si es superior al valor calculado...
  if(++myCTC>10) {
    //...lo reseteamos
    myCTC=0;
    //Cambiamos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
  }
}
