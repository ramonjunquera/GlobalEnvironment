/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219
  Fecha: 20180310
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219
  Material: breadboard, cables, 4 soportes para chip MAX7219, 4 matrix led 8x8, placa ESP8266

  Descripción:
  Descripción y demostración de funcionalidades de la librería.
  
  Resultado:
  Realizamos varios tests cíclicos
*/
#include <Arduino.h>
#include "RoJoMAX7219.h" //Librería de gestión de MAX7219
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos
#include "RoJoABC.h" //Gestión de fuentes

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDIN_display=D0;
  const byte pinCS_display=D1;
  const byte pinCLK_display=D2;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDIN_display=22;
  const byte pinCS_display=21;
  const byte pinCLK_display=4;

#endif

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;

void test1()
{
  //Función drawPixel con color 1 = dibujar

  //Limpiamos pantalla
  display.videoMem->clear();
  //Dibujamos una matriz de puntos
  for(byte x=0;x<11;x++)
    for(byte y=0;y<4;y++)
      display.videoMem->drawPixel(x*3,y*2,1);
  //Mostramos el resultado
  display.show();

  delay(1000);
}

void test2()
{
  //Sprites definidos en programa
  //Función de display: drawSpritePage
  //Función de sprite: drawPage
  
  //Limpiamos pantalla
  display.videoMem->clear();
  //Creamos el sprite
  RoJoSprite mySprite;
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
  for(int x=-mySprite.width();x<display.videoMem->width()+1;x++)
  {
    //Limpiamos pantalla
    display.videoMem->clear();
    //Dibujamos el sprite
    display.videoMem->drawSpritePage(x,0,&mySprite,1);
    //Lo mostramos
    display.show();
    //Esperamos un momento
    delay(80);
  }

  //Desplazamiento vertical de varios sprites
  for(int8_t y=-8;y<13;y++)
  {
    //Limpiamos pantalla
    display.videoMem->clear();
    //Dibujamos 3 sprites
    for(byte s=0;s<3;s++)
    {
      //Dibujamos sprite deplazados entre sí verticalmente
      display.videoMem->drawSprite(s*10,y-2*s,&mySprite,1);
    }
    //Los mostramos
    display.show();
    //Esperamos un momento
    delay(90);
  }
  
  //Borramos el sprite utilizado
  mySprite.clean();
}

void test3()
{
  //Lectura de sprites de desde archivos
  
  //Creamoss un nuevo sprite
  RoJoSprite ball;
  //Leemos su contenido desde un archivo
  ball.load(F("/ball.spr")); //Su tamaño es x=30,y=30,pages=4
  //Offset vertical
  int8_t y=-30;
  //Delta de desplazamiento
  int8_t dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+8000; //8 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Borramos la pantalla
    display.videoMem->clear();
    //Dibujamos el sprite
    display.videoMem->drawSprite(1,y,&ball,1);
    //Refrescamos pantalla
    display.show();
    //Calculamos la nueva coordenada vertical
    y+=dy;
    //Si hemos llegado a un final, cambiamos la dirección
    if(y==-30 || y==8) dy=-dy;
    //Esperamos un momento
    delay(40);
  }
  //Borramos el sprite utilizado
  ball.clean();
}

void test4()
{
  //Sprites de texto
  
  //Limpiamos pantalla
  display.videoMem->clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC font;
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!font.print(F("/5x7.fon"),F("12345"),&textSprite)) return;
  //Lo mostramos
  display.videoMem->drawSpritePage(0,0,&textSprite,4);
  //Refrescamos pantalla
  display.show();
  //Esperamos un momento
  delay(1000);

  //Reaprovechamos el sprite de texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!font.print(F("/5x7.fon"),F("Hola!"),&textSprite)) return;
  //Desplazamiento horizontal del sprite
  //Recorremos todas las columnas de pantalla
  for(int x=-textSprite.width();x<display.videoMem->width()+1;x++)
  {
    //Limpiamos pantalla
    display.videoMem->clear();
    //Dibujamos el sprite
    display.videoMem->drawSpritePage(x,0,&textSprite,1);
    //Lo mostramos
    display.show();
    //Esperamos un momento
    delay(80);
  }
  
  //Borramos el sprite utilizado
  textSprite.clean();
}

void test5()
{
  //Líneas en sprites

  for(byte y1=0;y1<8;y1++)
    for(byte y2=0;y2<8;y2++)
    {
      //Limpiamos pantalla
      display.videoMem->clear();
      //Dibujamos la línea
      display.videoMem->line(0,y1,display.videoMem->width(),y2,1);
      //Refrescamos pantalla
      display.show(); 
      //Esperamos un momento
      delay(70);
    }
}

void setup()
{
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(4,pinDIN_display,pinCS_display,pinCLK_display);
}

void loop()
{
  test1(); //Función drawPixel
  test2(); //Sprites definidos en programa
  test3(); //Lectura de sprites de desde archivos
  test4(); //Sprites de texto
  test5(); //Líneas en sprites
}
