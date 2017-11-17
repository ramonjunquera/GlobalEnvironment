/*
  Nombre de la librería: RoJoABC16.h
  Versión: 1.00
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes de color (16bits)
    Sólo compatible con las familias de placas ESP32 y ESP8266
*/

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266)
  #error Library RoJoABC16 is only compatible with ESP32 & ESP8266 family devices
#endif  

#ifndef RoJoABC16_h
#define RoJoABC16_h

#include <arduino.h>
#if defined(ESP32)
  #include <SPIFFS.h>
#else
  #include <FS.h>
#endif

class RoJoABC16
{
  private:
    File f; //Archivo de fuentes
    byte charMin; //Primer carácter ASCII incluido en librería
    byte charMax; //Último carácter ASCII incluido en librería
    byte pages; //Altura en páginas
    byte charCount; //Número de caracteres de la fuente
    bool charInFont(byte c); //El carácter pertenece a la fuente?
    byte charWidth(byte c); //Devuelve la anchura de un carácter
    unsigned short int charIndex(byte c); //Devuelve el índice de inicio de los datos gráficos de un carácter
    //Crea un sprite en base a un sprite de texto, al que añade un borde alrededor
    void Border(int16_t x,int16_t y,RoJoSprite16 *source,RoJoSprite16 *destination,uint16_t invisibleColor,uint16_t borderColor);
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

