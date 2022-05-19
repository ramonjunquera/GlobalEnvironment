/*
  Autor: Ramón Junquera
  Fecha: 20220518
  Tema: Gestión de led NeoPixel para modelos M5Stack con un sólo led
  Objetivo: Demo de gestión de led
  Material: M5Stack Atom Lite, M5Stack Atom Echo o M5Stack Stamp C3

  Descripción:
    Ejemplo de uso de librería RoJoNeoPixel genérica
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

#ifdef ARDUINO_M5StampC3 //Para M5Stack Stamp C3
  const byte pinLed=2;
#elif defined(ESP32) //Para M5Stack Atom Lite & Atom Echo
  const byte pinLed=27;
#endif

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

void setup() {
  leds.begin(1,pinLed); //Inicialización de led NeoPixel
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