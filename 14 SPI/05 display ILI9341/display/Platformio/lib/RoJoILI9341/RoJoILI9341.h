/*
  Nombre de la librería: RoJoILI9341.h
  Versión: 20190629
  Autor: Ramón Junquera
  Descripción:
    Gestión de display ILI9341 SPI 240x320
*/

#ifndef RoJoILI9341_h
#define RoJoILI9341_h

#include <Arduino.h>
#include <RoJoDisplayDriver.h>

class RoJoILI9341:public RoJoDisplayDriver
{
  private:
    byte _rotationCodes[4]={0b01001000,0b00101000,0b10001000,0b11101000}; //Configuraciones de rotación
    const uint16_t _xMaxDefault=240; //Anchura de display sin rotación
    const uint16_t _yMaxDefault=320; //Altura de display sin rotación
    uint16_t _xMax; //Anchura de display teniendo en cuenta la rotación
    uint16_t _yMax; //Altura de display teniendo en cuenta la rotación
    uint32_t _maxFreqSPI(); //Máxima frecuencia SPI soportada por el display
    void _writeCommand(byte command,...); //Envía al display un comando con sus correspondientes parámetros
    void _setCursorRangeY(int16_t y1,int16_t y2);
    void _setCursorRangeX(int16_t x1,int16_t x2);
  public:
    void rotation(byte r); //Configura la rotación
    void sleep(bool mode); //Activa/Desactiva el modo hibernación
    uint16_t xMax(); //Anchura de display
    uint16_t yMax(); //Altura de display
    bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno
    void reset(); //Reset
    bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoILI9341.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

