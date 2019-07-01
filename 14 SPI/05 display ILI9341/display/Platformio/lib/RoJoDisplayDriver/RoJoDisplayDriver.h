/*
  Nombre de la librería: RoJoILI9486.h
  Versión: 20190628
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RPI_ILI9486 SPI 3.5" 320x480
*/

#ifndef RoJoDisplayDriver_h
#define RoJoDisplayDriver_h

#include <Arduino.h>
#include <SPI.h> //Gestión SPI
#include <stdarg.h> //Para número variable de parámetros
#include <RoJoGraph.h> //Funciones gráficas avanzadas
#include <RoJoSprite16.h> //Gestión de sprites color

class RoJoDisplayDriver:public RoJoGraph
{
  protected:
    byte _pinDC; //Pin DC de display
    byte _pinRES; //Pin reset de display
    byte _pinCS; //Pin CS de display
    SPISettings _spiSetting; //Características de la conexión SPI
    virtual uint32_t _maxFreqSPI(); //Máxima frecuencia SPI soportada por el display
    virtual void _startSPI(); //Inicia una transacción SPI
    virtual void _endSPI(); //Finaliza una transacción SPI
    virtual void _writeCommand(byte command,...); //Envía al display un comando con sus correspondientes parámetros
    virtual void _setCursorRangeY(int16_t y1,int16_t y2);
    virtual void _setCursorRangeX(int16_t x1,int16_t x2);
    virtual void _setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2);

  public:
    virtual uint16_t xMax(); //Anchura de display (dummy)
    virtual uint16_t yMax(); //Altura de display (dummy)
    virtual bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno (dummy)
    virtual void reset(); //Reset
    virtual void begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqSPI=0); //Inicialización
    virtual bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel (dummy)
    virtual bool drawSprite(int16_t x,int16_t y,String filename); //Dibuja un sprite directamente de un archivo
    virtual byte drawBMP(int16_t x,int16_t y,String filename); //Dibuja un bmp directamente de un archivo
    virtual void drawSprite(int16_t x,int16_t y,RoJoSprite16 *sprite); //Dibuja un sprite
    virtual bool drawSpriteSync(int16_t x,int16_t y,RoJoSprite16 *source,RoJoSprite16 *destination); //Sincroniza dos sprites
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoDisplayDriver.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

