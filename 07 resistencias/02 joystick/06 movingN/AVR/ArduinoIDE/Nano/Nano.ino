/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180320
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Basándonos en el ejercicio anterior, ahora moveremos una secuencia de puntos luminosos con ayuda
  del joystick.
  Para simplificar, la longitud del cuerpo de la serpiente se definirá en una constante.
  No variará en el tiempo.
  Las coordenadas de los puntos que forman el cuerpo se guardarán en dos arrays (una para cada
  coordenada).
  Tanto la posición de salida como la dirección inicial se toman aleatoriamente.

  Tras leer la posición del joystick, calcularemos las coordenadas de la nueva cabeza.
  Si coinciden con un led ya encendido, terminaremos el juego.
  Si no coinciden, se desplazarán todas las coordenadas una posición.
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
//Máxima longitud
const byte maxBody=5;
//Coordenadas de los puntos del cuerpo
byte bodyX[maxBody],bodyY[maxBody];

void setup()
{
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);
  //Inicializamos la semilla de números aleatorios
  randomSeed(analogRead(A2));
  //Calculamos las coordenadas del punto inicial
  headX=random(8);
  headY=random(8);
  //Creamos las coordenadas del cuerpo. Todas en el mismo punto inicial
  for(byte i=0;i<maxBody;i++)
  {
    bodyX[i]=headX;
    bodyY[i]=headY;
  }
  //Calculamos la dirección inicial
  dy=0;
  dx=random(3)-1;
  if(dx==0) dy=random(2)*2-1;
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
  //Calculamos la nueva posición de la cabeza
  headX=bodyX[0]+dx;
  headY=bodyY[0]+dy;
  //Nos aseguramos que sean valores entre 0 y 7
  headX=(headX+8)%8;
  headY=(headY+8)%8;
  //Si en esa posición ya hay un led encendido...
  if(display.videoMem->getPixel(headX,headY))
  {
    //...hemos terminado
    while(1)
    {
       display.videoMem->drawPixel(headX,headY,0);
       display.show();
       delay(100);
       display.videoMem->drawPixel(headX,headY,1);
       display.show();
       delay(100);
    }
  }
  //Podemos avanzar

  //Encendemos el led de cabeza
  display.videoMem->drawPixel(headX,headY,1);
  //Apagamos el último punto del cuerpo
  display.videoMem->drawPixel(bodyX[maxBody-1],bodyY[maxBody-1],0);
  //Desplazamos las coordenadas del cuerpo una posición
  for(byte i=maxBody-2;i<250;i--)
  {
    bodyX[i+1]=bodyX[i];
    bodyY[i+1]=bodyY[i];
  }
  //Guardamos las coordenadas de la cabeza en la primera posición
  bodyX[0]=headX;
  bodyY[0]=headY;
  //Dibujamos los leds
  display.show();
  
  //Esperamos un momento
  delay(200);
}
