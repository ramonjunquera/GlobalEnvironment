/*
  Autor: Ramón Junquera
  Fecha; 20191028
  Tema: Gestión de interrupciones de hardware
  Objetivo: Descripción de configuración de interrupciones de ATtiny85 por registros
  Material adicional: ATtiny85, breadboard, interruptor de pulsador

  Descripción:
  Aunque la gestión de la configuración por registros sea más compleja, a veces merece
  la pena. En esta ocasión conseguimos que todos los pines del ATtiny85 puedan generar una
  interrupción por hardware al cambiar su estado.

  El primer inconveniente es que el nombre de la función a la que llama la interrupción es
  fijo.
  
  El segundo inconveniente es que no se pueden definir distintos tipos de interrupciones
  dependiendo del flanco: CHANGE, FALLING, RAISING, ...
  Sólo existe el tipo CHANGE, que produce la interrupción ante cualquier cambio de estado.
  Podemos comprobar esto porque cuando pulsamos y mantenemos pulsado, generamos una
  interrupción que hace cambiar el estado del led. Y cuando soltamos (después de un rato)
  se genera otra interrupción que vuelve a cambiar el estado del led.

  Para evitar el efecto rebote, hemos definido un tiempo en el que no se tendrán en cuenta
  las interrupciones generadas.

  En este ejemplo el interruptor está conectado al pin 0, pero se puede comprobar que 
  funciona con cualquiera.
  Sólo recordar que los pines 3 y 4 se utilizan por la conexión USB. Si queremos usar alguno
  de esos pines, se tendrá que alimentar el circuito externamente.
  
  Resultado:
  Cuando pulsamos el interruptor, se cambia el estado del led integrado.
*/

#include <Arduino.h>
#include <avr/interrupt.h> //Gestión de insterrupciones de hardware

//Declaración de variables globales
const byte pinLed = 1; //pin del led
const byte pinSwitch = 0; //pin al que está conectado el interruptor. Puede ser cualquiera
const long int delayTime=400; //tiempo de demora en milisegundos antes de comprobar una nueva interrupción
volatile long int nextIntTime=0; //tiempo a partir del cual se puede tener en cuenta la próxima interrupción

//Función a la que se llama ante cualquier interrupción
//El parámetro PCINT0_vect indica que se trata de interrupciones por hardware
ISR(PCINT0_vect) {
  //Tomamos nota de la hora actual
  volatile unsigned long now=millis();
  //Si la hora actual es superior al mínimo tiempo que debe transcurrir hasta el próximo evento...
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
  //Activamos las resistencias de pull-up para el pin de interruptor
  //Por defecto el pin del interruptor estará en HIGH y cuando se pulse en LOW
  pinMode(pinSwitch,INPUT_PULLUP);

  //Desactivamos las interupciones mientras las configuramos
  cli();
  //Activamos las interrupciones hardware por cambio de estado de un pin
  //Esto se hace activando el bit PCIE del registro GIMSK
  //También se puede escribir como GIMSK |= _BV(PCIE);
  GIMSK |= (1<<PCIE);
  //Activamos las interrupciones hardware para el pin seleccionado
  //Esto se hace activando el bit correspondiente al pin en el registro PCMSK
  //Por suerte el orden de los bits es igual que el orden de los pines
  PCMSK |= (1<<pinSwitch);
  //Reactivamos las interrupciones ahora que ya hemos terminado de configurarlas
  sei();
}

void loop() {
  //No tenemos nada que hacer en el bucle principal
  //Todo se gestiona por interrupciones
}
