/*
  Autor: Ramón Junquera
  Fecha: 20210215
  Tema: Led RGB
  Objetivo: Aprender a controlar un led RGB

  Descripción:
  Tenemos un led RGB, un potenciómetro y un botón.
  El potenciómetro nos permite seleccionar la intensidad de cada color base.
  El botón nos permite seleccionar el color base.

*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de pulsadores

//Declaración de constantes globales
const byte pinLeds[]={11,10,9};
const byte pinLedsVCC=12;
const byte pinButton=2;
const byte pinAnalogIn=A0;
//Declaración de variables globales
RoJoSwitch button(pinButton);
byte ledSelected=0;

void setup() {
  //Los pines de VCC de los leds RGB siempre deben estar en HIGH
  pinMode(pinLedsVCC,OUTPUT); digitalWrite(pinLedsVCC,HIGH);
  //Inicialmente todos los leds están apagados
  for(int8_t i=2;i>=0;i--) digitalWrite(pinLeds[i],HIGH);
}

void loop() {
  analogWrite(pinLeds[ledSelected],analogRead(pinAnalogIn)/4);
  if(button.pressed()) if(++ledSelected>2) ledSelected=0;
}
