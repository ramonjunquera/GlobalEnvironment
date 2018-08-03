/*
  Nombre de la librería: RoJoABC.h
  Versión: 20180222
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes
  Notas:
    - Los drivers de ESP ya incluyen su propia librería SD.h. Si aparecen problemas por usar múltiples librerías con
      el mismo nombre, es conveniente eliminar la librería SD.h de Arduino al compilar para ESP.
*/

#ifndef RoJoABC_h
#define RoJoABC_h

//Si declaramos la siguiente línea utilizaremos una tarjeta SD como almacenamiento por defecto
//en vez de un sistema SPIFFS
#define UseSD

#include <Arduino.h>
#ifdef UseSD //Si debemos utilizar una terjeta SD...
  #define SPIFFS SD //Cuando referenciemos a SPIFFS se redireccionará a SD
  #include <SD.h> //Librería de gestión SD
  #include "RoJoSpriteSD.h"
#else //Si debemos utilizar SPIFFS
  #ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
    #error Arduino family has not SPIFFS
  #elif defined(ESP32) //Si es un ESP32...
    #include <SPIFFS.h> 
  #else //Si cualquier otra cosa (ESP8266 o RPi)...
    #include <FS.h>
  #endif
  #include "RoJoSprite.h"
#endif

class RoJoABC
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
    void _seek(uint32_t pos); //Cambia la posición de lectura del archivo _f
    const uint32_t _freq_SD=25000000; //Frecuencia SPI de la SD = 25MHz
  public:
    RoJoABC(); //Constructor
    bool print(String fileName,String text,RoJoSprite *sprite); //Crea un sprite con el texto indicado
}; //Punto y coma obligatorio para que no de error

#endif

