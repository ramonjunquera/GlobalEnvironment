/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Copiar el estado de un pin de entrada
  Fecha: 20180523
  Material: placa ESP32 OLED

  Descripción:
  Aprovechando que esta placa tiene un botón programable incluido en placa
  asociado al pin 0 y un led en el pin 2, haremos un programa que copie
  el estado del botón en el led.
  Utilizaremos las resistencias internas de pullup.
  Esto significa que el pin del botón tendrá HIGH como estado por defecto.
  Para que el led se encienda al pulsar el botón, aplicaremos al pin del
  led el estado inverso al del botón.

  Resultado:
  El led se enciende al pulsar el botón
*/

#include <Arduino.h>

const byte pinLed=2; //Específico para esta placa
const byte pinSwitch=0;

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
  //Configuramos los pines de los interruptores como entradas y con las
  //resistencias de pullup activas
  pinMode(pinSwitch, INPUT_PULLUP);
}

void loop()
{
  //Copiamos el estado opuesto contínuamente
  digitalWrite(pinLed,!digitalRead(pinSwitch));
}
