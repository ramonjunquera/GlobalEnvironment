/*
  Autor: Ramón Junquera
  Fecha: 20190630
  Tema: Librería de touch screen para display RPI_ILI9486 SPI 3.5" 480*320
  Objetivo: Calibración del touch screen
  Material: breadboard, cables, display ILI9486, lector SD (obligatorio en
    placas Arduino)
  Ejemplos: ESP32, ESP8266, Mega, UNO, Nano, Raspberry Pi

  Descripción:
    Muestra unas líneas en el display que permiten calibrarlo y guardar la
    configuración en un archivo para futuros usos.

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
#include <RoJoILI9486.h> //Librería de gestión del display ILI9486
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
RoJoILI9486 display;
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
  display.block(x0,y0,x1,y1,colorYellow);
  //Pedimos coordenadas hasta que se detecte una pulsación
  while(!ts.getRawXY(x,y)) delay(1);
  //Redibujamos la línea en otro color para que el usuario sepa que se ha detectado la pulsación
  display.block(x0,y0,x1,y1,colorWhite);
  //Recogemos todas las coordenadas hasta que se deje de pulsar
  while(ts.getRawXY(x,y))
  {
    //Si las coordenadas son válidas...
    if(*x)
    {
      //Tenemos una muestra más
      items++;
      //Añadimos a los sumatorios
      xSum+=*x;
      ySum+=*y;
    }
    #ifdef ESP8266
      yield();
    #endif
  }
  //Se ha dejado de pulsar
  
  //Si hay alguna medida...
  if(items)
  {
    //...calculamos las medias y las guardamos
    *x=xSum/items;
    *y=ySum/items;
  }
  
  //Borramos la línea
  display.block(x0,y0,x1,y1,0);
}

void configTS()
{
  //Ejemplo de calibración del touch screen

  //Tamaño de pantalla
  const int32_t xMax=320;
  const int32_t yMax=480;
  //Coordenadas de hardware de los dos puntos de control
  int32_t x0,y0,x1,y1;
  //Coordenadas dummy
  uint16_t xDummy,yDummy;
  //Fijamos la rotación por defecto
  display.rotation(0);
  //Borramos la pantalla
  display.clear();
  //Solicitamos que se dibuje una línea que nos dará una coordenada de
  //hardware que se encuentra a 20 pixels del borde
  configTSline(20,20,xMax-20,20,&xDummy,&yDummy); y0=yDummy; //Obtenemos y0
  configTSline(20,yMax-20,xMax-20,yMax-20,&xDummy,&yDummy); y1=yDummy; //Obtenemos y1
  configTSline(20,20,20,yMax-20,&xDummy,&yDummy); x0=xDummy; //Obtenemos x0
  configTSline(xMax-20,20,xMax-20,yMax-20,&xDummy,&yDummy); x1=xDummy; //Obtenemos x1
  //Calculamos cuanto suponen 20 pixels de pantalla en coordenadas de hardware
  int32_t x20=((x1-x0)*20)/(xMax-40);
  int32_t y20=((y1-y0)*20)/(yMax-40);
  //Fijamos los valores de calibración
  //void setConfig(uint16_t xMaxDisplay,uint16_t yMaxDisplay,int16_t xMinTS,int16_t yMinTS,int16_t xMaxTS,int16_t yMaxTS);
  ts.setConfig(xMax,yMax,x0-x20,y0-y20,x1+x20,y1+y20);
}

void setup()
{
  Serial.begin(115200);
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
  //Inicializamos el touch screen
  ts.begin(pinCS_touchscreen,pinIRQ_touchscreen);
  //Calibramos el touch screen
  configTS();
  //Guardamos los parámetros actuales
  ts.saveConfig();
  //ts.loadConfig();
}

void loop()
{
  int16_t x,y;
  //Si se detecta pulsación...dibujamos el pixel
  if(ts.getXY(&x,&y)) display.drawPixel(x,y,0xFFFF);
  #ifdef ESP8266
    yield();
  #endif
}
