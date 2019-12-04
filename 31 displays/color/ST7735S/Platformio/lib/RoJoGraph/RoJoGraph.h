/*
  Nombre de la librería: RoJoGraph.h
  Versión: 20190906
  Autor: Ramón Junquera
  Descripción:
    Funciones gráficas avanzadas
*/

#ifndef RoJoGraph_h
#define RoJoGraph_h

#include <Arduino.h>
#ifdef ROJO_PIN_CS_SD //Si debemos utilizar SD...
  #ifdef __arm__ //Si es una Raspberry...
    #error Raspberry has not SD
  #endif
  //No es una Raspberry
  //Puede tener SD. Lo cargamos
  #include <SD.h> //Librería de gestión de SD
#else //Si debemos utilizar SPIFFS...
  #ifdef ARDUINO_ARCH_AVR //Si es una placa Arduino...
    #error Arduino family has not SPIFFS
  #endif
  //No es una placa Arduino. Puede tener SPIFFS
  //Cargamos la librería en función del dispositivo
  #ifdef ESP32 //Si es un ESP32...
    #include <SPIFFS.h> //Librería de gestión SPIFFS para ESP32
  #else //Si cualquier otra cosa (ESP8266 o RPi)...
    #include <FS.h> //Librería de gestión SPIFFS para ESP8266
  #endif
#endif

//Estructura para guardar rangos de pantalla
struct displayRange {
  bool visible=false;
  uint16_t x1;
  uint16_t x2;
  uint16_t y1;
  uint16_t y2;
};

class RoJoGraph {
  private:
    void _ellipse(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color,bool fill); //Dibuja una elipse
  protected:
    byte _colorDepth=0; //Profundidad de color. Inicialmente ninguna
  public:
    void begin(); //Inicialización
    virtual uint16_t xMax(); //Anchura de display (dummy)
    virtual uint16_t yMax(); //Altura de display (dummy)
    displayRange visibleRange(int16_t x,int16_t y,uint16_t xx,uint16_t yy); //Calcula el rango visible
    uint16_t getColor(byte r,byte g,byte b); //Obtiene el código de color en base a sus componentes
    void getComponents(uint16_t color,byte *r,byte *g,byte *b); //Descompone un color en sus componentes
    virtual bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel (dummy)
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja una línea
    virtual bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno
    virtual void clear(uint16_t color=0); //Borra el área de dibujo
    virtual bool rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t borderColor); //Dibuja un rectángulo sin relleno
    void triangle(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,uint16_t borderColor); //Dibuja un triángulo sin relleno
    void triangleFill(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,uint16_t fillColor); //Dibuja un triángulo relleno
    void circle(int16_t x,int16_t y,int16_t r,uint16_t color); //Dibuja una circunferencia
    void disk(int16_t x,int16_t y,int16_t r,uint16_t color); //Dibuja un círculo
    void ellipse(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color); //Dibuja una elipse
    void ellipseFill(int16_t x,int16_t y,uint16_t rx,uint16_t ry,uint16_t color); //Dibuja una elipse rellena
    byte infoBMP(String filename,uint16_t *width,uint16_t *height,byte *colorDepth); //Información de archivo BMP
    virtual byte drawBMP(String filename,int16_t x=0,int16_t y=0); //Dibuja un bmp directamente de un archivo
    byte infoSprite(String filename,uint16_t *width,uint16_t *height,byte *colorDepth); //Información de archivo de sprite
    virtual byte drawSprite(String filename,int16_t x=0,int16_t y=0); //Dibuja un sprite directamente de un archivo
    bool printOver(String filenameFon,String text,uint16_t textColor,int16_t x=0,int16_t y=0); //Imprime texto
    bool infoPrint(String filenameFon,String text,uint16_t *width,uint16_t *heigth); //Información sobre un texto

}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoGraph.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

