/*
  Autor: Ramón Junquera
  Fecha: 20180520
  Tema: Touch Screen XPT2046 de ILI9486
  Objetivo: Demo de uso de rotaciones
  Material: placa ESP, display ILI9486 con touch screen XPT2016

  Descripción:
  Demostración del uso de la librería de gestión del touch screen XPT2046.

  La demo comienza cargando los parámetros de configuración de un archivo.
  Este archivo ha tenido que ser generado después de una calibración.
  En el ejemplo anterior se puede calibrar y generar este archivo.

  La demo cambia de rotación cada 10 segundos.
  La rotación es informada tanto al display como al touch screen para
  que ambos estén sincronizados.
  Para saber qué rotación está aplicada en cada momento, se escribe en
  la primera línea de pantalla el código de la rotación.

  
  Pinout SPI:
                         display touchscreen
  Modelo   CLK MISO MOSI   CS        CS
  -------  --- ---- ---- ------- -----------
  ESP32    18   19   23    5         17
  ESP8266  D5   D6   D7    D8        D2
  RPi      11   9    10    8(CS0)    

  
  Resultado:
  Aunque la pantalla vaya cambiando de rotación, siempre se puede dibujar
  en ella porque el puntero coincide con los pixels dibujados.
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

//Rotación actual
byte rotation=0;
//Momento de siguiente cambio
uint32_t nextChange;
//Color
uint16_t color=display.getColor(255,255,255);

void setup()
{
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen);
  //Recuperamos la configuración del touch screen guardada
  ts.loadConfig();
}

void loop()
{
  //Si toca hacer el siguiente cambio...
  if(millis()>nextChange)
  {
    //Borramos la pantalla
    display.clear();
    //Pasamos a la siguiente rotación
    rotation=++rotation%4;
    //Aplicamos la rotación a la pantalla y al touch screen
    display.rotation(rotation);
    ts.rotation(rotation);
    //Escribimos en pantalla el tipo de rotación
    display.print("/FreeMono24pt7b.fon","Rotation:"+String(rotation),color,0,0);
    //Anotamos el momento del siguiente cambio. En 10 segundos
    nextChange=millis()+10000;
  }

  int16_t x,y;
  //Si se detecta pulsación...dibujamos el pixel
  if(ts.getXY(&x,&y)) display.drawPixel(x,y,display.getColor(255,255,255));
}

