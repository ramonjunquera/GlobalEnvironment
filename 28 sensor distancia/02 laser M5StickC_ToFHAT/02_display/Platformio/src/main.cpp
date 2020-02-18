/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: M5Stick C. ToF HAT (sensor de distancia láser)
  Objetivo: Mostrar en el display la distancia medida
  Material: M5Strick C, M5Stick ToF HAT
*/

#include <Arduino.h>
#include <RoJoST7735S.h> //Destión de display
#include <RoJoM5ToFHAT.h> //Gestión de sensor ToF

//Definición de objetos globales
RoJoM5ToFHAT tof;
RoJoST7735S display;
RoJoSprite spr;

void setup() {
  display.begin(); //Inicialización por defecto de display para M5StickC
  tof.begin(); //Inicializamos el sensor de distancia
  spr.setSize(64,23); //Dimensionamos el sprite que contendrá la distancia
}

void loop() {
  uint16_t d;
  while((d=tof.get())==0); //Intentaremos conseguir una medida válida
  spr.clear(); //Borramos sprite
  spr.printOver("/15x23d.fon",String(d),{255,255,255});
  display.drawSprite(&spr,8,10);
  delay(100);
}
