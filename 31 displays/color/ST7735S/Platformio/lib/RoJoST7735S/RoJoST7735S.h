/*
  Nombre de la librería: RoJoST7735S.h
  Versión: 20191103
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RoJoST7735S SPI 80x160
*/

#ifndef RoJoST7735S_h
#define RoJoST7735S_h

#include <Arduino.h>
#include <SPI.h> //Gestión SPI
#include <RoJoSprite.h> //Gestión de sprites
#include <RoJoGraph.h> //Gestión de gráficos avanzados
#ifdef ARDUINO_M5Stick_C
  #include <Wire.h> //Para gestión de chip de alimentación AXP192
#endif

class RoJoST7735S:public RoJoGraph {
  private:
    byte _pinDC; //Pin DC de display
    byte _pinRES; //Pin reset de display
    byte _pinCS; //Pin CS de display
    void _setCursorRangeX(int16_t x1,int16_t x2); //Define rango X
    void _setCursorRangeY(int16_t y1,int16_t y2); //Define rango Y
    void _setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2); //Define  rango X & Y
    const uint16_t _xMaxDefault=80; //Anchura de display sin rotación
    const uint16_t _yMaxDefault=160; //Altura de display sin rotación
    uint16_t _xMax; //Anchura de display
    uint16_t _yMax; //Altura de display
    void _writeCommand(byte command,...); //Envía al display un comando con sus correspondientes parámetros
    void _startCOMM(); //Inicia comunicación
    void _endCOMM(); //Finaliza comunicación
    SPISettings _spiSetting; //Características de la conexión SPI
    byte _rotationCodes[4]={0xCC,0xA8,0x08,0x68}; //Configuraciones de rotación
    const uint16_t _x0Default=26; //Coordenada x asignada a la primera columna sin rotación
    const uint16_t _y0Default=1; //Coordenada y asignada a la primera fila sin rotación
    uint16_t _x0; //Coordenada x asignada a la primera columna
    uint16_t _y0; //Coordenada y asignada a la primera fila
  public:
    void sleep(bool mode); //Activa/Desactiva el modo hibernación
    uint16_t xMax(); //Anchura de display
    uint16_t yMax(); //Altura de display
    bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno
    void reset(); //Reset
    bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel
    void begin(byte pinRES=18,byte pinDC=23,byte pinCS=5,uint32_t freqCOMM=39999999,byte pinCLK=13,byte pinMOSI=15,int8_t pinMISO=-1); //Inicialización
    bool drawSprite(RoJoSprite *sprite,int16_t x=0,int16_t y=0); //Dibuja un sprite
    byte drawSprite(String filename,int16_t x=0,int16_t y=0); //Dibuja un sprite directamente de un archivo
    bool drawSpriteSync(RoJoSprite *source,RoJoSprite *destination,int16_t x=0,int16_t y=0); //Sincroniza dos sprites
    void rotation(byte r); //Configura la rotación
};

#ifdef __arm__
  #include <RoJoST7735S.cpp> //Para guardar compatibilidad con RPi
#endif

#endif