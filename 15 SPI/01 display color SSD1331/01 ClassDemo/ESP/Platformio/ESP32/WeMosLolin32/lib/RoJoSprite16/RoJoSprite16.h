/*
  Nombre de la librería: RoJoSprite16.h
  Versión: 20171101
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color 16bits
    Sólo compatible con las familias de placas ESP32 y ESP8266 y
    Raspberry Pi 3
*/

#ifndef RoJoSprite16_h
#define RoJoSprite16_h

//Si declaramos la siguiente línea se entiende que queremos utilizar la
//versión completa. Si no, se utilizará una versión con menos
//funcionalidades que ocupa menos
#define RoJoSprite16Full

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266) && !defined(__arm__)
  #error Library RoJoSprite16 is only compatible with ESP32 & ESP8266 family devices or Raspberry Pi 3
#endif  

#include <Arduino.h>

class RoJoSprite16
{
  private:
    uint16_t *_bitmap; //Array de gráficos
    uint16_t _xMax,_yMax; //Anchura y altura en pixels
    void _copy(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor,bool invisible); //Copia un sprite sobre otro
  public:
    RoJoSprite16(); //Constructor
    ~RoJoSprite16(); //Destructor
    uint16_t width();
    uint16_t height();
    void clean(); //Libera memoria ocupada por el array de gráficos
    void setSize(uint16_t x,uint16_t y); //Fija un nuevo tamaño para el sprite
    bool load(String fileName); //Carga la información del sprite desde un archivo
    uint16_t getPixel(uint16_t x,uint16_t y); //Devolvemos color
    void copy(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor); //Copia un sprite sobre otro en unas coordenadas
    void copy(int16_t x,int16_t y,RoJoSprite16 *source); //Copia un sprite sobre otro en unas coordenadas
    void setPixel(int16_t x,int16_t y,uint16_t color); //Guardamos color
//Funciones excluidas de la versión completa    
#if defined(RoJoSprite16Full)
    void save(String fileName); //Guarda la información del sprite en un archivo
	  void clear(uint16_t color); //Borra el sprite llenando el fondo de un color
    void clear(); //Borra el sprite pintando el fondo de negro
    void clear(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno de un color
    void clear(int16_t x1,int16_t y1,int16_t x2,int16_t y2); //Dibuja un rectángulo relleno negro
    void replaceColor(uint16_t source,uint16_t destination); //Cambia los pixels de un color por otro
    void resize(uint16_t width,uint16_t height,RoJoSprite16 *source); //Redimensiona un sprite
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja una línea recta
    void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t borderColor); //Dibuja un rectángulo
#endif
}; //Punto y coma obligatorio para que no de error

#endif

