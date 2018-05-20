/*
  Nombre de la librería: RoJoXPT2046.h
  Versión: 20180520
  Autor: Ramón Junquera
  Descripción:
    Gestión del touch panel XPT2046
  Mejoras futuras:
    Reducir la dispersión de coordenadas de hardware en la primera pulsación
*/

#ifndef RoJoXPT2046_h
#define RoJoXPT2046_h

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI

class RoJoXPT2046
{
  private:
    //Variable interna del pin CS
    byte _pinCS;
    //Características de la conexión SPI
    const SPISettings _spiSetting=SPISettings(2999999,MSBFIRST,SPI_MODE0); //frecuencia < 3MHz
    //Tamaño del display sin rotación
    int32_t _xMaxDisplay=320; //Anchura de display
    int32_t _yMaxDisplay=480; //Altura de display
    //Valores mínimos de coordenadas de hardware para que sean visibles
    int32_t _xMinTS=30894;
    int32_t _yMinTS=31459;
    //Valores máximos de coordenadas de hardware para que sean visibles
    int32_t _xMaxTS=1393;
    int32_t _yMaxTS=1826;
    //Nombre de archivo de configuración
    String _fileConfig="xpt2046.cfg";
    //Rotación actual
    byte _rotation=0;
  public:
    //Inicialización
    void begin(byte pinCS=SS);
    //Obtiene las coordenadas de hardware
    bool getRawXY(uint16_t *x,uint16_t *y);
    //Fija los valores de conversión
    void setConfig(uint16_t xMaxDisplay,uint16_t yMaxDisplay,uint16_t xMinTS,uint16_t yMinTS,uint16_t xMaxTS,uint16_t yMaxTS);
    //Obtiene las coordenadas de pantalla
    bool getXY(int16_t *x,int16_t *y);
    //Fija la rotación
    void rotation(byte rotationCode);
    //Salva la configuración en una archivo
    void saveConfig();
    //Recupera la configuración desde un archivo
    bool loadConfig();
}; //Punto y coma obligatorio para que no de error

#endif

