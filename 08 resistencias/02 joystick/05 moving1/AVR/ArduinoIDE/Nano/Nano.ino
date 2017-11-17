/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  
  Material adicional: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Un punto luminoso se mueve por la matriz en hotizontal o vertical a velocidad constante
  y respondiendo a las órdenes del joy.
*/

#include <Arduino.h>
#include <RoJoMatrix.h>

//Creamos el objeto m que gestionará la matriz (de matrices) de leds
//En la creación ya se incluye la activación y la inicialización
//tras ello estará lista para ser utilizada
//Los parámetros son:
//  pin del DIN (DATA)
//  pin del CLK
//  pin del LOAD(/CS)
//  número de matrices enlazadas
RoJoMatrix m(4,2,3,1);
//Posición
int headX,headY;
//Dirección
int dx,dy;

void setup()
{
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
  headX=(headX+8)%8;
  headY=(headY+8)%8;
  
  //Dibujamos la nueva posición
  m.Clear();
  m.SetLed(headX,headY,1);
  m.Refresh();
  
  //Esperamos un momento
  delay(200);
}
