/*
  Autor: Ramón Junquera
  Fecha: 20220517
  Tema: M5Stack Atom Lite led
  Objetivo: Demo de gestión de led
  Material: M5Stack Atom Lite o M5Stack Atom Echo

  Descripción:
    Ejemplo de uso de librería RoJoNeoPixel genérica
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

void setup() {
  leds.begin(1,27); //Inicialización de led NeoPixel en M5Stack Atom Lite en pin 27
}

//Colores planos
void test1() {
  leds.videoMem[0].setColor(0xFF0000); //Rojo
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0x00FF00); //Verde
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0x0000FF); //Azul
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0xFFFF00); //Amarillo
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0xFF00FF); //Magenta
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0x00FFFF); //Cyan
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0xFFFFFF); //Blanco
  leds.show();
  delay(1000);
  leds.videoMem[0].setColor(0); //Negro
  leds.show();
  delay(1000);
}

void loop() {
  test1(); //Colores planos
}