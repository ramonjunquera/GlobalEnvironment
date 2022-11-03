/*
  Autor: Ramón Junquera
  Fecha: 20221103
  Tema: Anillos de leds NeoPixel

  Descripción:
    Sucesión de varios ejemplos con anillo de 8 leds
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <RoJoNeoPixelRing.h>

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
RoJoNeoPixelRing ring;

//Sucesión ascendente
void test1() {
  //Un anillo de 8 leds tiene un ángulo entre leds de 360/8=45 grados
  for(uint16_t angle=0;angle<360*4;angle+=45) { //Daremos 4 vueltas
    ring.clear(); //Borramos
    ring.set({100,0,0},angle); //Encendemos el led correspondiente al ángulo en verde
    leds.show(); //Mostramos resultado
    delay(100);
  }
}

//Efecto KITT
void test2() {
  ring.clear(); //Borramos
  //Un anillo de 8 leds tiene un ángulo entre leds de 360/8=45 grados
  for(uint16_t angle=0;angle<360*5;angle+=45) { //Daremos 5 vueltas
    for(byte i=0;i<8;i++) leds.videoMem[i].channel[1]/=2; //Reducimos brillo de todos los leds a la mitad
    ring.set({0,255,0},angle); //Encendemos el led correspondiente al ángulo en rojo
    leds.show();
    delay(100);
  }
}

//Giramos un arco
void test3() {
  //Un anillo de 8 leds tiene un ángulo entre leds de 360/8=45 grados
  for(uint16_t angle=0;angle<360*4;angle+=45) { //Daremos 4 vueltas
    ring.clear(); //Borramos
    ring.set({0,0,99},angle,angle+90); //Dibujamos en azul un arco de 90 grados
    leds.show(); //Mostramos resultado
    delay(100);
  }
}

//Giramos un arco de dos colores
void test4() {
  //Un anillo de 8 leds tiene un ángulo entre leds de 360/8=45 grados
  for(uint16_t angle=0;angle<360*4;angle+=45) { //Daremos 4 vueltas
    ring.set({50,50,0},angle,angle+180); //Dibujamos en amarillo un arco de 180 grados
    ring.set({0,0,80},angle+180,angle); //Dibujamos en azul un arco de 180 grados
    leds.show(); //Mostramos resultado
    delay(100);
  }
}

void setup() {
  leds.begin(8,13); //8 leds en pin 13
  ring.begin(&leds,0,8); //El anillo comienza en posición 0 con 8 leds
}

void loop() {
  test1(); //Sucesión ascendente
  test2(); //KITT
  test3(); //Arco
  test4(); //Arco de dos colores
}