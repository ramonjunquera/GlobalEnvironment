/*
  Autor: Ramón Junquera
  Fecha: 20191028
  Tema: Gestión de interrupciones de hardware
  Objetivo: Detección del pin que produce una interrupción
  Material adicional: ATtiny85, breadboard, 2x interruptor de pulsador, 2x led

  Descripción:
  En ejercicios anteriores teníamos el problema de que sólo hay una función para
  las interrupciones de hardware. Y si tenemos varios pines que pueden provocar
  interrupción, no somos capaces de identificarlos.

  En el programa actual solventamos el problema.

  Lo que hacemos es tomar el valor del registro PINB, en la que cada uno de sus
  bits corresponde con el estado de cada uno de los pines digitales.

  Este valor lo aislamos, dejando sólo el valor de los bits que estamos tratando y 
  después lo comparamos con el valor tomado la vez anterior.
  Hacemos un XOR para que sólo queden encendidos los bits que hayan sufrido
  algún cambio.
  Después escribiremos funciones independientes para gestionar las posibles 
  interrupciones provocadas por cada pin.

  El resultado es que tenemos 2 interruptores y dos leds.
  Cada uno de los interruptores cambia el estado de un led.

  La gestión del efecto rebote para cada pin es independiente.
  
  Resultado:
  Cada interruptor afecta a su propio led, aunque ambos produzcan interrupciones de hardware.
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
void IntSwitch1() {
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
void IntSwitch2() {
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
    //Hay una interrupción generada por el pin del pulsador 1
    //Llamamos a la función que gestiona esta interrupción
    IntSwitch1();
  }

  //Si ha cambiado de estado el pin del pulsador 2...
  if(changedStatus & (1<<pinSwitch2)) {
    //Hay una interrupción generada por el pin del pulsador 2
    //Llamamos a la función que gestiona esta interrupción
    IntSwitch2();
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
