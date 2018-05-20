/*
  Nombre de la librería: RoJoILI9486.h
  Versión: 20180519
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RPI_ILI9486 SPI 3.5" 320x480
    Permite la gestión de sprites
*/

#ifndef RoJoILI9486_h
#define RoJoILI9486_h

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI
#include "RoJoSprite16.h" //Gestión de sprites color

#ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
  #error Arduino family has not SPIFFS
#elif defined(ESP32) //Si es un ESP32...
  #include <SPIFFS.h> 
#else //Si cualquier otra cosa (ESP8266 o RPi)...
  #include <FS.h>
#endif

struct displayRange
{
  //Guarda el resultado del rango de pantalla visible
  //Utilizado para definición de rangos de plantalla y dibujo de sprites
  
  bool visible=false;
  int16_t x1;
  int16_t x2;
  int16_t y1;
  int16_t y2;
};


class RoJoILI9486
{
  private:
    //Tamaño del display sin rotación
    const uint16_t _xMaxDefault=320; //Anchura de display
    const uint16_t _yMaxDefault=480; //Altura de display
    //Tamaño del display teniendo en cuenta la rotación
    uint16_t _xMax=320; //Anchura de display
    uint16_t _yMax=480; //Altura de display
    //Configuraciones de rotación
    byte _rotationCodes[4]={0b01001000,0b00101000,0b10001000,0b11101000};
    //Variables internas de los pines
    byte _pinDC_display;
    byte _pinRES_display;
    byte _pinCS_display;
    //Características de la conexión SPI
    const SPISettings _spiSetting=SPISettings(39999999,MSBFIRST,SPI_MODE0); //Frecuencia <40MHz
    //Inicia una transacción SPI
    void _startSPI();
    //Finaliza una transacción SPI
    void _endSPI();
    //Cambia estado del pin CS
    void _changeCS(bool s);
    //Escribe un comando en el display
    void _writeCommand(byte command);
    //Escribe un en el display
    void _writeData(byte d); //Un dato de 8bits
    //Intercambia los valores de dos variables enteras
    void _swap(int16_t *a,int16_t *b);
    //Fija el rango del del cursor
    void _setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2);
    //Calcula el rango visible de un sprite
    void visibleRange(int16_t x,int16_t y, int16_t xLength, int16_t yLength, displayRange *answer);
    
    //Variables para gestión de fuentes
    File  _f; //Archivo de fuentes
    byte _charMin=0; //Primer carácter ASCII incluido en librería
    byte _charMax=0; //Último carácter ASCII incluido en librería
    byte _pages=0; //Altura en páginas
    byte _charCount=0; //Número de caracteres de la fuente
    //Calcula la anchura de un carácter
    byte _charWidth(byte c);
    //Devuelve el índice de inicio de los datos gráficos de un carácter
    uint16_t _charIndex(byte c);
  public:
    void softReset();
    void hardReset();
    //Sale del modo de bajo consumo
    void sleepOut();
    //Entra en modo de bajo consumo
    void sleepIn();
    //Activar display
    void on();
    //Desactivar display
    void off();
    //Inicialización
    void begin(byte pinRES,byte pinDC,byte pinCS=SS);
    //Dibuja un pixel en ciertas coordenadas
    void drawPixel(int16_t x,int16_t y,uint16_t color);
    //Obtiene el código de color en base a sus componentes
    uint16_t getColor(byte r,byte g,byte b);
    //Borra la pantalla
    void clear();
    //Descompone un color en sus componentes
    void getComponents(uint16_t color,byte *r,byte *g,byte *b);
    //Dibuja un rectángulo relleno
    void rect(int16_t x,int16_t y,uint16_t xLength,uint16_t yLength,uint16_t fillColor);
    //Dibuja un sprite
    void drawSprite(int16_t x,int16_t y,RoJoSprite16 *sprite);
    bool drawSprite(int16_t x,int16_t y,String fileName); //Directamente de un archivo
    //Dibuja una línea
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color);
    //Dibuja un texto
    bool print(String fileNameFon,String text,uint16_t textColor,int16_t x,int16_t y);
    //Configura la rotación
    void rotation(byte r);
    //Devuelve la anchura
    uint16_t xMax();
    //Devuelve la altura
    uint16_t yMax();
}; //Punto y coma obligatorio para que no de error

#endif

