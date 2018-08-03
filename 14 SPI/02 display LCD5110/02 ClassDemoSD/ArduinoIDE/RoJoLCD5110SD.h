/*
  Nombre de la librería: RoJoLCD5110.h
  Versión: 20180222
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
  private:
    //Variables internas de los pines
    byte _pinDC_display;
    byte _pinRST_display;
    byte _pinCS_display;
    //Frecuencia por defecto de la conexión SPI
    #ifdef ARDUINO_ARCH_AVR //Para placas Arduino...
      const uint32_t _freq_display=3999999; //Menos de 4MHz
    #else
      const uint32_t _freq_display=25000000; //25MHz
    #endif
    //Características de la conexión SPI
    //Baudrate a la frecuencia definida, con orden MSBFIRST y modo SPI_MODE0 (polaridad, fase, borde)
    //  _spiSetting = SPISettings(_freq_display, MSBFIRST, SPI_MODE0);
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
  public:
    const byte xMax=84; //Anchura de display en pixels
    const byte pagesMax=6; //Altura de display de páginas
    const byte yMax=8*pagesMax; //Altura de display en pixels
    
    //La memoria de vídeo es un sprite
    RoJoSprite *videoMem;
    //Resetea el display  
    void reset();
    //Envía memoria de vídeo al display
    void show();
    //Inicialización
    void begin(byte pinRST_display,byte pinDC_display,byte pinCS_display=SS,byte pinCS_SD=SS);
    //Fija el display en modo invertido (blanco <-> negro)
    void reverse(bool reserveMode);
    //Fija el contraste
    void setContrast(byte c);
    //Destructor
    ~RoJoLCD5110();
}; //Punto y coma obligatorio para que no de error

#endif

