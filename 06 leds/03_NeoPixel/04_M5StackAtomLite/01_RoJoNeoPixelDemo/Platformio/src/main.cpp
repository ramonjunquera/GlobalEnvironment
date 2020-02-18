/*
  Autor: Ramón Junquera
  Fecha: 20200216
  Tema: M5Stack Atom Lite led
  Objetivo: Demo de gestión de led
  Material: M5Stack Atom Lite

  Descripción:
    Ejemplo de uso de librería RoJoNeoPixel genérica
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

void setup() {
  leds.begin(1,1,27); //Inicialización de led NeoPixel en M5Stack Atom Lite en pin 27
}

//Colores planos
void test1() {
  leds.v->clear({255,0,0}); //Rojo
  leds.draw();
  delay(1000);
  leds.v->clear({0,255,0}); //Verde
  leds.draw();
  delay(1000);
  leds.v->clear({0,0,255}); //Azul
  leds.draw();
  delay(1000);
  leds.v->clear({255,255,0}); //Amarillo
  leds.draw();
  delay(1000);
  leds.v->clear({255,0,255}); //Magenta
  leds.draw();
  delay(1000);
  leds.v->clear({0,255,255}); //Cyan
  leds.draw();
  delay(1000);
  leds.v->clear({255,255,255}); //Blanco
  leds.draw();
  delay(1000);
  leds.v->clear(); //Negro
  leds.draw();
  delay(1000);
}

void loop() {
  test1(); //Colores planos
}