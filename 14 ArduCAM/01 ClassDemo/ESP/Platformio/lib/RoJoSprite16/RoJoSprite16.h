/*
  Nombre de la librería: RoJoSprite16.h
  Versión: 20190628
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color 16bits
*/

#ifndef RoJoSprite16_h
#define RoJoSprite16_h

#include <Arduino.h>
#include <RoJoGraph.h> //Funciones gráficas avanzadas

class RoJoSprite16:public RoJoGraph
{
  private:
    uint16_t _xMax,_yMax; //Anchura y altura en pixels
    uint16_t *_bitmapMem; //Puntero a array de gráficos en memoria
    void _drawSprite(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor=0,bool invisible=false); //Dibuja un sprite sobre otro
  public:
    RoJoSprite16(); //Constructor
    void end(); //Libera el array de gráficos
    bool setSize(uint16_t x,uint16_t y); //Fija un nuevo tamaño para el sprite
    ~RoJoSprite16(); //Destructor
    uint16_t xMax(); //Anchura del sprite
    uint16_t yMax(); //Altura del sprite
    bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel
    bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno (dummy)
    uint16_t getPixel(int16_t x,int16_t y); //Devolvemos color
    bool load(String fileName); //Carga la información del sprite desde un archivo
    byte loadBMP(String fileName); //Carga la información del sprite desde un archivo .bmp
    void replaceColor(uint16_t source,uint16_t destination); //Cambia los pixels de un color por otro
    void drawSprite(int16_t x,int16_t y,RoJoSprite16 *source); //Dibuja un sprite en unas coordenadas
    void drawSprite(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor); //Dibuja un sprite en unas coordenadas
    bool print(String fileNameFon,String text,uint16_t textColor,uint16_t backColor=0); //Crea sprite de texto
    bool print(String fileNameFon,String text,uint16_t textColor,uint16_t backColor,uint16_t borderColor); //Crea un sprite de texto con borde
    void resize(uint16_t width,uint16_t height,RoJoSprite16 *source); //Redimensiona un sprite
    bool save(String fileName); //Guarda la información del sprite en un archivo
    bool copy(RoJoSprite16 *source,uint16_t x1=0,uint16_t y1=0,uint16_t x2=0,uint16_t y2=0); //Copia un sprite en otro
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoSprite16.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

