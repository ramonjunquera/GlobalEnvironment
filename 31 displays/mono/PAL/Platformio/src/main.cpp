/*
  Autor: Ramón Junquera
  Fecha: 20210211
  Tema: Librería para vídeo compuesto PAL
  Objetivo: Demo de librería RoJoPAL
  Material: M5Stack Atom Lite

  Descripción:
    Diferentes ejemplos de uso de la librería.
*/

#include <Arduino.h>
#include <RoJoSprite2.h> //Gestión de sprites
#include <RoJoPAL.h> //Gestión de vídeo compuesto PAL

RoJoSprite2 screen(1); //Sprite gris para pantalla

//Carga un sprite del máximo tamaño permitido
void test01() {
  PAL_end(); //Nos aseguramos que el refresco de pantalla esté desactivado
  screen.loadSprite("/mountain.spr");
  PAL_begin(&screen);
  delay(4000);
}

//copy
void test02() {
  RoJoSprite2 spr(1); //Sprite gris
  spr.copy(&screen,200,0,250,80);
  screen.drawSprite(&spr,200,100);
  spr.end();
  delay(3000);
}

//clear & block
void test03() {
  byte tone;
  screen.clear();
  for(byte i=0;i<=10;i++) {
    tone=i*25;
    screen.block(i*50,i*15,140,50,screen.getColor(tone,tone,tone));
  }
  delay(4000);
}

//drawSprite
void test04() {
  screen.clear();

  //Dibujamos las 3 primeras líneas leyendo directamente desde archivo
  for(byte y=0;y<3;y++) {
    for(byte x=0;x<21;x++) {
      screen.drawSprite("/heart.spr",x*32,y*32);
    }
  }

  RoJoSprite2 spr(1); //Sprite gris
  spr.loadSprite("/heart.spr");
  //Dibujamos las 3 últimas líneas escribiendo desde sprite en memoria
  for(byte y=3;y<6;y++) {
    for(byte x=0;x<21;x++) {
      screen.drawSprite(&spr,x*32,y*32);
    }
  }
  spr.end();
  delay(4000);
}

//drawPixel
void test05() {
  byte tone;
  screen.clear();
  for(uint16_t y=0;y<screen.yMax();y+=2) {
    for(uint16_t x=0;x<screen.xMax();x+=2) {
      tone=x*y;
      screen.drawPixel(x,y,screen.getColor(tone,tone,tone));
    }
  }
  delay(6000);
}

//line
void test06() {
  byte tone;
  screen.clear();
  for(byte i=0;i<=20;i++) {
    tone=i*10;
    screen.line(0,screen.yMax()-i*10,i*32,0,screen.getColor(tone,tone,tone));
  }
  delay(3000);
}

//replaceColor
void test07() {
  screen.replaceColor(0,screen.getColor(255,255,255)); //Cambia negro por blanco
  delay(3000);
}

//Nuevo sprite de pantalla y loadBMP
void test08() {
  PAL_end(); //Obligatorio desactivar el refresco de pantalla para cambiar el sprite
  //Se puede cargar un .bmp color sobre un sprite de grises
  screen.loadBMP("/gotas.bmp"); //Cargamos imágen de distinto tamaño
  PAL_begin(&screen); //La volvemos a mostrar
  delay(3000);
}

//drawSprite con color invisible
void test09() {
  RoJoSprite2 spr(1); //Sprite gris
  spr.loadSprite("/heart.spr");
  for(byte y=0;y<8;y++) {
    for(byte x=0;x<10;x++) {
      screen.drawSprite(&spr,spr.xMax()*x,spr.yMax()*y,spr.getColor(255,255,255)); //Blanco=invisible
    }
  }
  spr.end();
  delay(3000);
}

//print
void test10() {
  //Dibujamos texto directamente desde archivo de fuentes
  screen.printOver("/10x15.fon","Hola!",0,10,10);
  //Dibujamos texto desde sprite
  RoJoSprite2 spr(1); //Sprite gris

  //Texto negro, fondo blanco
  spr.print("/10x15.fon","Adios!",0,spr.getColor(255,255,255));
  //Dibujamos texto haciendo el fondo invisible
  screen.drawSprite(&spr,10,30,screen.getColor(255,255,255));

  //Texto negro, fondo gris, borde blanco
  spr.print("/10x15.fon","Buenas tardes",0,spr.getColor(100,100,100),spr.getColor(255,255,255));
  //Dibujamos texto haciendo el fondo invisible
  screen.drawSprite(&spr,10,50,screen.getColor(100,100,100));

  spr.end();
  delay(4000);
}

//resize
void test11() {
  RoJoSprite2 sprOriginal(1),sprResized(1); //Sprites grises
  sprOriginal.loadSprite("/heart.spr");
  for(byte i=32;i<200;i++) {
    sprResized.resize(&sprOriginal,i,i);
    screen.drawSprite(&sprResized);
  }
  sprResized.end();
  sprOriginal.end();
  delay(2000);
}

//rotate
void test12() {
  screen.clear(screen.getColor(100,100,100));
  RoJoSprite2 spr(1),sprRotate(1); //Sprites grises
  spr.print("/10x15.fon","Hola",spr.getColor(255,255,255));
  screen.drawSprite(&spr,20,10,0);
  sprRotate.rotate(&spr,90);
  screen.drawSprite(&sprRotate,60,30,0);
  sprRotate.rotate(&spr,270);
  screen.drawSprite(&sprRotate,0,30,0);
  sprRotate.rotate(&spr,180);
  screen.drawSprite(&sprRotate,20,70,0);
  sprRotate.end();
  spr.end();
  delay(4000);
}

//rect, block, circle, disk, triangle, triangleFill, ellipse, ellipseFill
void test13() {
  uint32_t colorWhite=screen.getColor(255,255,255);
  screen.clear();
  screen.rect(0,0,20,20,colorWhite);
  screen.block(30,0,20,20,colorWhite);
  screen.circle(10,40,10,colorWhite);
  screen.disk(40,40,10,colorWhite);
  screen.triangle(10,60,20,80,0,80,colorWhite);
  screen.triangleFill(40,60,50,80,30,80,colorWhite);
  screen.ellipse(10,105,10,15,colorWhite);
  screen.ellipseFill(40,105,10,15,colorWhite);
  delay(5000);
}

void setup() {
  //Serial.begin(115200);
  screen.begin();
}

void loop() {
  test01(); //Carga un sprite del máximo tamaño permitido
  test02(); //copy
  test03(); //clear & block
  test04(); //Mosaico
  test05(); //drawPixel
  test06(); //line
  test07(); //replaceColor
  test08(); //Nuevo sprite de pantalla
  test09(); //drawSprite con color invisible
  test10(); //print
  test11(); //resize
  test12(); //rotate
  test13(); //rect, block, circle, disk, triangle, triangleFill, ellipse, ellipseFill
}
