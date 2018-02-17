/*
  Nombre de la librería: RoJoABC.h
  Versión: 20180211
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes
  Notas:
    - Los drivers de ESP ya incluyen su propia librería SD.h. Si aparecen problemas por usar múltiples librerías con
      el mismo nombre, es conveniente eliminar la librería SD.h de Arduino al compilar para ESP.
*/

#ifndef RoJoABC_cpp
#define RoJoABC_cpp

//Si declaramos la siguiente línea utilizaremos una tarjeta SD como almacenamiento por defecto
//en vez de un sistema SPIFFS
#define UseSD

#include <Arduino.h>
#ifdef UseSD //Si debemos utilizar una terjeta SD...
  #define SPIFFS SD //Cuando referenciemos a SPIFFS se redireccionará a SD
  #include <SD.h> //Librería de gestión SD
  #include "RoJoSpriteSD.h"
  #include "RoJoABCSD.h"
#else //Si debemos utilizar SPIFFS
  #ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
    #error Arduino family has not SPIFFS
  #elif defined(ESP32) //Si es un ESP32...
    #include <SPIFFS.h> 
  #else //Si cualquier otra cosa (ESP8266 o RPi)...
    #include <FS.h>
  #endif
  #include "RoJoSprite.h"
  #include "RoJoABC.h"
#endif

RoJoABC::RoJoABC()
{
  //Constructor

  #ifndef UseSD //Si no utilizamos una tarjeta SD...
    //Inicializamos el acceso a archivos
    //Puesto que SPIFFS no se puede desconectar, lo inicializamos al crear el
    //objeto y no nos preocupamos más de él
    //Si utilizamos un tarjeta SD, ya la inicializaremos cuando vayamos a utilizarla
    //en el método print
    SPIFFS.begin();
  #endif
}

bool RoJoABC::_charInFont(byte c)
{
  //El carácter pertenece a la fuente?
  return (c>=_charMin && c<=_charMax);
}

void RoJoABC::_seek(uint32_t pos)
{
  //Cambia la posición de lectura del archivo _f
  #ifdef UseSD
    _f.seek(pos); //El método seek de la librería SD sólo tiene un parámetro
  #else
    _f.seek(pos,SeekSet); //El método seek de las librerías SPIFFS tienen dos parámetros
  #endif
}

byte RoJoABC::_charWidth(byte c)
{
  //Devuelve la anchura de un carácter
  //Le pasamos como parámetros el archivo de fuentes y el carácter

  //Si el no carácter pertenece a la fuente...la anchura es cero
  if(!_charInFont(c)) return 0;
  //Definimos la variable que guardará la anchura
  byte width;
  //Posicionamos el puntero de lectura del archivo
  _seek(3+_charCount*2+c-_charMin);
  //Leemos la anchura
  _f.readBytes((char *)&width,1);
  //La devolvemos
  return width;
}

uint16_t RoJoABC::_charIndex(byte c)
{
  //Devuelve el índice de inicio de los datos gráficos de un carácter
  //Le pasamos como parámetros el archivo de fuentes y el carácter

  //Si el carácter no pertenece a la fuente...el índice es cero
  if(!_charInFont(c)) return 0;
  //Definimos la variable que guardará el índice
  uint16_t index;
  //Posicionamos el puntero de lectura del archivo
  _seek(3+2*(c-_charMin));
  //Leemos el índice
  _f.readBytes((char *)&index,2);
  //Lo devolvemos
  return index;
}

bool RoJoABC::print(String fileNameFon,String text,RoJoSprite *sprite)
{
  //Crea un sprite con el texto indicado basado en una fuente
  //Devuelve false ante cualquier error

  #ifdef UseSD //Si debemos utilizar una SD...
    //Inicializamos el uso de archivos
    //Indicamos la frecuencia y pin CS obtenidos de los valores que se utilizan
    //en la clase RoJoSprite
    #ifdef ESP32
      //Puesto que ESP32 tiene su propia librería SD, los parámetros de begin
      //son distintos
      SPIFFS.begin(RoJoSprite::pinCS_SPI,SPI,RoJoSprite::freq_SPI);
    #elif defined(ESP8266)
      //Puesto que ESP8266 también tiene su propia librería SD, los parámetros de 
      //begin también son distintos
      SPIFFS.begin(RoJoSprite::pinCS_SPI,RoJoSprite::freq_SPI);
    #else
      //Si no es un ESP, es un Arduino. Usamos los parámetros de la librería SD.h estándar
      SPIFFS.begin(RoJoSprite::freq_SPI,RoJoSprite::pinCS_SPI);
    #endif
    //Abrimos el archivo indicado como sólo lectura
    _f=SPIFFS.open(fileNameFon);
  #else //Si es SPIFFS...
    //Abrimos el archivo indicado como sólo lectura
    _f=SPIFFS.open(fileNameFon,"r");
  #endif
  //Si hubo algún problema...hemos terminado
  if(!_f) return false;
  //Leemos ASCII del primer carácter de la fuente
  _f.readBytes((char *)&_charMin,1);
  //Leemos ASCII del último carácter de la fuente
  _f.readBytes((char *)&_charMax,1);
  //Leemos el número de páginas
  _f.readBytes((char *)&_pages,1);
  //Calculamos el número de caracteres de la fuente
  _charCount=_charMax-_charMin+1;

  //Inicialmente la anchura del sprite es 0
  uint16_t spriteWidth=0;
  //Byte gráfico actualmente procesado
  byte pageByte;

  //Calculamos la anchura del sprite
  //Recorremos todos los caracteres del texto
  for(uint16_t i=0;i<text.length();i++)
  {
    //Si el carácter pertenece a la fuente...
    if(_charInFont(text[i]))
    {
      //...este carácter será representado
      //Si este no es el primer carácter, añadiremos una
      //columna de separación.
      if(spriteWidth>0) spriteWidth++;
      
      //Añadimos la anchura del carácter
      spriteWidth+=_charWidth(text[i]);
    }
  }
  //Tenemos calculada la anchura del sprite en spriteWidth

  //Fijamos el tamaño del sprite
  (*sprite).setSize(spriteWidth,_pages);
  //Si el sprite tiene anchura...
  if(spriteWidth>0)
  {
    //Variable para guardar la posición x en la que estamos
    //escribiendo en el sprite
    uint16_t x=0;
    //Volvemos a recorrer todos los caracteres del string
    for(byte i=0;i<text.length();i++)
    {
      //Si el carácter está dentro del rango de la fuente...
      if(_charInFont(text[i]))
      {
        //Si no es el primer carácter...dejaremos una separación
        if(x>0) x++;
        //Obtenemos las propiedades del carácter
        byte chWidth = _charWidth(text[i]);
        uint16_t chIndex = _charIndex(text[i]);
        //Posicionamos la lectura del archivo
        _seek(5+_charCount*3+chIndex);
        //Recorremos todas las páginas de altura
        for(byte p=0;p<_pages;p++)
        {
          //Recorremos las columnas que forman el carácter
          for(byte c=0;c<chWidth;c++)
          {
            //Leemos el byte de la página
            _f.readBytes((char *)&pageByte,1);
            //Lo escribimos en el sprite
            (*sprite).setPage(x+c,p,pageByte,4);
          }
        }
        //El siguiente caracter lo comenzaremos a escribir a una
        //distacia igual a la anchura del carácter escrito
        x+=chWidth;
      }
    }
  }
  //Hemos terminado de leer el archivo de fuentes. Lo cerramos
  _f.close();
  //Todo correcto
  return true;
}

#endif

