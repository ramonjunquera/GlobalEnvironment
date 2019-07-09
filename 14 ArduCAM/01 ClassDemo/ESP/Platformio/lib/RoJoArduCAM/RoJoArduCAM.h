/*
  Nombre de la librería: RoJoArduCAM.h
  Versión: 20190709
  Autor: Ramón Junquera
  Descripción:
    Gestión de cámara ArduCAM-Mini-2MP para placas ESP
*/    

#ifndef RoJoArduCAM_h
#define RoJoArduCAM_h

#include <Arduino.h>
#include <RoJoSprite16.h> //Gestión de sprites color
#include <Wire.h> //Gestión I2C
#include <SPI.h> //Gestión SPI

class RoJoArduCAM
{
  private:
    byte _csPin; //Pin para CS
    bool _started=false; //Se ha inicializado correctamente la cámara?
    bool _currentIsJPG=true; //El formato actual es JPG (o BMP)?
    bool _bufferIsJPG=true; //El formato de la foto en la memoria de la cámara es JPG (o BMP)?
    byte _currentResId; //Código de resolución definida actualmente
    byte _bufferResId; //Código de resolución de la imagen contenida en memoria de cámara
    const SPISettings _spiSetting0 = SPISettings(8000000, MSBFIRST, SPI_MODE0); //SPI settings
    const SPISettings _spiSetting3 = SPISettings(8000000, MSBFIRST, SPI_MODE3); //SPI settings
    const byte _I2Cid=0x30; //Identificador I2C de la cámara
    bool _photoInCamera=false; //Hay alguna foto en la memoria de la cámara?
    void _writeRegFile(byte regIndex,File *f); //Escribe una colección de registros de RoJoArduCAM.fat en un archivo
    uint32_t _bufferSize(); //Lee el tamaño de la imagen de la memoria de la cámara
    void _writeRegSPI(byte reg,byte value); //Escribe un registro por SPI
    byte _readRegSPI0(byte reg); //Lee un registro por SPI en modo SPI_MODE0
    byte _readRegSPI3(byte reg); //Lee un registro por SPI en modo SPI_MODE3
    void _writeRegI2C(byte reg,byte value); //Escribe un registro por I2C
    byte _readRegI2C(byte reg); //Lee un registro de I2C
    void _writeRegI2C(byte regIndex); //Envía por I2C un colección de registros de RoJoArduCAM.dat
    const uint16_t _resX[9]={160,176,320,352,640,800,1024,1280,1600}; //Anchuras de las distintas resoluciones
    const uint16_t _resY[9]={120,144,240,288,480,600, 768,1024,1200}; //Alturas de las distintas resoluciones
    byte _pinSCL; //Pin utilizado para SCL en I2C
	public:
		byte begin(byte csPin,byte pinSDA=255,byte pinSCL=255); //Inicialización de la cámara
    void takePhoto(); //Hace una foto y la guarda en la memoria de la cámara
    bool photoIsReady(); //Hay alguna foto en la memoria de la cámara para descargar?
    void setResolution(byte resId); //Fija una resolución
    void setFormatJPG(); //Fija formato jpg
    void setFormatBMP(); //Fija formato bmp
    byte savePhoto(String filename,uint32_t *resX,uint32_t *resY); //Guarda la foto capturada en una archivo
    byte savePhoto(uint32_t resX,uint32_t resY,RoJoSprite16 *sprite); //Guarda la foto capturada en un sprite
    void setLight(byte lightId); //Fija un tipo de iluminación
    void setSaturation(int8_t s); //Fija la saturación de color
    void setBrightness(int8_t b); //Fija el nivel de brillo
    void setContrast(int8_t c); //Fija el nivel de contraste
    void setEffect(byte effectId); //Fija un efecto especial
};

#ifdef __arm__
  #include <RoJoArduCAM.cpp> //Para guardar compatibilidad con RPi
#endif

#endif
