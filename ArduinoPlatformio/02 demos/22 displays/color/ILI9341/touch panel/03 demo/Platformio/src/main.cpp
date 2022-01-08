/*
  Autor: Ramón Junquera
  Fecha: 20210205
  Tema: Touch Screen XPT2046 de ILI9341
  Objetivo: Demo de detección de pulsaciones en botones
  Material: breadboard, cables, display ILI9341
  Ejemplos: ESP32, ESP8266, Mega, UNO, Nano, Raspberry Pi

  Prerequisitos:
  Es necesario tener un archivo de configuración del touch screen guardado.

  Descripción:
    Crearemos una demo para dibujar con colores en el display.
    En la parte inferior dibujaremos una botonera con 8 distintos colores.
    Cada botón tiene un tamaño de 40x40 pixels.
    Haciendo click en ellos cambiará el color del lápiz.
    No se podrá dibujar encima de los botones.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8
  Mega     52   50   51  53
  UNO/Nano 13   12   11  10
  RPi      11   9    10  8(CS0)
  
  Nota 1:
    La librería RoJoXPT2046.h tiene en cuenta la constante de
    compilación ROJO_PIN_CS_SD para discriminar el sistema de
    almacenamiento de archivos, necesario para guardar/recuperar
    el archivo de configuración de touch screen.
  Nota 2:
    Configuración de Build de Geany para Raspberry Pi:
      Compile: g++ -Wall -std=c++11 -c "%f" -pthread -I"lib"
      Build: g++ -Wall -std=c++11 -o "%e" "%f" -pthread -I"lib"
      Execute: sudo "./%e"
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9486
#include <RoJoXPT2046.h> //Librería de gestión del touch screen

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D8;
  const byte pinCS_display=D3;
  const byte pinCS_touchscreen=D1;
  const byte pinIRQ_touchscreen=D0;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5;
  const byte pinCS_touchscreen=16;
  const byte pinIRQ_touchscreen=17;
#elif defined(__arm__) //Si es una Raspberry Pi
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
  const byte pinCS_touchscreen=7;
  const byte pinIRQ_touchscreen=17;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDC_display=5;
  const byte pinRES_display=4;
  const byte pinCS_display=3;
  const byte pinCS_touchscreen=6;
  const byte pinIRQ_touchscreen=2;
#endif

//Creamos objetos de gestión
RoJoILI9341 display;
RoJoXPT2046 ts;
//Variables globales
uint16_t palette[8]; //Paleta de colores
uint16_t currentColor; //Color actual
uint16_t currentColorIndex=7; //Índice de color seleccionado inicialmente
int16_t x,y; //Coordenadas actuales
int16_t xLast,yLast; //Últimas coordenadas
bool last=false; //Existe una pulsación anterior?. Inicialmente no

void setup() {
  //Serial.begin(115200);
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);

  palette[0]=0; //negro
  palette[1]=display.getColor(255,0,0); //rojo
  palette[2]=display.getColor(0,255,0); //verde
  palette[3]=display.getColor(0,0,255); //azul
  palette[4]=display.getColor(255,255,0); //amarillo
  palette[5]=display.getColor(255,0,255); //magenta
  palette[6]=display.getColor(0,255,255); //cyan
  palette[7]=display.getColor(255,255,255); //blanco

  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen,pinIRQ_touchscreen);
  //Cargamos configuración de touchscreen
  ts.loadConfig();
  //Dibujamos los botones de la paleta
  //Tienen un tamaño de 30x30, pero el perímetro que reservado para el recuadro de selección
  for(uint16_t i=0;i<8;i++) display.block(1+i*30,291,i*30+28,318,palette[i]);
  //Ponemos un recuadro blanco al color seleccionado
  display.rect(currentColorIndex*30,290,29,29,palette[7]);
  //Obtenemos el color actual seleccionado por defecto
  currentColor=palette[currentColorIndex];
}

void loop() {
  //Si se detecta pulsación...
	if(ts.getXY(&x,&y)) {
    //Si las coordenadas son válidas...
    if(x>=0) {
      //Si la pulsación está en la paleta...
      if(y>290) {
        //...seleccionamos el nuevo color de la paleta
        uint16_t index=x/30;
        //Si el índice de color seleccionado es distinto al actual...
        if(index!=currentColorIndex) {
          //Quitamos el recuadro de selección
          display.rect(currentColorIndex*30,290,29,29,0);
          //Tenemos un nuevo índice seleccionado
          currentColorIndex=index;
          //Dibujamos el recuadro de selección
          display.rect(currentColorIndex*30,290,29,29,palette[7]);
          //El nuevo color para dibujar ha cambiado
          currentColor=palette[currentColorIndex];
        }
        //No tendremos punto para el siguiente ciclo
        last=false;
      }
      //Si la pulsación no está en la paleta...
      else {
        //Si tenemos punto anterior...dibujamos una línea desde el pixel anterior al actual
        if(last) display.line(xLast,yLast,x,y,currentColor);
        //Si no tenemos punto anterior...tendremos uno para el próximo ciclo
        else last=true;
        //Las coordenadas anteriores serán las actuales
        xLast=x;
        yLast=y;
      }
    }
	}
	//Si no se detecta pulsación...no hay punto para el siguiente ciclo
	else last=false;
}
