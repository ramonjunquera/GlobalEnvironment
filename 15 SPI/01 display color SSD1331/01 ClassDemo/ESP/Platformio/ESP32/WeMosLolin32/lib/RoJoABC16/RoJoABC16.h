/*
  Nombre de la librería: RoJoABC16.h
  Versión: 20171026
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes de color (16bits)
    Sólo compatible con las familias de placas ESP32 y ESP8266 y
      Raspberry Pi 3
*/

#ifndef RoJoABC16_h
#define RoJoABC16_h

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266) && !defined(__arm__)
  #error Library RoJoSprite16 is only compatible with ESP32 & ESP8266 family devices or Raspberry Pi 3
#endif

#include <Arduino.h>
#if defined(ESP32)
  #include <SPIFFS.h>
#else
  #include <FS.h>
#endif
#include "RoJoSprite16.h"

class RoJoABC16
{
  private:
    File _f; //Archivo de fuentes
    byte _charMin; //Primer carácter ASCII incluido en librería
    byte _charMax; //Último carácter ASCII incluido en librería
    byte _pages; //Altura en páginas
    byte _charCount; //Número de caracteres de la fuente
    bool _charInFont(byte c); //El carácter pertenece a la fuente?
    byte _charWidth(byte c); //Devuelve la anchura de un carácter
    uint16_t _charIndex(byte c); //Devuelve el índice de inicio de los datos gráficos de un carácter
  public:
    RoJoABC16(); //Constructor
    ~RoJoABC16(); //Destructor
    bool load(String fileName); //Cargamos datos de un archivo de fuentes
    void close(); //Cierra el archivo actual de fuentes
    void print(String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor); //Crea un sprite con el texto indicado
    void print(String text,RoJoSprite16 *sprite,uint16_t textColor); //Crea un sprite con el texto indicado
    void print(String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor,uint16_t borderColor); //Crea un sprite de texto con borde
}; //Punto y coma obligatorio para que no de error

#endif

