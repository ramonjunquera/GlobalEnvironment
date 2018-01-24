/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219
  Fecha: 20180124
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219
  Material: breadboard, cables, chip MAX7219, matrix led 8x8, placa ESP8266,
    resistencia de 10Kohmios, condensador 10µF, condensador 100 nF

  Descripción:
  Descripción y demostración de funcionalidades de la librería.
  Cuando utilizamos una matriz sin soporte, la estamos girando 90 grados.
  Lo que antes eran columnas, ahora son filas.
  Tendremos esto en cuenta en todo el ejemplo

  Pinout de MAX7219:
           
   DOUT C5 C1  C6  C4 V+ ISET C8  C3 C7  C2  CLK
    │  │  │  │  │  │  │  │  │  │  │  │
  ███████████████████████████
  ███████████████████████████
    ██████████ MAX7219 ███████████
  ███████████████████████████
  ███████████████████████████
    │  │  │  │  │  │  │  │  │  │  │  │
   DIN  F1 F5 GND  F7 F3  F4  F8 GND F6  F2 LOAD      
        
  DIN = Primero de los tres pines conectados a la placa
  LOAD = Segundo de los tres pines conectados a la placa
  CLK = Tercero de los tres pines conectados a la placa
  DOUT = Salida a conectar con la pata DIN del siguiente chip de la cadena
  GND = Tierra
  V+ = Alimentación. Funciona a 3.3V. Sugeridos 5V
  ISET = Siempre estará conectado a V+ a través de una resistencia
  Fx = Filas
  Cx = Columnas

  Pinout de led matrix 8x8
 
   C8 C7 F2 C1 F4 C6 C4 F1
    ¦  ¦  ¦  ¦  ¦  ¦  ¦  ¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
    ¦  ¦  ¦  ¦  ¦  ¦  ¦  ¦
   F5 F7 C2 C3 F8 C5 F6 F3

  Nota:
    Los condensadores son opcionales, aunque convenientes para filtrar
    interferencias.
  
  Resultado:
  Realizamos varios tests cíclicos
*/
#include <Arduino.h>
#include "RoJoMAX7219.h" //Librería de gestión de MAX7219
#include "RoJoABC.h" //Gestión de fuentes

//Definimos los pines del display
const byte pinDIN=D0;
const byte pinCS=D1;
const byte pinCLK=D2;

//Declaración de variables globales
//Creamos el objeto display que gestionará el chip MAX7219
//RoJoMatrix(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
RoJoMAX7219 display(1,pinDIN,pinCS,pinCLK);

void test1()
{
  //Un pixel en movimiento rebota contras los bordes

  //Coordenadas iniciales
  uint8_t x=1;
  uint8_t y=5;
  //Dirección (delta)
  uint8_t dx=1;
  uint8_t dy=1;
  //Calculamos la duración del ejemplo
  uint32_t maxTime=millis()+5000; //5 segundos
  
  while(millis()<maxTime)
  {
    //Limpiamos pantalla
    display.clear();
    //Dibujamos el pixel
    display.setPixel(x,y,1);
    //Lo mostramos
    display.show();
    //Calculamos las nuevas coordenadas
    x+=dx;
    y+=dy;
    //Si hemos tocado algún borde...rebotamos
    if(x==0 || x==7) dx=-dx;
    if(y==0 || y==7) dy=-dy;
    //Esperamos un momento
    delay(100);
  }
}

void test2()
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
  mySprite.setPage(3,0,0b01000001,4);
  mySprite.setPage(4,0,0b01011101,4);
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
    //Dibujamos sprite
    display.drawSprite(0,y,&mySprite,1);
    //Lo mostramos
    display.show();
    //Esperamos un momento
    delay(90);
  }
  
  //Borramos el sprite utilizado
  mySprite.clean();
}

void test3()
{
  //Sprites de texto
  
  //Limpiamos pantalla
  display.clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC font;
  //Creamos el sprite con el texto
  //Si no podemos crear el sprite de texto...hemos terminado  
  if(!font.print(F("/RoJoABC5x7digits.fon"),F("2"),&textSprite)) return;
  //Lo mostramos
  display.drawSpritePage(0,0,&textSprite,4);
  //Refrescamos pantalla
  display.show();
  
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
  test2();
  test3();
  delay(1000);
}
