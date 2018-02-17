/*
  Nombre de la librería: RoJoLCD5110.h
  Versión: 20180213
  Autor: Ramón Junquera
  Descripción:
    Gestión de display Nokia LCD 5110 SPI 84x48
*/

#ifndef RoJoLCD5110_h
#define RoJoLCD5110_h

//Si declaramos la siguiente línea utilizaremos una tarjeta SD como almacenamiento por defecto
//en vez de un sistema SPIFFS
#define UseSD

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI
#ifdef UseSD //Si debemos utilizar una tarjeta SD...
  #include "RoJoSpriteSD.h"
#else //Si debemos utilizar SPIFFS
  #include "RoJoSprite.h"
#endif

class RoJoLCD5110
{
  public:
    static const byte xMax=84; //Anchura de display en pixels
    static const byte pagesMax=6; //Altura de display de páginas
    static const byte yMax=8*pagesMax; //Altura de display en pixels
  private:
    //Variables internas de los pines
    byte _pinDC;
    byte _pinRST;
    byte _pinCS;
    //Frecuencia por defecto de la conexión SPI
    uint32_t _spiFreq=25000000; //25MHz
    //Características de la conexión SPI
    //Baudrate a la frecuencia definida, con orden MSBFIRST y modo SPI_MODE0 (polaridad, fase, borde)
    //  _spiSetting = SPISettings(_spiFreq, MSBFIRST, SPI_MODE0);
    //Se define en el método begin
    SPISettings _spiSetting;
    //Inicia una transacción SPI
    void _startSPI();
    //Finaliza una transacción SPI
    void _endSPI();
    //Escribe un comando en el display
    void _writeCommand(byte c);
    //Escribe una colección de datos en el display
    void _writeData(byte *buffer,uint16_t len);
    //Página en la que se está escribiendo
    byte _currentPage=255;
    //Aplica una máscara al byte de una página (de la memoria de vídeo)
    void _mask(byte page,byte x,byte mask,byte color);
    //Intercambia los valores de dos variables enteras
    void _swap(int16_t *a,int16_t *b);
    //Dibuja una línea horizontal
    void _lineH(int16_t y,int16_t x1,int16_t x2,byte color,bool withoutBorder);
    //Dibuja un rectángulo relleno
    void _rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,bool withoutBorder);
    //Las coordenadas están dentro de la pantalla?
    bool _inScreen(int16_t x,int16_t y);   
  public:
    //Memoria del display. La primera dimensión es y=página
    //La segunda es x=columna
    byte videoMem[pagesMax][xMax];
    //Resetea el display  
    void reset();
    //Borra la pantalla
    void clear();
    //Envía memoria de vídeo al display
    void show();
    //Inicialización
    void begin(byte pinRST,byte pinDC);
    void begin(byte pinRST,byte pinDC,byte pinCS);
    void begin(byte pinRST,byte pinDC,byte pinCS,uint32_t spiFreq);
    //Dibuja un un pixel
    void setPixel(int16_t x,int16_t y,byte color);
    //Obtener el estado de un pixel
    bool getPixel(int16_t x,int16_t y);
    //Dibuja una línea vertical
    void lineV(int16_t x,int16_t y1,int16_t y2,byte color);
    //Dibuja una línea horizontal
    void lineH(int16_t y,int16_t x1,int16_t x2,byte color);
    //Dibuja un rectángulo relleno
    void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor);
    //Dibuja rectángulo con borde y relleno
    void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,byte borderColor);
    //Dibuja una línea
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte color);
    //Dibuja un sprite en una página
    void drawSpritePage(int16_t x,int16_t page,RoJoSprite *sprite,byte color);
    //Dibuja un sprite en unas coordenadas
    void drawSprite(int16_t x,int16_t y,RoJoSprite *sprite,byte color);
    //Toma parte de la memoria de vídeo para crear un sprite
    void getSprite(int16_t x1,int16_t page1,int16_t x2,int16_t page2,RoJoSprite *sprite);
    //Fija el display en modo invertido (blanco <-> negro)
    void reverse(bool reserveMode);
    //Fija el contraste
    void setContrast(byte c);
}; //Punto y coma obligatorio para que no de error

#endif

