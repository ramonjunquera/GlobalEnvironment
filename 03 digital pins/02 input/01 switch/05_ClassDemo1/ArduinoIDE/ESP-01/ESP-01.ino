/*
  Autor: Ramón Junquera
  Fecha: 20181031
  Tema: Interruptores
  Objetivo: Gestión de interruptor
  Material: breadboard, cables, ESP-01, regulador de tensión para breadboard, interruptor de pulsador,
    zócalo para ESP-01, fuente de alientación externa
  Descripción:
    Cuando se pulsa el interruptor cambia el estado del led integrado.
    Utilizamos la librería de gestión de interruptores.
    Es muy importante que el regulador de tensión de la breadboard saque 3.3V.
  Resultado:
    El interruptor cambia el estado del led integrado.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Librería de gestión de interruptores

//Definición de constantes globales
const byte pinSwitch = 0;

//Devinición de variables globales
RoJoSwitch mySwitch = RoJoSwitch(pinSwitch); //Objeto de gestión del interruptor

void setup()
{
  //Configuramos el pin del led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  //Si se ha detectado una pulsación en el interruptor...cambiamos el estado del led
  if(mySwitch.pressed()) digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
}

