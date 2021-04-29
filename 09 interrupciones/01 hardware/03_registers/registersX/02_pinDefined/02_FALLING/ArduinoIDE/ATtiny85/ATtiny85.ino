/*
  Autor: Ramón Junquera
  Fecha: 20191028
  Tema: Gestión de interrupciones de hardware
  Objetivo: Detección del pin que produce una interrupción y filtro por flanco
  Material adicional: ATtiny85, breadboard, 2x interruptor de pulsador, 2x led

  Descripción:
  Basado en el ejercicio anterior, además de detectar el pin que genera una
  interrupción, también filtraremos por tipo y sólo atenderemos las que tengan
  flancos de bajada (FALLING).
  Ahora al soltar el pulsador no debe cambiar el estado del led.

  Nota:
  Como tenemos conectados elementos (leds) a los pines del USB, la transferencia del 
  programa no se hará correctamente si no los soltamos.
  
  Resultado:
  Cada interruptor afecta a su propio led y sólo cambian de estado al pulsar, no al soltar.
*/

#include <Arduino.h>
#include <avr/interrupt.h> //Gestión de interrupciones por hardware

//Declaración de variables globales
const byte pinLed1 = 3; //pin del led 1
const byte pinLed2 = 4; //pin del led 2
const byte pinSwitch1 = 0; //pin del pulsador 1
const byte pinSwitch2 = 2; //pin del pulsador 2
const long int delayTime1=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción del pulsador 1
const long int delayTime2=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción del pulsador 2
volatile long int nextIntTime1=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción del pulsador 1
volatile long int nextIntTime2=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción del pulsador 2
volatile byte lastStatus=(1<<pinSwitch1) | (1<<pinSwitch2); //Último estado de los pines digitales = apagados

//Función llamada cuando se produce una interrución generada por el pulsador 1
//con flanco de bajada
void IntFallingSwitch1() {
  //Tomamos nota de la hora actual
  volatile unsigned long now=millis();
  //Si la hora actual es superior al mínimo tiempo que debe transcurrir hasta
  //el próximo evento del pulsador 1...
  if(now>nextIntTime1) {
    //Podemos dar la interrupción como válida
    //Calculamos la hora mínima en la que debe ocurrir el próximo evento
    nextIntTime1=now+delayTime1;
    //Cambiamos el estado del pin del led 1
    digitalWrite(pinLed1,!digitalRead(pinLed1));
  }
}

//Función llamada cuando se produce una interrución generada por el pulsador 2
//con flanco de bajada
void IntFallingSwitch2() {
  //Tomamos nota de la hora actual
  volatile unsigned long now=millis();
  //Si la hora actual es superior al mínimo tiempo que debe transcurrir hasta
  //el próximo evento del pulsador 2...
  if(now>nextIntTime2) {
    //Podemos dar la interrupción como válida
    //Calculamos la hora mínima en la que debe ocurrir el próximo evento
    nextIntTime2=now+delayTime2;
    //Cambiamos el estado del pin del led 2
    digitalWrite(pinLed2,!digitalRead(pinLed2));
  }
}

//Función a la que se llama ante cualquier interrupción
//El parámetro PCINT0_vect indica que se trata de interrupciones por hardware
ISR(PCINT0_vect) {
  //Tomamos nota del estado de todos los pines
  //Cada bit corresponde a un pin
  //Y lo filtramos sólo por los pines que nos interesan
  byte currentStatus=(PINB & ((1<<pinSwitch1) | (1<<pinSwitch2)));
  //Hacemos un XOR entre el último estado y el actual para que sólo queden los 
  //bits que han cambiado
  byte changedStatus = currentStatus ^ lastStatus;
  //A partir de ahora el último estado será el actual
  lastStatus=currentStatus;

  //Si ha cambiado de estado el pin del pulsador 1...
  if(changedStatus & (1<<pinSwitch1)) {
    //Si el estado actual es LOW...
    if(!(currentStatus & (1<<pinSwitch1))) {
      //...es porque antes era HIGH y se trata de una interrupción FALLING
      IntFallingSwitch1();
    }
  }

  //Si ha cambiado de estado el pin del pulsador 2...
  if(changedStatus & (1<<pinSwitch2)) {
    //Si el estado actual es LOW...
    if(!(currentStatus & (1<<pinSwitch2)))
    {
      //...es porque antes era HIGH y se trata de una interrupción FALLING
      IntFallingSwitch2();
    }
  }
}

void setup() {
  //Configuramos los pines de los leds como salidas
  pinMode(pinLed1,OUTPUT);
  pinMode(pinLed2,OUTPUT);
  //Activamos las resistencias de pull-up para los pines de los pulsadores
  pinMode(pinSwitch1,INPUT_PULLUP);
  pinMode(pinSwitch2,INPUT_PULLUP);

  //Desactivamos las interupciones mientras las configuramos
  cli();
  //Activamos las interrupciones hardware por cambio de estado de un pin
  //Esto se hace activando el bit PCIE del registro GIMSK
  //También se puede escribir como GIMSK |= _BV(PCIE);
  GIMSK |= (1<<PCIE);
  //Activamos las interrupciones hardware para los pines de los pulsadores
  PCMSK |= (1<<pinSwitch1) | (1<<pinSwitch2);
  //Reactivamos las interrupciones ahora que ya hemos terminado de configurarlas
  sei();
}

void loop() {
  //No tenemos nada que hacer en el bucle principal
  //Todo se gestiona por interrupciones
}
