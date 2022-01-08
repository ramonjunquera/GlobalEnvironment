/*
  Autor: Ramón Junquera
  Fecha: 20210825
  Tema: M5Stack Atom Matrix leds
  Objetivo: Demo de gestión de leds
  Material: M5Stack

  Descripción:
    Sucesión de varios ejemplos
*/

#include <Arduino.h>
#include <RoJoNeoPixel2AtomMatrix.h> //Gestión de leds NeoPixel de M5Stack Atom Matrix

RoJoNeoPixel2AtomMatrix leds; //Objeto de gestión de leds NeoPixel

void setup() {
  Serial.begin(115200);
  leds.begin(); //Inicialización de leds NeoPixel en M5Stack Atom Matrix
}

//Pixel
void test0() {
  for(byte y=0;y<5;y++) {
    for(byte x=0;x<5;x++) {
      leds.drawPixel(x,y,99); //Azul
      leds.show();
      delay(100);
      leds.drawPixel(x,y,0); //Negro
    }
  }
}

//Colores planos
void test1() {
  leds.clear(0xFF0000); //Rojo
  leds.show();
  delay(1000);
  leds.clear(0x00FF00); //Verde
  leds.show();
  delay(1000);
  leds.clear(0x0000FF); //Azul
  leds.show();
  delay(1000);
  leds.clear(0xFFFF00); //Amarillo
  leds.show();
  delay(1000);
  leds.clear(0xFF00FF); //Magenta
  leds.show();
  delay(1000);
  leds.clear(0x00FFFF); //Cyan
  leds.show();
  delay(1000);
  leds.clear(0xFFFFFF); //Blanco
  leds.show();
  delay(1000);
  leds.clear(); //Negro
  leds.show();
  delay(1000);
}

//Texto scroll
void test2() {
  RoJoSprite2 spr(3); //Color real
  spr.print("/5x5f.fon","HOLA MUNDO!",spr.getColor(0,255,0));
  for(int16_t x=5;x>-spr.xMax();x--) {
    leds.clear();
    leds.drawSprite(&spr,x);
    leds.show();
    delay(100);
  }
  spr.end();
}

//Dimming
void test3() {
  RoJoSprite2 blankSpr(3); //Sprite negro
  blankSpr.setSize(5,8); //Mismo tamaño que una letra
  uint32_t colors[7]={ //Tabla de colores
    blankSpr.getColor(255,0,0), //Rojo
    blankSpr.getColor(0,255,0), //Verde
    blankSpr.getColor(0,0,255), //Azul
    blankSpr.getColor(255,255,0), //Amarillo
    blankSpr.getColor(255,0,255), //Magenta
    blankSpr.getColor(0,255,255), //Cyan
    blankSpr.getColor(255,255,255) //Blanco
  };
  RoJoSprite2 number(3); //Sprite con el número
  RoJoSprite2 res(3); //Sprite con el resultado

  for(byte i=1;i<=7;i++) { //Contamos del 1 al 7...
    number.print("/5x5f.fon",String((int)i),colors[i-1]); //Dibujamos el número en el sprite
    for(byte level=0;level<255;level++) { //Recorremos todos los niveles...
      res.fade(&blankSpr,&number,level); //Creamos el sprite resultado para este nivel
      leds.drawSprite(&res);
      leds.show(); //Lo mostramos
      delay(4);
    }
    //Como antes pero ahora pasamos de número a negro
    for(byte level=255;level>0;level--) {
      res.fade(&blankSpr,&number,level);
      leds.drawSprite(&res);
      leds.show(); //Lo mostramos
      delay(4);
    }
  }
  res.end();
  number.end();
  blankSpr.end();
}

//Fading
void test4() {
  RoJoSprite2 spr1(3),spr2(3),res(3); //Definimos los sprites para hacer el fade
  uint32_t colors[7]={ //Tabla de colores
    spr1.getColor(255,0,0), //Rojo
    spr1.getColor(0,255,0), //Verde
    spr1.getColor(0,0,255), //Azul
    spr1.getColor(255,255,0), //Amarillo
    spr1.getColor(255,0,255), //Magenta
    spr1.getColor(0,255,255), //Cyan
    spr1.getColor(255,255,255) //Blanco
  };
  spr2.print("/5x5f.fon","1",colors[0]); //Creamos el primer número en el sprite 2
  leds.drawSprite(&spr2); //Lo pasamos a la memoria de vídeo
  leds.show(); //Lo mostramos
  delay(500); //Esperamos 
  for(byte i=2;i<=7;i++) { //Recorremos los números del 2 al 7
    spr1.copy(&spr2); //Copiamos el sprite 2 en el 1
    spr2.print("/5x5f.fon",String((int)i),colors[i-1]); //Creamos el número en el sprite 2
    for(byte level=0;level<255;level++) { //hacemos el fade del sprite 1 al 2
      res.fade(&spr1,&spr2,level); //Creamos el sprite con el level correcto
      leds.drawSprite(&res); //Dibujamos el resultado
      leds.show(); //Lo mostramos
      delay(4);
    }
    delay(500); //Tras el fade esperamos un momento
  }
  res.end();
  spr1.end();
  spr2.end();
}

//Kitt circular
void test5() {
  byte x[16]={0,1,2,3,4,4,4,4,4,3,2,1,0,0,0,0}; //Coordenadas X de cada punto
  byte y[16]={0,0,0,0,0,1,2,3,4,4,4,4,4,3,2,1}; //Coordenadas Y de cada punto
  byte levels[16]; //Niveles de intensidad de cada punto. Sólo canal rojo
  leds.clear(); //Borramos imágen
  for(byte cycles=0;cycles<4;cycles++) { //Número de vueltas...
    for(byte p=0;p<16;p++) { //Recorremos todas las posiciones del círculo...
      levels[p]=255; //Posición actual a brillo máximo
      for(byte steps=0;steps<20;steps++) { //Número de pasos por posición...
        //Dibujamos la situación actual
        for(byte i=0;i<16;i++) { //Recorremos todas la posiciones del círculo...
          leds.drawPixel(x[i],y[i],((uint32_t)levels[i])<<16);
        }
        leds.show();
        //Dimming
        for(byte i=0;i<16;i++) {
          if(levels[i]>0) levels[i]--;
        }
        delay(4);
      }
    }
  }
}

//Carga y dibujo de archivos bmp
void test6() {
  RoJoSprite2 spr(3);
  spr.loadBMP("/heart.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.loadBMP("/ball.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.loadBMP("/smiley.bmp");
  leds.drawSprite(&spr);
  leds.show();
  delay(2000);
  spr.end();
}


void loop() {
  test0(); //Pixel
  test1(); //Colores planos
  test2(); //Texto scroll
  test3(); //Dimming
  test4(); //Fading
  test5(); //Kitt
  test6(); //bmp
}
