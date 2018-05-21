/*
  Autor: Ramón Junquera
  Fecha: 20180521
  Tema: Touch Screen XPT2046 de ILI9486
  Objetivo: Demo de detección de pulsaciones en botones
  Material: placa RPi, display ILI9486 con touch screen XPT2016

  Descripción:
  Es necesario tener un archivo de configuración del touch screen guardado.
  
  Crearemos una demo para dibujar con colores en el display.
  En la parte inferior dibujaremos una botonera con 8 distintos colores.
  Cada botón tiene un tamaño de 40x40 pixels.
  Haciendo click en ellos cambiará el color del lápiz.
  No se podrá dibujar encima de los botones.

  Puesto que con el driver de vídeo específico para RPi, el refresco de
  pantalla es inteligente y por lo tanto más lento para mostrar un sólo
  pixel, se ha modificado la rutina utilizada en las placas ESP para que
  no diguje puntos, sino líneas.

  Como posible mejora futura se podría dibujar un rectángulo blanco alrededor
  del color de la paleta que está seleccionado. Así el usuario sabría con
  qué color se dibujará.
  
  Pinout SPI:
                         display touchscreen
  Modelo   CLK MISO MOSI   CS        CS
  -------  --- ---- ---- ------- -----------
  ESP32    18   19   23    5         17
  ESP8266  D5   D6   D7    D8        D2
  RPi      11   9    10    8(CS0)    7(CS1)
  
  Resultado:
  Se puede dibujar en pantalla y cambiar el color de la tinta.
*/

#include <Arduino.h>
#include "RoJoRPiILI9486.cpp" //Librería de gestión del display ILI9486
#include "RoJoXPT2046.cpp" //Librería de gestión del touch screen

//Definición de pines
const byte pinCS_touchscreen=7;

//Creamos objetos de gestión
RoJoRPiILI9486 display;
RoJoXPT2046 ts;

int main(int argc, char **argv)
{
  //Inicializamos el display
  display.begin();
  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen);
  //Recuperamos la configuración del touch screen guardada
  ts.loadConfig();
  //Borramos la pantalla
  display.videoMem->clear();
  //Paleta de colores
  uint16_t palette[8];
  palette[0]=0; //negro
  palette[1]=display.getColor(255,0,0); //rojo
  palette[2]=display.getColor(0,255,0); //verde
  palette[3]=display.getColor(0,0,255); //azul
  palette[4]=display.getColor(255,255,0); //amarillo
  palette[5]=display.getColor(255,0,255); //magenta
  palette[6]=display.getColor(0,255,255); //cyan
  palette[7]=0xFFFF; //blanco
  //Dibujamos los botones de la paleta
  for(uint16_t i=0;i<8;i++) display.videoMem->clear(i*40,440,i*40+40,480,palette[i]);
  //Refrescamos la pantalla
  display.show();
  
  //Color con el que se dibuja actualmente
  uint16_t color=0xFFFF; //Inicialmente blanco
  //Coordenadas actuales y anteriores
  int16_t x,y,xLast,yLast;
  //Existe una pulsación anterior?
  bool last=false; //Inicialmente no
  while(1)
  {
	  //Si se detecta pulsación...
	  if(ts.getXY(&x,&y))
	  {
		//Si la pulsación está en la paleta...
		if(y>440)
		{
			//...seleccionamos el nuevo color de la paleta
			color=palette[x/40];
			//No tendremos punto para el siguiente ciclo
			last=false;
		}
		//Si la pulsación no está en la paleta...
		else
		{
			//Si tenemos punto anterior...
			if(last)
			{
				//...dibujamos el una línea desde el pixel anterior al actual
				display.videoMem->line(xLast,yLast,x,y,color);
				display.show();
			}
			//Si no tenemos punto anterior...tendremos uno para el próximo ciclo
			else last=true;
			//Lo anotamos
			xLast=x;
			yLast=y;
		}
	  }
	  //Si no se detecta pulsación...no hay punto para el siguiente ciclo
	  else last=false;
  }
}

