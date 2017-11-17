/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
  Continuamos con la programación de los timers.
  En el ejercicio anterior conseguíamos programar el timer 1 sin prescaler, pero teníamos el problema de que la frecuencia
  de parpadeo era tan alta que nuestros ojos no eran capaces de detectarlo.

  Repetiremos el ejercicio, pero esta vez le pondremos el máximo prescaler (1024).
  Recuperando los cálculos del ejercicio anterior...

  El reloj de la placa es de 16 MHz.
  Esto son 16 millones de ciclos por segundo.
  El periodo (tiempo que transcurre entre dos ciclos) es de 
    periodo = 1 / frecuencia = 1 / 16.000.000 = 0,0000000625s= 0,0000625ms = 0,0625µs = 62,5ns
  El timer 1 es de 16 bits.
  En 16 bits, el mayor múmero que se puede almacenar es (2^16)-1=65.535
  Por lo tanto el timer 1 sin prescaler provocará la interrupción cada 65.535 ciclos.
  En segundos 62,5ns * 65.535 = 4095937,5ns = 4095,9375µs = 4,0959375ms ~ 4,1ms
  Si aplicamos un prescaler de 1024 se activará cada 4,1ms * 1024 = 4198,4ms = 4,1984s ~ 4,2s

  Como nota informatica calcularemos el periodo de cada uno de los prescalers para un timer de 16 bits

  prescaler=1    periodo=prescaler*MaxValue/frecuencia=1*65535/16000000=0,0040959375s=4,0959375ms~4,1ms
  prescaler=8    periodo=prescaler*MaxValue/frecuencia=8*65535/16000000=0,0327675s=32,7675ms~33ms
  prescaler=64   periodo=prescaler*MaxValue/frecuencia=64*65535/16000000=0,26214s~0,3s
  prescaler=256  periodo=prescaler*MaxValue/frecuencia=256*65535/16000000=1,04856s~1s
  prescaler=1024 periodo=prescaler*MaxValue/frecuencia=1024*65535/16000000=4,19424s~4,2s

  Recordamos la tabla de códigos de los distintos prescalers:
  
     bit   2    1    0
  Nombre CS12 CS11 CS10 Funcionalidad
           0    0    0  No clock source (Timer/Counter stopped)
           0    0    1  clock/1 (No prescaling)
           0    1    0  clock/8 (From prescaler)
           0    1    1  clock/64 (From prescaler)
           1    0    0  clock/256 (From prescaler)
           1    0    1  clock/1024 (From prescaler)
           1    1    0  External clock source on T1 pin. Clock on falling edge
           1    1    1  External clock source on T1 pin. Clock on rising edge

  Conclusión el led cambiará de estado cada 4,2 segundos
    
  Resultado:
  El led integrado en placa parpadea cada 4,2 segundos.
*/

#include <Arduino.h>

//Como trabajaremos directamente con los registros del procesador, incluiremos las librerías de gestión del fabricante
//del procesador. No hacemos referencia a librerías de Arduino.
#include <avr/io.h>
#include <avr/interrupt.h>

//Declaración de variables globales
const byte pinLed = 13; //Pin al que está conectado el led

void setup()
{
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Deshabilitamos las interrupciones globales. No queremos que salte ninguna interrupción mientras las configuramos.
  cli();
  //Fijamos el registro TCCR1A a 0 para que se mantengan los valores por defecto de la forma de onda
  TCCR1A=0;
  //Activamos el primer y tercer bit (el CS10 y el CS12) del registro TCCR1B para que se utilice como valor de prescaler 1024
  //El contador comienza a funcionar.
  //Habitualmente la nomenclatura es TCCR1B=(1<<CS10) | (1<<CS12);
  TCCR1B=B101;
  //En el registro TIMSK1 activamos el primer bit (TOIE1) para que salte la interrupción al llegar al máximo
  //Habitualmente se utiliza la siguiente nomenclatura: TIMSK1=(1<<TOIE1);
  TIMSK1=1;
  //Volvemos a activar las interrupciones
  sei();
}

void loop()
{
  //No tenemos nada que ejecutar en el bucle principal
}

ISR(TIMER1_OVF_vect)
{
  //Función a la que se llama cuando se produce una interrupción
  //Con el parámetro TIMER1_OVF_vect indicamos que se llama cuando se produce el desbordamiento (overflow) del
  //contador del timer 1

  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
}

