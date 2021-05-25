/*
  Autor: Ramón Junquera
  Fecha: 20210524
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
void Test1() {
  leds.v->clear(); //Apagamos todos los leds
  for(byte turnCount=0;turnCount<3;turnCount++) { //Damos 3 vueltas
    for(byte y=0;y<2;y++) { //Recorremos todos los leds
      for(byte x=0;x<5;x++) { //Recorremos todos los leds
        leds.v->drawPixel(x,y,leds.v->getColor(0,255,0)); //Encendemos el led procesado. Sólo canal verde
        leds.draw(); //Mostramos la configuración actual
        leds.v->drawPixel(x,y,0); //Apagamos el led procesado para que no aparezca en el siguiente ciclo
        delay(100); //Esperamos un momento
      }
    }
  }
}

//Efecto KITT por ambos lados
void Test2() {
  int8_t x=0; //Posición del led procesado
  int8_t delta=1; //Dirección
  uint32_t color;

  leds.v->clear(); //Apagamos todos los leds
  for(byte i=0;i<100;i++) { //Hacemos 100 cambios de led
    for(byte y=0;y<2;y++) { //Recorremos ambos lados
      leds.v->drawPixel(x,y,leds.v->getColor(255,0,0)); //Encendemos el led en rojo
    }
    leds.draw(); //Mostramos la configuración actual
    x+=delta; //Calculamos la siguiente posición
    if(x==0 || x==4) delta=-delta; //Si hemos llegado a un extremo...cambiamos de dirección
    //Atenuamos el canal rojo de todos los pixels
    for(byte dimY=0;dimY<2;dimY++) {
      for(byte dimX=0;dimX<5;dimX++) {
        color=leds.v->getPixel(dimX,dimY); //Obtenemos el color
        byte r,g,b; //Canales de color
        leds.v->getColor(color,&r,&g,&b); //Descomponemos en canales
        color=leds.v->getColor(r/2,0,0); //Reducimos a la mitad el canal rojo
        leds.v->drawPixel(dimX,dimY,color); //Dibujamos de nuevo el pixel
      }
    }
    delay(100); //Esperamos un momento
  }
}

void setup() {
  //Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
}

void loop() {
  Test1(); //Sucesión ascendente
  Test2(); //KITT
}