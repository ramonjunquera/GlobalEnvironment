/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219
  Fecha: 20171113
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

//Definimos los pines del display
const byte pinDIN=D0;
const byte pinCS=D1;
const byte pinCLK=D2;

//Declaración de variables globales
//Creamos el objeto display que gestionará la cadena de chips MAX7219
//En la creación ya se incluye la activación y la inicialización
//tras ello estará lista para ser utilizada
//RoJoMatrix(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
RoJoMAX7219 display(4,pinDIN,pinCS,pinCLK);

void test1()
{
  //Función setPixel con color 1 = dibujar

  //Limpiamos pantalla
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte x=0;x<11;x++)
    for(byte y=0;y<4;y++)
      display.setPixel(x*3,y*2,1);
  //Mostramos el resultado
  display.show();
}

void test2()
{
  //Función setPixel con color 2 = invertir
  
  //Dibujamos un rectángulo relleno sobre la matriz anterior que invierte los pixels
  for(byte x=0;x<display.width();x++)
    for(byte y=0;y<8;y++)
      display.setPixel(x,y,2);
  //Mostramos el resultado
  display.show();
}

void test3()
{
  //Función setPixel con color 0 = borrar
  
  //Dibujamos un rectángulo relleno en el interior que borra
  for(byte x=2;x<display.width()-2;x++)
    for(byte y=2;y<6;y++)
      display.setPixel(x,y,0);
  //Mostramos el resultado
  display.show();
}

void test4()
{
  //Función getPixel
  
  //Del rectángulo que contiene todo, invertimos los puntos
  for(byte x=0;x<display.width();x++)
    for(byte y=0;y<8;y++)
      display.setPixel(x,y,!display.getPixel(x,y));
  //Mostramos el resultado
  display.show();
}

void test5()
{
  //Sprites definidos en programa.
  //Función de display: drawSpritePage
  //Función de sprite: setPage
  
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite
  RoJoSprite mySprite;
  //Lo dimensionamos. Anchura=7. Páginas=1
  mySprite.setSize(7,1);
  //Lo dibujamos
  //void setPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.setPage(0,0,0b00111110,4); //4=escribir el valor tal cual
  mySprite.setPage(1,0,0b01000001,4);
  mySprite.setPage(2,0,0b01010101,4);
  mySprite.setPage(3,0,0b01010001,4);
  mySprite.setPage(4,0,0b01010101,4);
  mySprite.setPage(5,0,0b01000001,4);
  mySprite.setPage(6,0,0b00111110,4);

  //Desplazamiento horizontal del sprite
  //Recorremos todas las columnas de pantalla
  for(int x=-mySprite.width();x<display.width()+1;x++)
  {
    //Limpiamos pantalla
    display.clear();
    //Dibujamos el sprite
    display.drawSpritePage(x,0,&mySprite,1);
    //Lo mostramos
    display.show();
    //Esperamos un momento
    delay(80);
  }

  //Desplazamiento vertical de varios sprites
  for(int8_t y=-8;y<13;y++)
  {
    //Limpiamos pantalla
    display.clear();
    //Dibujamos 3 sprites
    for(byte s=0;s<3;s++)
    {
      //Dibujamos sprite
      display.drawSprite(s*10,y-2*s,&mySprite,1);
    }
    //Los mostramos
    display.show();
    //Esperamos un momento
    delay(90);
  }
  
  //Borramos el sprite utilizado
  mySprite.clean();
}

void test6()
{
  //Lectura de sprites de desde archivos
  
  //Creamoss un nuevo sprite
  RoJoSprite ball;
  //Leemos su contenido desde un archivo
  ball.load(F("/ball30x30.spr")); //Su tamaño es x=30,y=30,pages=4
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
    display.clear();
    //Dibujamos el sprite
    display.drawSprite(1,y,&ball,1);
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

void test7()
{
  //Sprites de texto
  
  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC font;
  //Si no podemos cargar la fuente desde el archivo...hemos terminado
  if(!font.load(F("/RoJoABC5x7digits.fon"))) return;
  //Creamos el sprite con el texto
  font.print(F("12345"),&textSprite);
  //Lo mostramos
  display.drawSpritePage(0,0,&textSprite,4);
  //Refrescamos pantalla
  display.show();
  
  //Terminamos de utilizar el objeto de gestión de fuentes
  font.close();
  //Borramos el sprite utilizado
  textSprite.clean();
}


void setup()
{
  //Nada especial que inicializar
}

void loop()
{
  test1();
  delay(1000);
  test2();
  delay(1000);
  test3();
  delay(1000);
  test4();
  delay(1000);
  test5();
  test6();
  test7();
  delay(2000);
}
