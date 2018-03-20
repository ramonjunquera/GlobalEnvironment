/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180320  
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Un punto luminoso se mueve por la matriz en horizontal o vertical a velocidad constante
  y respondiendo a las órdenes del joy.
*/

#include <Arduino.h>
#include "RoJoMAX7219SD.h" //Librería de gestión de MAX7219

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;

//Definición de pines
const byte pinDIN_display=4;
const byte pinCS_display=3;
const byte pinCLK_display=2;
//Posición
int headX,headY;
//Dirección
int dx,dy;

void setup()
{
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);
  
  //Comenzaremos desde el centro
  headX=3;
  headY=3;
  //Inicialmente se mueve hacia la derecha
  dx=1;
  dy=0;
}

void loop()
{
  //Leemos las entradas analógicas para x & y
  int x=analogRead(A0);
  int y=analogRead(A1);
  //Convertimos las coordenadas a un valor entre 0 y 2
  x/=342;
  y/=342;
  
  //Calculamos la dirección
  //Si se pide un sentido en horizontal...
  if(x!=1)
  {
    dx=x-1;
    dy=0;  
  }
  else //No se pide un sentido horizontal
  {
    //Si se pide un sentido vertical...
    if(y!=1)
    {
      dy=y-1;
      dx=0;
    }
  }
  
  //Calculamos la nueva posición
  headX+=dx;
  headY+=dy;
  //Nos aseguramos que sean valores entre 0 y 7
  //Esto quiere decir que si nos salimos de la pantalla apareceremos en el extremo opuesto
  headX=(headX+8)%8;
  headY=(headY+8)%8;
  
  //Dibujamos la nueva posición
  display.videoMem->clear();
  display.videoMem->drawPixel(headX,headY,1);
  display.show();
  
  //Esperamos un momento
  delay(200);
}
