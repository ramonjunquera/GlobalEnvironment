/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Lectura de estado de un pin
  Objetivo: Comprobar si el pin 1 también puede ser de entrada
  Material adicional: ATtiny85, breadboard, pulsador, led, resistencia de 200 ohmios

  Descripción:
  Hemos comprobado que de los 5 pines útiles, 4 de ellos podemos utilizarlos como
  entradas digitales. No hemos podido probar el pin 1 porque es el que nos daba 
  la señal de que todo funcionaba.
  En este ejercicio sólo comprobaremos si el pin 1 puede utilziarse como entrada
  digital.
  Para comprobar si funciona correctamente, incluiremos un led externo en el 
  pin 2, que se encenderá al pulsar el interruptor.

  Nota:
  Este proyecto NO funciona cuando suministramos la alimentación a través del USB
  porque interfiere en el estado de los pines.
  Se debe alimentar con una fuente externa a través de los pines GND & 5V

  Resultado:
  El led se enciende al pulsar el interruptor conectado al pin 1.
  
  Conclusión:
  Se pueden utilizar cualquiera de los 5 pines para lecturas digitales
*/

//Definimos los pines
const byte pinLed=2;
const byte pinSwitch=1;

void setup() { 
  //Configuramos el pin del pulsador como entrada (con resistencias internas de pullup)
  pinMode(pinSwitch,INPUT_PULLUP);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
}

void loop() {
  //Escribimos en el pin del led el estado inverso al leido del pin del pulsador
  digitalWrite(pinLed,!digitalRead(pinSwitch));
}
