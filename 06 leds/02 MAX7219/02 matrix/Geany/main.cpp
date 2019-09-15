/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219
  Fecha: 20190913
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219
  Material: breadboard, cables, 4 soportes para chip MAX7219, 4 matrix led 8x8

  Descripción:
    Demostración de funcionalidades de la librería.
  
  Resultado:
    Realizamos varios tests cíclicos
*/
#include <Arduino.h>
#include <RoJoMAX7219.h> //Librería de gestión de MAX7219
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDIN=D0;
  const byte pinCS=D1;
  const byte pinCLK=D2;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDIN=22;
  const byte pinCS=21;
  const byte pinCLK=4;
#elif defined(ARDUINO_ARCH_AVR) //Si es un Arduino...
  const byte pinDIN=4;
  const byte pinCS=3;
  const byte pinCLK=2;
#elif defined(__arm__) //Si es una RPi...
  const byte pinDIN=16;
  const byte pinCS=20;
  const byte pinCLK=21;
#endif

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;

//Función drawPixel con color 1 = dibujar
void Test1() {
  //Limpiamos pantalla
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=2)
    for(byte x=0;x<display.xMax();x+=3) {
      display.drawPixel(x,y,1);
      delay(50);
    }
  delay(1000);
}

//Sprites definidos en programa
//  RoJoMAX7219::drawSpritePage
//  RoJoSprite::drawPage
void Test2() {
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite monocromo
  RoJoSprite mySprite(1);
  //Lo dimensionamos. Anchura=7. Páginas=1
  mySprite.setSize(7,1);
  //Lo dibujamos
  //void drawPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.drawPage(0,0,0b00111110,4); //4=escribir el valor tal cual
  mySprite.drawPage(1,0,0b01000001,4);
  mySprite.drawPage(2,0,0b01010101,4);
  mySprite.drawPage(3,0,0b01010001,4);
  mySprite.drawPage(4,0,0b01010101,4);
  mySprite.drawPage(5,0,0b01000001,4);
  mySprite.drawPage(6,0,0b00111110,4);

  //Desplazamiento horizontal del sprite
  //Recorremos todas las columnas de pantalla
  for(int16_t x=-(int16_t)mySprite.xMax();x<(int16_t)display.xMax()+1;x++) {
    //Limpiamos memoria de vídeo
    display.v->clear();
    //Dibujamos el sprite sobre la memoria de vídeo
    display.drawSprite(&mySprite,x,0);
    //Esperamos un momento
    delay(80);
  }

  //Desactivamos temporalmente el refresco automático
  //Primero compondremos la imágen y después la mostraremos
  display.autoDraw=false;
  //Desplazamiento vertical de varios sprites
  for(int8_t y=-8;y<13;y++) {
    //Limpiamos memoria de vídeo
    display.v->clear();
    //Dibujamos 3 sprites
    for(byte s=0;s<3;s++) {
      //Dibujamos sprite deplazados entre sí verticalmente
      display.v->drawSprite(&mySprite,s*10,y-2*s);
    }
    //Mostramos la memoria de vídeo
    display.draw();
    //Esperamos un momento
    delay(90);
  }
  
  //Borramos el sprite utilizado
  mySprite.end();
  //Recativamos el refresco automático
  display.autoDraw=true;
}

//Lectura de sprites de desde archivos
void Test3() {
  //Creamos un nuevo sprite monocromo
  RoJoSprite ball(1);
  //Leemos su contenido desde un archivo
  ball.loadSprite(F("/ball.spr")); //Su tamaño es x=30,y=30,pages=4
  //Offset vertical
  int8_t y=-30;
  //Delta de desplazamiento
  int8_t dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+8000; //8 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    display.v->clear();
    //Dibujamos el sprite
    display.drawSprite(&ball,1,y);
    //Calculamos la nueva coordenada vertical
    y+=dy;
    //Si hemos llegado a un final, cambiamos la dirección
    if(y==-30 || y==8) dy=-dy;
    //Esperamos un momento
    delay(40);
  }
  //Borramos el sprite utilizado
  ball.end();
}

//Sprites de texto
void Test4() {
  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite textSprite(1);
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!textSprite.print(("/5x7.fon"),F("12345"),1)) return;
  //Lo mostramos
  display.drawSprite(&textSprite);
  //Esperamos un momento
  delay(1000);

  //Reaprovechamos el sprite de texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!textSprite.print(F("/5x7.fon"),F("Hola!"),1)) return;
  //Desplazamiento horizontal del sprite
  //Recorremos todas las columnas de pantalla
  for(int16_t x=-(int16_t)textSprite.xMax();x<(int16_t)display.xMax()+1;x++) {
    //Limpiamos la memoria de vídeo
    display.v->clear();
    //Dibujamos el sprite
    display.drawSprite(&textSprite,x,0);
    //Esperamos un momento
    delay(80);
  }
  //Borramos el sprite utilizado
  textSprite.end();
}

//Líneas en sprites
void Test5() {
  //Desactivamos temporalmente el refresco automático
  display.autoDraw=false;
  for(byte y1=0;y1<8;y1++)
    for(byte y2=0;y2<8;y2++) {
      //Limpiamos pantalla
      display.clear();
      //Dibujamos la línea
      display.line(0,y1,display.xMax(),y2,1);
      //Refrescamos pantalla
      display.draw(); 
      //Esperamos un momento
      delay(70);
    }
  //Recativamos el refresco automático
  display.autoDraw=true;
}

void setup()
{
  //Serial.begin(115200); delay(4000); //DEBUG
  //Inicializamos el display
  //RoJoMAX7219(byte chainedChips,byte pinDIN,byte pinCS,byte pinCLK);
  display.begin(4,pinDIN,pinCS,pinCLK);
}

void loop()
{
  Test1(); //Función drawPixel
  Test2(); //Sprites definidos en programa
  Test3(); //Lectura de sprites de desde archivos
  Test4(); //Sprites de texto
  Test5(); //Líneas en sprites
}
