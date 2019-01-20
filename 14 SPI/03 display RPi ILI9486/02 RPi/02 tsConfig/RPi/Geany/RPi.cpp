/*
  Autor: Ramón Junquera
  Fecha: 20180521
  Tema: Touch Screen XPT2046 de ILI9486
  Objetivo: Ejemplo de calibración
  Material: placa RPi, display ILI9486 con touch screen XPT2016

  Descripción:
  Demostración del uso de la librería de gestión del touch screen XPT2046.

  La demostración incluye un par de rutinas que permiten calibrar el touch screen.
  Una vez obtenidos los valores, se guardan en la clase para que puedan ser
  utilizados, y se guardan en una archivo.
  Esto permitirá en un futuro recuperar los parámetros sin necesidad de
  repetir el proceso de calibración. Ni siquiera será necesario incluir
  las funciones para ello.

  Una vez terminada la calibración, dibujará los pixels correspondientes a las
  pulsaciones.
  
  Pinout SPI:
                         display touchscreen
  Modelo   CLK MISO MOSI   CS        CS
  -------  --- ---- ---- ------- -----------
  ESP32    18   19   23    5         17
  ESP8266  D5   D6   D7    D8        D2
  RPi      11   9    10    8(CS0)    7(CS1)
  
  Resultado:
  Tras la calibración, permite dibujar en pantalla para comprobar si está bien hecha.
*/

#include <Arduino.h>
#include "RoJoRPiILI9486.cpp" //Librería de gestión del display ILI9486
#include "RoJoXPT2046.cpp" //Librería de gestión del touch screen

//Definición de pines
const byte pinCS_touchscreen=7;

//Creamos objetos de gestión
RoJoRPiILI9486 display;
RoJoXPT2046 ts;

void configTSline(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t *x,uint16_t *y)
{
  //Parte de la rutina de calibración del touch screen
  //Dibuja una línea, recoge las medidas y devuelve la media

  //Definimos colores
  uint16_t colorWhite=display.getColor(255,255,255); //Cuando se dibuja
  uint16_t colorYellow=display.getColor(255,255,0); //Cuando se muestra

  //Definimos variables de sumatorios
  uint32_t xSum,ySum;
  //Inicializamos las variables de sumatorios
  xSum=ySum=0;
  //Definimos variable de número de muestras
  uint32_t items=0;
  //Dibujamos la línea en amarillo
  display.videoMem->line(x0,y0,x1,y1,colorYellow);
  display.show();
  //Pedimos coordenadas hasta que se detecte una pulsación
  while(!ts.getRawXY(x,y)) delay(1);
  //Redibujamos la línea en otro color para que el usuario sepa que se ha detectado la pulsación
  display.videoMem->line(x0,y0,x1,y1,colorWhite);
  display.show();
  //Recogemos todas las coordenadas hasta que se deje de pulsar
  while(ts.getRawXY(x,y))
  {
    //Tenemos una muestra más
    items++;
    //Añadimos a los sumatorios
    xSum+=*x;
    ySum+=*y;
  }
  //Se ha dejado de pulsar
  //Calculamos las medias y las guardamos
  *x=xSum/items;
  *y=ySum/items;
  //Borramos la línea
  display.videoMem->line(x0,y0,x1,y1,0);
}

void configTS()
{
  //Ejemplo de calibración del touch screen

  //Tamaño de pantalla
  const uint16_t xMax=320;
  const uint16_t yMax=480;
  //Coordenadas de hardware de los dos puntos de control
  uint16_t x0,y0,x1,y1;
  //Coordenada dummy
  uint16_t z;
  //Fijamos la rotación por defecto
  display.rotation(0);
  //Borramos la pantalla
  display.videoMem->clear();
  do
  {
    //Solicitamos que se dibuje una línea que nos dará una coordenada de
    //hardware que se encuentra a 20 pixels del borde
    configTSline(20,20,xMax-20,20,&z,&y0); //Obtenemos y0
    configTSline(20,yMax-20,xMax-20,yMax-20,&z,&y1); //Obtenemos y1
    configTSline(20,20,20,yMax-20,&x0,&z); //Obtenemos x0
    configTSline(xMax-20,20,xMax-20,yMax-20,&x1,&z); //Obtenemos x1
  }
  //Repetiremos si las coordenadas no parecen válidas
  while(x0<x1 || y0<y1);
  //Actualizamos la pantalla y quedará limpia
  display.show();
  //Calculamos cuanto suponen 20 pixels de pantalla en coordenadas de hardware
  int16_t x20=(x1-x0)/((xMax-40)/20);
  int16_t y20=(y1-y0)/((yMax-40)/20);
  //Enviamos los valores de calibración
  //void setConfig(uint16_t xMaxDisplay,uint16_t yMaxDisplay,int16_t xMinTS,int16_t yMinTS,int16_t xMaxTS,int16_t yMaxTS);
  ts.setConfig(xMax,yMax,x0-x20,y0-y20,x1+x20,y1+y20);
}

int main(int argc, char **argv)
{
  //Inicializamos el display
  display.begin();
  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen);
  //Calibramos el touch screen
  configTS();
  //Guardamos los parámetros actuales
  ts.saveConfig();
  
  int16_t x,y;
  while(1)
  {
    //Si se detecta pulsación...dibujamos el pixel
    if(ts.getXY(&x,&y))
    {
		display.videoMem->drawPixel(x,y,0xFFFF);
		display.show();
	}
  }
}

