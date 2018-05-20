/*
  Autor: Ramón Junquera
  Fecha: 20180520
  Tema: Touch Screen XPT2046 de ILI9486
  Objetivo: Demo de detección de pulsaciones en botones
  Material: placa ESP, display ILI9486 con touch screen XPT2016

  Descripción:
  Es necesario tener un archivo de configuración del touch screen guardado.
  
  Crearemos una demo para dibujar con colores en el display.
  En la parte inferior dibujaremos una botonera con 8 distintos colores.
  Cada botón tiene un tamaño de 40x40 pixels.
  Haciendo click en ellos cambiará el color del lápiz.
  No se podrá dibujar encima de los botones.

  Como posible mejora futura se podría dibujar un rectángulo blanco alrededor
  del color de la paleta que está seleccionado. Así el usuario sabría con
  qué color se dibujará.
  
  Pinout SPI:
                         display touchscreen
  Modelo   CLK MISO MOSI   CS        CS
  -------  --- ---- ---- ------- -----------
  ESP32    18   19   23    5         17
  ESP8266  D5   D6   D7    D8        D2
  RPi      11   9    10    8(CS0)    

  
  Resultado:
  Se puede dibujar en pantalla y cambiar el color de la tinta.
*/

#include <Arduino.h>
#include "RoJoILI9486.h" //Librería de gestión del display ILI9486
#include "RoJoXPT2046.h" //Librería de gestión del touch screen

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D3;
  const byte pinRES_display=D4;
  const byte pinCS_display=D8; //CS por defecto
  const byte pinCS_touchscreen=D2;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=5;
  const byte pinRES_display=22;
  const byte pinCS_display=21;
  const byte pinCS_touchscreen=17;
#endif

//Creamos objetos de gestión
RoJoILI9486 display;
RoJoXPT2046 ts;

//Paleta de colores
uint16_t palette[8];
//Color con el que se dibuja actualmente
uint16_t color=0xFFFF; //Inicialmente blanco

void setup()
{
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen);
  //Recuperamos la configuración del touch screen guardada
  ts.loadConfig();
  //Borramos la pantalla
  display.clear();
  //Definimos la paleta de colores
  palette[0]=0; //negro
  palette[1]=display.getColor(255,0,0); //rojo
  palette[2]=display.getColor(0,255,0); //verde
  palette[3]=display.getColor(0,0,255); //azul
  palette[4]=display.getColor(255,255,0); //amarillo
  palette[5]=display.getColor(255,0,255); //magenta
  palette[6]=display.getColor(0,255,255); //cyan
  palette[7]=0xFFFF; //blanco
  //Dibujamos los botones de la paleta
  for(uint16_t i=0;i<8;i++) display.rect(i*40,440,40,40,palette[i]);
}

void loop()
{
  int16_t x,y;
  //Si se detecta pulsación...
  if(ts.getXY(&x,&y))
  {
    //Si la pulsación está en la paleta...seleccionamos el nuevo color de la paleta
    if(y>440) color=palette[x/40];
    //Si la pulsación no está en la paleta...sibujamos el pixel
    else display.drawPixel(x,y,color);
  }
}

