/*
  Autor: Ramón Junquera
  Fecha: 20191025
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar el timer0 en ATtiny85
  Material adicional: ATTiny85

  Descripción:
  La placa ATtiny85 tiene dos timers. El 0 y el 1.
  Ambos tienen una resolución de 8 bits.
  Por lo tanto, el contador podrá tener 256 valores distintos.

  La gestión del timer0 y timer1 es parecida, pero no idéntica.
  En este ejercicio trararemos la gestión del timer0.

  En un timer, se aumenta el valor del contador una vez cada ciclo de reloj.
  El procesador ATtiny85 contiene un procesador que funciona a una frecuencia de 16MHz.
  Esto son 16 millones de ciclos de reloj cada segundo.
  Si calculamos el periodo de un ciclo tenemos:
    Periodo = 1 / frecuencia = 1 / 16.000.000 = 0,000000063s = 0,000063ms = 0,063µs = 63ns
  Cada 63 nanosegundos se aumenta el contador del timer.

  Podríamos hacer que salte la interrupción cada 63 nanosegundos, pero no sería práctico
  porque es un valor demasiado pequeño.

  Cuando el contador de un timer llega a su máximo valor (255), el siguiente ciclo lo incrementará
  y lo hará volver a cero.

  El tiempo que tarda el timer en recorrer todos sus valores es:
    tiempo total = tiempo de cambio * número de valores = 63ns * 256 = 16128ns = 16µs
  Auque podamos hacer saltar una interrupción cada vez que el contador alcance su máximo valor, 16µs sigue
  siendo un valor muy pequeño.

  Para poder tener valores más manejables, se ha inventado el prescaler.
  El prescaler es un múltiplo de los ciclos de reloj.
  Permite que el contador aumente sólo cada cierto número de ciclos de reloj.
  Hasta ahora hemos hecho los cálculos con prescaler = 1.

  El prescaler del timer0 de ATtiny85 permite tomar los siguiente valores: 1,8,64,256 o 1024.
  Si tomamos el mayor (1024) y hacemos saltar la interrupción cada vez que el contador alcance su máximo
  valor, tendríamos un periodo de:
    periodo = prescaler*resolución/frecuencia = 1024*256/16000000 = 0,016384s ~ 16ms
  Este valor es algo más manejable.

  Veremos algunos registros para gestionar el timer0:
    TCCR0A = Timer/Counter Control Register A
    TCCR0B = Timer/Counter Control Register B
    TIMSK = Timer/Counter Interrupt Mask Register

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

    Este registro permite configurar el comportamiento de las interrupciones del
    timer0:
    - En qué valor del contador debe saltar la interrupción
    - En qué sentido se recorre el contador
    - etc
    Habitualmente usaremos su valor por defecto: cero

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

    La primera de las posibilidades sirve para que el contador no incremente su valor (y no salte nunca la interrupción).
    Las dos últimas no nos interesan demasiado porque sirven para utilizar un reloj externo para incrementar el contador.
    
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
    Por ahora sólo atenderemos al bit 1 : TOIE0
      Indica si el contador del timer0 debe hacer saltar la interrupción cuando llegue al máximo
      valor que puede contener.
 
  Con la información actual, nos centraremos en el proyecto.
  Crearemos un programa que haga parpadear el led integrado de la placa mediante interrupciones internas.
  Utilizaremos el timer0.
  Usaremos un prescaler.

  Al principio hemos calculado que usando un presacaler de 1024 (el máximo permitido para este timer) conseguíamos un periodo
  de unos 16ms.
  Este valor es insuficiente para hacer parpadear un led. Nos costaría mucho distinguir si parpadea.
  Necesitamos que sea mayor, pero el problema es que ya estamos utilizando el mayor de los prescalers.
  Pasa solucionarlo, crearemos una variable que haga de contador adicional.
  Cada vez que salte la interrupción aumentaremos el valor de nuestro contador y si alcanza un valor adecuado,
  entonces cambiaremos el estado del led.

  Si queremos que el led parpadee cada segundo (aproximadamente), necesitaremos que nuestro contador alcance el siguiente valor:
    k = periodo deseado/periodo de timer = 1000ms/33ms = 30,3 ~ 30
    k = periodo deseado/periodo de timer = 1000ms/16ms = 62,5 ~ 63

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
  //Utilizamos el valor por defecto del registro TCCR0A. No quremos cambiar la configuración del timer0
  TCCR0A=0;
  //Fijaremos un prescaler de 1024: CS02=1 + CS01=0 + CS00=1
  TCCR0B=B101;
  //Registro TIMSK. bits referentes a interrupción 0:
  //  bit 1 : TOIE0 : timer0 Overflow Interrupt Enable        = 1 = salta cuando el contador llega al valor máximo
  //  bit 3 : OCIE0B : timer0 Output Compare Interrupt Enable = 0 = salta cuando el contador sea igual a OCR0B
  //  bit 4 : OCIE0A : timer0 Output Compare Interrupt Enable = 0 = salta cuando el contador esa igual a OCR0A
  //              76543210     76543210
  TIMSK=(TIMSK & B11100101) | B00000010;
  //Volvemos a activar las interrupciones
  sei(); 
}

void loop() {
  //No tenemos nada que ejecutar en el bucle principal
}

//Función a la que se llama cuando se produce una interrupción
//Con el parámetro TIMER0_OVF_vect indicamos que se llama cuando se produce el desbordamiento (overflow) del
//contador del timer 0
ISR(TIMER0_OVF_vect) {
  //Aumentamos el valor del nuestro contador y si es superior al valor calculado...
  if(++myCTC>63) {
    //...lo reseteamos
    myCTC=0;
    //Cambiamos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
  }
}
