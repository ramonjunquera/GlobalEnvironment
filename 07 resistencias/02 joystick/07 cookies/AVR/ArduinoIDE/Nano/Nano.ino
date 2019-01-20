/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180320
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Último ejercicio de snake!.
  Ahora tenemos galletas que aparecen aleatoriamente.
  Cada vez que la serpiente como una, se hace un poco más grande.
*/

#include <Arduino.h>
#include "RoJoMAX7219SD.h" //Librería de gestión de MAX7219

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;

//Definición de pines
const byte pinDIN_display=4;
const byte pinCS_display=3;
const byte pinCLK_display=2;
//Posición de la cabeza
byte headX,headY;
//Posición de la galleta
byte cookieX,cookieY;
//Dirección
int dx,dy;
//Máxima longitud
const byte maxBody=20;
//Longitud del cuerpo
byte bodyLength=1;
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
  bodyX[0]=random(8);
  bodyY[0]=random(8);
  //Calculamos la dirección inicial
  dy=0;
  dx=random(3)-1;
  if(dx==0) dy=random(2)*2-1;
  //Dibujamos le cabeza
  display.videoMem->drawPixel(bodyX[0],bodyY[0],1);
  //Creamos una nueva galleta
  NewCookie();
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
  //Si la nueva posición de la cabeza coincide con la galleta...
  if(headX==cookieX && headY==cookieY)
  {
    //Si la longitud del cuerpo no es la máxima...aumentamos la longitud en uno
    if(bodyLength<maxBody) bodyLength++;
    //Desplazamos las coordenadas del cuerpo una posición
    Move();
    //Calculamos la posición de una nueva galleta
    NewCookie();
  }
  else //La nueva posición de la cabeza no coincide con la galleta
  {
    //Si la nueva posición ya está ocupada...
    if(display.videoMem->getPixel(headX,headY))
    {
      //...hemos terminado
      GameOver();
    }
    else //La nueva posición está libre
    {
      //Apagamos el último punto del cuerpo
      display.videoMem->drawPixel(bodyX[bodyLength-1],bodyY[bodyLength-1],0);
      //Desplazamos las coordenadas del cuerpo una posición
      Move();
    }
  }
  //Apagamos la galleta
  display.videoMem->drawPixel(cookieX,cookieY,0);
  //Dibujamos el escenario
  display.show();
  //Esperamos un momento
  delay(150);
  //Encendemos la galleta
  display.videoMem->drawPixel(cookieX,cookieY,1);
  //Dibujamos el escenario
  display.show();
  //Esperamos un momento
  delay(150);
}

void NewCookie()
{
  //Crea una nueva galleta en una posición vacía

  do
  {
    cookieX=random(8);
    cookieY=random(8);
  } 
  while (display.videoMem->getPixel(cookieX,cookieY));
}

void GameOver()
{
  //Fin de juego
  //Parpadea la cabeza

  //Repite para siempre
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

void Move()
{
  //Desplaza las coordenadas del cuerpo una posición, guardamos y encendemos la cabeza

  //Desplazamos las coordenadas del cuerpo una posición
  for(byte i=bodyLength-2;i<250;i--)
  {
    bodyX[i+1]=bodyX[i];
    bodyY[i+1]=bodyY[i];
  }
  //Guardamos las coordenadas de la cabeza en la primera posición
  bodyX[0]=headX;
  bodyY[0]=headY;
  //Encendemos el led de cabeza
  display.videoMem->drawPixel(headX,headY,1);
}

