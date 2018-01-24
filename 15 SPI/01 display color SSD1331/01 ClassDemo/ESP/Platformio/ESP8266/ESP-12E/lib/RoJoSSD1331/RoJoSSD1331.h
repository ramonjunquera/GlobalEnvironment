/*
  Nombre de la librería: RoJoSSD1331.h
  Versión: 20171104
  Autor: Ramón Junquera
  Descripción:
    Gestión de display OLED SPI 0.95" 96x64 SSD1331
    Permite la gestión de sprites
    Sólo compatible con las familias de placas ESP32 y ESP8266 y
      Raspberry Pi 3
*/

#ifndef RoJoSSD1331_h
#define RoJoSSD1331_h

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266) && !defined(__arm__)
  #error Library RoJoSprite16 is only compatible with ESP32 & ESP8266 family devices or Raspberry Pi 3
#endif  

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI por hardware
#include "RoJoSprite16.h"

class RoJoSSD1331
{
  private:
    //Variables internas de los pines
    byte _pinDC;
    byte _pinRES;
    byte _pinCS;
    //Frecuencia por defecto de la conexión SPI = 80MHz (la más alta que soporta el bus SPI)
    uint32_t _spiFreq=80000000;
    //Características de la conexión SPI
    //Baudrate a la frecuencia definida, con orden MSBFIRST y modo SPI_MODE3 (polaridad, fase, borde)
    //  _spiSetting = SPISettings(_spiFreq, MSBFIRST, SPI_MODE3);
    //Se define en el método begin
    SPISettings _spiSetting;
    //Inicia una transacción SPI
    void _startSPI();
    //Finaliza una transacción SPI
    void _endSPI();
    //Escribe un comando en el display
    void _writeCommand(byte *buffer,byte len);
    //Escribe un dato en el display
    void _writeData(uint16_t d);
    //Activa/desactiva el relleno de los rectángulos
    void _fill(bool f);
    //Dibuja un rectángulo. Función interna
    void _rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB,byte fillR,byte fillG,byte fillB);
    //Intercambia los valores de dos variables enteras
    void _swap(int16_t *a,int16_t *b);
    //Dibuja un pixel del color indicado en la posición del cursor. Método privado
    void _setPixel(uint16_t color);
    
  public:
    const byte xMax=96; //Anchura de display
    const byte yMax=64; //Altura de display
    void reset();
    void begin(byte pinRES,byte pinDC,byte pinCS); //Inicialización
    void begin(byte pinRES,byte pinDC,byte pinCS,uint32_t spiFreq); //Inicialización
    //Dibuja un pixel en ciertas coordenadas
    void setPixel(int16_t x,int16_t y,uint16_t color);
    //Dibuja un pixel del color indicado en la posición del cursor
    void setPixel(uint16_t color);
    //Obtiene el código de color en base a sus componentes
    uint16_t getColor(byte r,byte g,byte b);
    //Borra la pantalla
    void clear();
    //Dibuja un rectángulo relleno
    void rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB,byte fillR,byte fillG,byte fillB);
    void rect(byte x1,byte y1,byte x2,byte y2,uint16_t borderColor,uint16_t fillColor);
    //Dibuja un rectángulo sin relleno
    void rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB);
    void rect(byte x1,byte y1,byte x2,byte y2,uint16_t borderColor);
    //Copia un área en otra
    void copy(byte x1,byte y1,byte x2,byte y2,byte x3,byte y3);
    //Hace una zona más oscura
    void darker(byte x1,byte y1,byte x2,byte y2);
    //Descompone un color en sus componentes
    void getComponents(uint16_t color,byte *r,byte *g,byte *b);
    //Dibuja una línea
    void line(byte x1,byte y1,byte x2,byte y2,byte r,byte g,byte b);
    void line(byte x1,byte y1,byte x2,byte y2,uint16_t color);
    //Dibuja un sprite
    void drawSprite(int16_t x,int16_t y,RoJoSprite16 *sprite);
    //Fija el rango horizontal del cursor
    void setCursorRangeX(int16_t x1,int16_t x2);
    //Fija el rango vertical del cursor
    void setCursorRangeY(int16_t y1,int16_t y2);
}; //Punto y coma obligatorio para que no de error

#endif

