/*
  Autor: Ramón Junquera
  Fecha: 20220517
  Tema: M5Stack Fire. Leds NeoPixel
  Objetivo: Demo de gestión de leds
  Material: M5Stack Fire

  Descripción:
    Sucesión de varios ejemplos
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

//Sucesión ascendente
void test1() {
  for(byte i=0;i<10;i++) leds.videoMem[i].setColor(0); //Apagamos todos los leds
  for(byte turnCount=0;turnCount<3;turnCount++) { //Damos 3 vueltas
    for(byte i=0;i<10;i++) { //Recorremos todos los leds
      leds.videoMem[i].setColor(0x00FF00); //Verde
      leds.show();
      leds.videoMem[i].setColor(0); //Negro
      delay(100); //Esperamos un momento
    }
  }
}

//Efecto KITT por ambos lados
void test2() {
  int8_t x=0; //Posición del led procesado
  int8_t delta=1; //Dirección

  for(byte i=0;i<10;i++) leds.videoMem[i].setColor(0); //Apagamos todos los leds
  for(byte epoch=100;epoch>0;epoch--) { //Hacemos 100 cambios de led
    //Reducimos a la mitad el brillo del canal rojo de los 5 primeros leds (un lado)
    //Recordemos que los canales de NeoPixel son GRB y no RGB. POr eso el canal rojo es el 1
    for(byte i=0;i<5;i++) leds.videoMem[i].channel[1]/=2;
    leds.videoMem[x].channel[1]=0xFF; //Leds actual a máximo brillo en canal rojo
    //Copiamos configuración de un lado al otro
    for(byte i=0;i<5;i++) leds.videoMem[i+5].channel[1]=leds.videoMem[i].channel[1];
    leds.show();
    x+=delta; //Calculamos la siguiente posición
    if(x==0 || x==4) delta=-delta; //Si hemos llegado a un extremo...cambiamos de dirección
    delay(100); //Esperamos un momento
  }
}

void setup() {
  //Configuración NeoPixel para M5Stack Fire. 5x2=10 leds en pin 15
  leds.begin(10,15);
}

void loop() {
  test1(); //Sucesión ascendente
  test2(); //KITT
}