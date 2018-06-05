/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Versión: 20180605
  Objetivo: Utilizar interrupciones con la librería de gestión de interruptores
  Material: placa ESP8266, breadboard, 3 pulsadores, led RGB
  Descripción:
    Utilizando un led RGB y tres pulsadores, gestionaremos las pulsaciones mediante
    la clase RoJoSwitch y utilizaremos interrupciones para llamar a las clases.
    El led RGB es de ánodo común.
  Resultado:
    Cada pulsador enciende un color
*/
#include <Arduino.h>
#include "RoJoSwitch.h"

//Declaración de constantes globales
const byte pinSwitchR = D1;
const byte pinSwitchG = D2;
const byte pinSwitchB = D3;
const byte pinLedR = D6;
const byte pinLedG = D0;
const byte pinLedB = D5;

//Declaración de objetos de gestión de interruptores
RoJoSwitch switchR(pinSwitchR);
RoJoSwitch switchG(pinSwitchG);
RoJoSwitch switchB(pinSwitchB);

void intSwitchR()
{
  //Función a la que se llama cuando cambia el estado del interruptor R

  //Si el interruptor ha sido pulsado...cambiamos el estado del led
  if(switchR.pressed()) digitalWrite(pinLedR,!digitalRead(pinLedR));
}
void intSwitchG()
{
  //Función a la que se llama cuando cambia el estado del interruptor G

  //Si el interruptor ha sido pulsado...cambiamos el estado del led
  if(switchG.pressed()) digitalWrite(pinLedG,!digitalRead(pinLedG));
}
void intSwitchB()
{
  //Función a la que se llama cuando cambia el estado del interruptor B

  //Si el interruptor ha sido pulsado...cambiamos el estado del led
  if(switchB.pressed()) digitalWrite(pinLedB,!digitalRead(pinLedB));
}

void setup()
{ 
  //Configuramos los pines del led RGB como salidas
  pinMode(pinLedR,OUTPUT);
  pinMode(pinLedG,OUTPUT);
  pinMode(pinLedB,OUTPUT);
  //Definimos la interrupciones
  attachInterrupt(pinSwitchR,intSwitchR,CHANGE);
  attachInterrupt(pinSwitchG,intSwitchG,CHANGE);
  attachInterrupt(pinSwitchB,intSwitchB,CHANGE);
}

void loop()
{
  //Nada especial que hacer aquí
}
