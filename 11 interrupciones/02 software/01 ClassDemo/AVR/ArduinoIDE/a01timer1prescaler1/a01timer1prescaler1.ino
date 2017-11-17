/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
  Una interrupción interna o timer es un contador que tras alcanzar un valor límite predefinido, llama
  a la ejecución de una función.
  El contador comienza siempre desde cero, y aumenta secuencialmente su valor a un ritmo que le marca el
  reloj de la placa Arduino.
  Los Arduino UNO, Mega o Nano tienen un reloj de 16 MHz. Quiere decir que realiza 16 millones de ciclos
  en cada segundo. Por lo tanto, el contador del timer incrementa su valor 16 millones de veces por segundo.
  Haciendo un cálculo rápido tenemos:
    Periodo = 1 / frecuencia = 1 / 16.000.000 = 0,0000000625s = 0,0000625ms = 0,0625µs = 62,5ns
  Cada 62,5 nanosegundos se aumenta el contador de la interrupción en un Arduino UNO.

  El valor más pequeño que se puede dar al límite de un contador es 1. Es decir, cada vez que de aumente
  el contador, saltará la interrupción (se provocará un desbordamiento y se activará el flag del timer).
  Así que podríamos lanzar la ejecución de la función de la interrupción cada 62,5 ns.
  En la práctica esto no es cierto, porque la función necesita unos cuantos ciclos de reloj para poder hacer algo.
  El aumento del contador es algo de lo que no tenemos que preocuparnos nosotros. Lo hace automáticamente 
  el procesador. Incluso el reseteo del contador y vuelta a comenzar es automático. Nosotros nos ocuparemos
  de indicarle cuántos ciclos debe contar para que salte la interrupción y algún detalle más.

  El número de timers que es capaz de gestionar una placa depende de su procesador.
  Un Arduino UNO o Nano (chips Atmega 168 y 328) tiene 3 timers independientes. En cambio una MEGA tiene los 3
  anteriores y 3 más (total 6).
  Los timers se identifican por un número consecutivo comenzando por el 0.
  El timer 0 es un temporizador de 8 bits que se encarga de hacer funcionar correctamente las funciones delay() y
  millis(). Esto quiere decir que si utilizarmos este timer, las dos funciones indicadas dejarán de funcionar
  correctamente.
  El timer 1 es un temporizador de 16 bits utilizado por la librería servo.
  El timer 2 es un temporizador de 8 bits utilizado por la función tone() y el PWM de los pines 3 y 11
  Los timers 3, 4 y 5 sólo los tienen los chips Atmega 1280 y 2560 (Arduino MEGA). Son de 10 bits y funcionan de la
  misma manera que el timer 1.

  El número de bits de un timer indican el tamaño del máximo valor a contar.
  El mayor valor que podemos almacenar en 8 bits es 2^8 -1 = 255.
  Con 16 bits tenemos 2^16 -1 = 65535.

  Así que con un timer de 8 bits, lo más lento que podríamos conseguir que salte una interrupción sería:
  Tiempo = Periodo * ciclos = 62,5ns * 255 ~ 15.937ns ~ 16µs
  Si el timer es de 16 bits:
  Tiempo = Periodo * ciclos = 62,5ns * 65.535 ~ 4.095.937ns ~ 4.096µs ~ 4,1ms
  Así que usando el timer más flexible (el 1 por tener 16 bits), podemos consegir que la interrupción se produzca cada
  4,1 milisegundos. Esto sigue siendo demasiado rápido.

  Para facilitar las cosas se ha creado el concepto de prescaler. El prescales en un número por el que se multiplica el
  valor máximo del contador para tener más flexibilidad y aumentar periodo de ejecución de la función de la interrupción.
  Hasta ahora, en los cálculos que habíamos hecho, suponíamos como valor de prescaler = 1.
  Pero el prescaler puede tomar los siguiente valores: 1,8,64,256 o 1024.
  Gracias a esto, los 4,1ms de antes se convierten ahora en 4,1*1024 ~ 4.198ms ~ 4,2s. Esto comienza a ser un valor 
  manejable.

  Como ya vemos que el timer más flexible es el 1 y el impacto de utilizarlo no es demasiado (a no ser que utilicemos la
  librería servo), utilizaremos timer 1 para hacer los siguientes ejemplos.
  
  Un timer se programa gracias a 5 registros (5 bytes). Para el timer 1 los registros son: TCCR1A, TCCR1B, TIMSK, OCR1AH y OCR1AL
  Los dos primeros se denominan TCCR =  Timer Counter Control Register
  El tercero TIMSK = Timer/Counter Interrupt Mask Register
  Los dos últimos son los bytes alto y bajo de un valor de 16bits llamado OCR1A.

  El registro TCCR1A sirve para definir el modo de generación de la onda. No lo utilizaremos. Nos parecen correctos sus
  valores de forma de onda.
  El registro TCCR1B define el prescaler. Esto sí es importante para nosotros.
  El registro TIMSK define si la interrupción está funcionando y si ya ha saltado.
  El valor OCR1A sirve para almacenar el CTC, del que hablaremos en próximos ejercicios.

  Hablemos del registro TCCR1B.
  El valor por defecto de este registro es 0 (sus 8 bits a 0).
  Sus 3 bits más bajos (0,1 y 2) son los que definen el prescaler.
  Cada uno de esos bits tiene nombre:
    bit 0 = CS10
    bit 1 = CS11
    bit 2 = CS12
  Esta es la tabla de posibles valores y su funcionalidad:

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
  
  La primera de las posibilidades sirve para que el contador no incremente su valor (y no salte nunca la interrupción)
  Las dos últimas no nos interesan demasiado porque sirven para utilizar un reloj externo para incrementar el contador.
  
  Hablemos del registro TIMSK1.
  El valor por defecto de este registro es 0 (sus 8 bits a 0).
  Los bits que más nos interesan son el 0 y el 1.

  El bit 0, también llamado TOIE1, es el que indica si el contador de timer debe hacer saltar la interrupción cuando
  llegue al máximo valor que puede contener. 65535 para los timers de 16 bits y 255 para los de 8 bits.
  
  El bit 1, también llamado OCIE1A, permite hacer saltar la interrupción aunque el contador no haya alcanzado su valor
  máximo. Hablaremos de esto más adelante. Por ahora no lo utilizamos.

  En este ejercicio haremos una prueba muy simple con los timers.

  Usaremos el timer 1 (16 bits).
  Utilizaremos la frecuencia de reloj. Prescaler = 1.
  Cada vez que salte la interrupción cambiaremos el estado del pin seleccionado.
  Según hemos calculado antes, esto ocurrirá cada 4,1ms aproximadamente.

  El pin seleccionado será el 13 que ya tiene asociado el led de placa, para no tener que añadir elementos al circuito.

  Sólo tenemos un problema.
  Si un ciclo de encendido/apagado tarda 4,1*2=8,2ms
  En un segundo tenemos 1000ms/8,2ms~122 ciclos = 122 hertzios
  Nuestro ojo no es capaz de diferenciar esta frecuencia. Es demasiado alta.
  Nos parecerá que el led está encendido continuamente.
  Sólo podremos notar una disminución de la intensidad (porque sólo está encendido la mitad del tiempo).
  Pero para hacer la prueba de cómo programar una interrupción, ya es suficiente.
  
  Resultado:
  El led integrado en placa parpadea a 122 hertzios (imperceptible)
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
  //Activamos el primer bit (CS10) del registro TCCR1B para que se utilice como valor de prescaler 1 (no prescaling)
  //El contador comienza a funcionar.
  //Para activar un sólo bit habitualmente se utiliza la siguiente nomenclatura: TCCR1B=(1<<CS10);
  TCCR1B=1;
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

