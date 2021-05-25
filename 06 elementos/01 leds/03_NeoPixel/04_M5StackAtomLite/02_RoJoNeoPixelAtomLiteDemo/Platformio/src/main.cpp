/*
  Autor: Ramón Junquera
  Fecha: 20201204
  Tema: M5Stack Atom Lite led
  Objetivo: Demo de gestión de led
  Material: M5Stack Atom Lite o M5Stack Atom Echo

  Descripción:
    Ejemplo de uso de librería RoJoNeoPixel genérica
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en M5Stack Atom Lite
#include <RoJoSwitch.h> //Gestión de interruptor

RoJoNeoPixelAtomLite led; //Objeto de gestión de leds NeoPixel
RoJoSwitch button(39); //Objeto de gestión de pulsador. Pin 39 en Atom

void setup() {
  led.begin(); //Inicialización de led NeoPixel en M5Stack Atom Lite o Echo
}

void loop() {
  led.draw(255,0,0); //Rojo
  while(!button.pressed()); //Esperar pulsación
  led.draw(0,255,0); //Verde
  while(!button.pressed()); //Esperar pulsación
  led.draw(0,0,255); //Azul
  while(!button.pressed()); //Esperar pulsación
  led.draw(255,255,0); //Amarillo
  while(!button.pressed()); //Esperar pulsación
  led.draw(255,0,255); //Magenta
  while(!button.pressed()); //Esperar pulsación
  led.draw(0,255,255); //Cyan
  while(!button.pressed()); //Esperar pulsación
  led.draw(255,255,255); //Blanco
  while(!button.pressed()); //Esperar pulsación
  led.draw(0,0,0); //Negro
  while(!button.pressed()); //Esperar pulsación
}