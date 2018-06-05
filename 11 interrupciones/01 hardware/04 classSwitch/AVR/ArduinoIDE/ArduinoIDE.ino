/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Utilizar interrupciones con la librería de gestión de interruptores
  Material: breadboard, cables, interruptor de pulsador, placa Arduino

  Descripción:
  Ahora que ya sabemos gestionar interrupciones, utilizaremos la librería de gestión
  de interruptores (RoJoSwitch) para llamarla sólo cuando se produzca un cambio de
  estado en el pin del interruptor, en vez de contínuamente.

  El led integrado en placa cambiará de estado cuando se detecte una pulsación en el
  interruptor.
  Recordemos que la librería ya gestiona el efecto rebote.
  
  Resultado:
  El led integrado en placa cambia de estado cuando se pulsa el interruptor
*/

#include <Arduino.h>
#include "RoJoSwitch.h"

//Declaración de variables globales
const byte pinLed = 13; //Pin del led integrado
const byte pinSwitch = 2; //pin al que está conectado el interruptor
const byte intSwitch = 0; //Interrupción correspondiente al pin del interruptor

//Declaramos el objeto de gestión del interruptor
RoJoSwitch mySwitch(pinSwitch);

void IntSwitch()
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Si el interruptor ha sido pulsado...cambiamos el estado del led
  if(mySwitch.pressed()) digitalWrite(pinLed,!digitalRead(pinLed));
}

void setup()
{
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Definimos la interrupción
  //Será la correspondiente al pin 2
  //Se llamará a la función IntSwitch
  //Ocurrirá cuando el estado del pin cambie
  attachInterrupt(intSwitch,IntSwitch,CHANGE);
}

void loop()
{
  //Nada especial que hacer aquí
}



