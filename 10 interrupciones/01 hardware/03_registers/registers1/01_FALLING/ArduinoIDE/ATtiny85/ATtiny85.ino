/*
  Autor: Ramón Junquera
  
  Tema: Gestión de interrupciones de hardware
  
  Objetivo: Detección del flanco del cambio de estado de un pin
  
  Material adicional: ATtiny85, breadboard, 3x interruptor de pulsador

  Descripción:
  Nos basamos en los ejemplos anteriores para desarrollar ahora un programa que sea capaz
  de detectar el flanco del cambio de un pin.
  El led integrado sólo debe cambiar su estado ante una pulsación. No al soltar.
  Esto es, cuando sea un cambio FALLING, de HIGH a LOW.

  Contaremos con un sólo interruptor.

  Controlamos el efecto rebote, pero sólo para el flanco de bajada.
  Podría ser que al soltar el pulsador se produzca un efecto rebote que haga cambiar el
  estado del led. Pero por lo general, sólo cambia el estado al pulsar. No al soltar.

  Para distinguir el flanco, tras la interrupción, comprobamos el valor del pin.
  Si se encuentra en estado LOW es que antes de la interrupción estaba en HIGH.
  Esto lo podemos tomar como cierto si no dejamos pasar mucho tiempo desde que
  ocurre la interrupción hasta que se lee el estado del pin.
  
  Resultado:
  El led sólo cambia de estado cuando se pulsa el interruptor, no cuando se suelta.
*/

#include <Arduino.h>
#include <avr/interrupt.h> //Gestión de interrupciones por hardware

//Declaración de variables globales
const byte pinLed = 1; //pin del led
const byte pinSwitch = 2; //pin del interruptor
const long int delayTime=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción
volatile long int nextIntTime=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción

//Función a la que se llama ante cualquier interrupción
//El parámetro PCINT0_vect indica que se trata de interrupciones por hardware
ISR(PCINT0_vect) {
  //Tomamos nota de la hora actual
  volatile unsigned long now=millis();
  //Si la hora actual es superior a mínimo tiempo que debe transcurrir hasta el próximo evento...
  if(now>nextIntTime) {
    //Si el estado actual del pin del interruptor es LOW...
    if(!digitalRead(pinSwitch)) {
      //...es que antes estaba en HIGH. Por lo tanto es un flanco de bajada
      //Debemos tener en cuenta esta interrupción
      //Calculamos la hora mínima en la que debe ocurrir el próximo evento
      nextIntTime=now+delayTime;
      //Cambiamos el estado del pin del led
      digitalWrite(pinLed,!digitalRead(pinLed));
    }
  }
}

void setup() {
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Activamos las resistencias de pull-up para el pin del interruptor
  pinMode(pinSwitch,INPUT_PULLUP);

  //Desactivamos las interupciones mientras las configuramos
  cli();
  //Activamos las interrupciones hardware por cambio de estado de un pin
  //Esto se hace activando el bit PCIE del registro GIMSK
  //También se puede escribir como GIMSK |= _BV(PCIE);
  GIMSK |= (1<<PCIE);
  //Activamos las interrupciones hardware para el pin del interruptor
  PCMSK |= (1<<pinSwitch);
  //Reactivamos las interrupciones ahora que ya hemos terminado de configurarlas
  sei();
}

void loop() {
  //No tenemos nada que hacer en el bucle principal
  //Todo se gestiona por interrupciones
}
