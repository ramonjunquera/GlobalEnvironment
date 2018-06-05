/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Demostración de las capacidades de la clase RoJoSwitch
  Material adicional: breadboard, interruptor, 4 leds, 4 resistencias de 220 ohmios, cables, Arduino UNO

  Descripción:
  Tenemos 4 leds y un interruptor, que cuando se pulsa hace que se encienda el siguiente led.
  Comienza con el primero de los leds encendido.
  Cuando termina con el último vuelve a empezar.
  
  Aprovechamos la funcionalidad de la librería RoJoSwitch.
  
  La novedad es que guardamos los pines de los leds en un array.
  
  Resultado:
  El interruptor sirve para que se encienda el siguiente led.

  Nota:
  Puesto que sólo encendemos un led cada vez, podríamos simplificar el circuito y utilizar una única
  resistencia para todos los leds.
*/

#include <Arduino.h>
#include "RoJoSwitch.h"

const byte pinSwitch=2;
const byte pinLeds[]={3,4,5,6}; //Pin de cada led

//Declaramos los objetos de gestión de los interruptores
RoJoSwitch mySwitch(pinSwitch);

byte activeLed=0;

void setup()
{
  //Recorremos todos los leds
  for(byte i=0;i<4;i++)
  {
    //Configuramos el pin como salida
    pinMode(pinLeds[i],OUTPUT);
    //Lo apagamos
    digitalWrite(pinLeds[i],LOW);
  }
  //Encendemos el led inicial
  digitalWrite(pinLeds[activeLed],HIGH);
}

void loop()
{
  //Si se ha pulsado el interruptor...
  if(mySwitch.pressed())
  {
    //Apagamos el led activo
    digitalWrite(pinLeds[activeLed],LOW);
    //Aumentamos el indice del led activo
    activeLed++;
    //Sólo tenemos 4 leds
    activeLed&=0x03;
    //Encendemos el nuevo led activo
    digitalWrite(pinLeds[activeLed],HIGH);
  }
}
