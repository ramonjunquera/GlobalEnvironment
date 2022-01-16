/*
  Autor: Ramón Junquera
  Fecha: 20191028
  Tema: Gestión de interrupciones de hardware
  Objetivo: Definición de varias interrupciones simultáneas
  Material adicional: ATtiny85, breadboard, 3x interruptor de pulsador

  Descripción:
  Basándonos en el ejemplo anterior, esta vez definiremos más de un pin para que
  genere interrupciones por hardware. Concretamente utilizaremos 3 interruptores.
  Con la pulsación de cualquiera de ellos, se cambiará el estado del led
  integrado.

  En esta ocasión tampoco hacemos caso de si el flanco es de subida o bajada.

  Puesto que utilizamos los pines 3 y 4, debemos alimentar externamente el circuito
  para que funcione correctamente.

  Puesto que la función de interrupción es única, no podemos detectar el pin que
  ha disparado la interrupción.
  
  Resultado:
  Cuando pulsamos cualquier interruptor, se cambia el estado del led integrado.
*/

#include <Arduino.h>
#include <avr/interrupt.h> //Gestión de interrupciones por hardware

//Declaración de variables globales
const byte pinLed = 1; //pin del led
const byte pinSwitch1 = 2; //pin del interruptor 1
const byte pinSwitch2 = 3; //pin del interruptor 2
const byte pinSwitch3 = 4; //pin del interruptor 3
const long int delayTime=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción
volatile long int nextIntTime=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción

//Función a la que se llama ante cualquier interrupción
//El parámetro PCINT0_vect indica que se trata de interrupciones por hardware
ISR(PCINT0_vect) {
  //Tomamos nota de la hora actual
  volatile unsigned long now=millis();
  //Si la hora actual es superior a mínimo tiempo que debe transcurrir hasta el próximo evento...
  if(now>nextIntTime) {
    //Podemos tomar esta interrupción como buena!
    //Calculamos la hora mínima en la que debe ocurrir el próximo evento
    nextIntTime=now+delayTime;
    //Cambiamos el estado del pin del led
    digitalWrite(pinLed,!digitalRead(pinLed));
  }
}

void setup() {
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Activamos las resistencias de pull-up para los pines de los interruptores
  pinMode(pinSwitch1,INPUT_PULLUP);
  pinMode(pinSwitch2,INPUT_PULLUP);
  pinMode(pinSwitch3,INPUT_PULLUP);

  //Desactivamos las interupciones mientras las configuramos
  cli();
  //Activamos las interrupciones hardware por cambio de estado de un pin
  //Esto se hace activando el bit PCIE del registro GIMSK
  //También se puede escribir como GIMSK |= _BV(PCIE);
  GIMSK |= (1<<PCIE);
  //Activamos las interrupciones hardware para los pines de los interruptores
  PCMSK |= (1<<pinSwitch1) | (1<<pinSwitch2) | (1<<pinSwitch3);
  //Reactivamos las interrupciones ahora que ya hemos terminado de configurarlas
  sei();
}

void loop() {
  //No tenemos nada que hacer en el bucle principal
  //Todo se gestiona por interrupciones
}
