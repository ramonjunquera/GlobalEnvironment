/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Feha: 20181010
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
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

