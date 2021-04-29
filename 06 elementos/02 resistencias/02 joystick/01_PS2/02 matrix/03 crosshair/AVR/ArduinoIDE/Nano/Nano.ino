/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180319
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Ahora que somos capaces de posicionar un led en relación al estado del joy, mostraremos un 
  punto de mira (un sprite) gracias a las capacidades de la librería RoJoSprite.

  Cuando pulsemos el botón del joy, se quedará encendido el led marcado.

  El circuito es el mismo que el del ejercicio anterior.
  
  Resultado:
  Podemos encender leds, señalándolos con un punto de mira
*/

#include <Arduino.h>
#include "RoJoMAX7219SD.h" //Librería de gestión de MAX7219
#include "RoJoSpriteSD.h" //Librería de gestión de sprites monocromos

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;
//Creamos el sprite del punto de mira
RoJoSprite crosshair;
//Creamos el sprite de background
RoJoSprite back;

//Definición de pines
const byte pinButton=5;
const byte pinDIN_display=4;
const byte pinCS_display=3;
const byte pinCLK_display=2;

void setup()
{
  //Configuramos el pin del botón del joy como entrada con resistencias de pullup activas
  pinMode(pinButton,INPUT_PULLUP);
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);
  //Dimensionamos el sprite del punto de mira. Anchura=5. Páginas=1
  crosshair.setSize(5,1);
  //Lo dibujamos
  //void drawPage(int16_t x,int16_t page,byte mask,byte color);
  crosshair.drawPage(0,0,0b00000100,4); //4=escribir el valor tal cual
  crosshair.drawPage(1,0,0b00000100,4);
  crosshair.drawPage(2,0,0b00011011,4);
  crosshair.drawPage(3,0,0b00000100,4);
  crosshair.drawPage(4,0,0b00000100,4);
  //Dimensionamos el sprite de background. Anchura=8. Páginas=1. Como el display
  back.setSize(8,1);
}

void loop()
{
  //Leemos la entrada analógica para x
  int x=analogRead(A0);
  //Leemos la entrada analógica para y
  int y=analogRead(A1);
  //Leemos el estado del pulsador. 1=suelto, 1=presionado
  bool b=digitalRead(pinButton);

  //Convertimos las coordenadas a un valor entre 0 y 7
  x/=128;
  y/=128;
  //Si se ha pulsado el botón...encendemos el pixel seleccionado
  if(!b) back.drawPixel(x,y,1); //1=encender pixel
  //Dibujamos el sprite de fondo. Opaco
  display.videoMem->drawSpritePage(0,0,&back,4); //4=sobreescribiendo
  //Dibujamos sprite del punto de mira, con el fondo transparente
  display.videoMem->drawSprite(x-2,y-2,&crosshair,1); //1=fondo transparente
  //Mostramos la memoria de vídeo en la matriz
  display.show();
  //Esperamos un momento
  delay(50);
}
