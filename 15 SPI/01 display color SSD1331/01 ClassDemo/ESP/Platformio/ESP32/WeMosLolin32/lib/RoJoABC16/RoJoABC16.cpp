/*
  Nombre de la librería: RoJoABC16.h
  Versión: 20180124
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes de color (16bits)
    Sólo compatible con las familias de placas ESP32 y ESP8266 y
      Raspberry Pi 3
*/

#ifndef RoJoABC16_cpp
#define RoJoABC16_cpp

#include <Arduino.h>
#if defined(ESP32)
  #include <SPIFFS.h>
#else
  #include <FS.h>
#endif
#include "RoJoSprite16.h"
#include "RoJoABC16.h"

RoJoABC16::RoJoABC16()
{
  //Constructor

  //Inicializamos el acceso a archivos de SPIFFS
  SPIFFS.begin();

  //Anotamos que el número de páginas es nulo
  _pages=0;
}

bool RoJoABC16::_charInFont(byte c)
{
  //El carácter pertenece a la fuente?
  return (c>=_charMin && c<=_charMax);
}

byte RoJoABC16::_charWidth(byte c)
{
  //Devuelve la anchura de un carácter

  //Si el no carácter pertenece a la fuente...la anchura es cero
  if(!_charInFont(c)) return 0;
  //Definimos la variable que guardará la anchura
  byte width;
  //Posicionamos el puntero de lectura del archivo
  _f.seek(3+_charCount*2+c-_charMin,SeekSet);
  //Leemos la anchura
  _f.readBytes((char *)&width,1);
  //La devolvemos
  return width;
}

uint16_t RoJoABC16::_charIndex(byte c)
{
  //Devuelve el índice de inicio de los datos gráficos de un carácter

  //Si el carácter no pertenece a la fuente...el índice es cero
  if(!_charInFont(c)) return 0;
  //Definimos la variable que guardará el índice
  uint16_t index;
  //Posicionamos el puntero de lectura del archivo
  _f.seek(3+2*(c-_charMin),SeekSet);
  //Leemos el índice
  _f.readBytes((char *)&index,2);
  //Lo devolvemos
  return index;
}

bool RoJoABC16::print(String fileNameFon,String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor)
{
  //Crea un sprite con el texto indicado
  //Se indica el color del texto y de fondo
  //Devuelve false ante cualquier error

  //Abrimos el archivo indicado del SPIFFS como sólo lectura
  _f=SPIFFS.open(fileNameFon,"r");
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
  (*sprite).setSize(spriteWidth,_pages*8);
  //Fijamos el color de fondo
  (*sprite).clear(backColor);
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
        _f.seek(5+_charCount*3+chIndex,SeekSet);
        //Recorremos todas las páginas de altura
        for(byte p=0;p<_pages;p++)
        {
          //Recorremos las columnas que forman el carácter
          for(byte c=0;c<chWidth;c++)
          {
            //Leemos el byte de la página
            _f.readBytes((char *)&pageByte,1);
            //Recorremos todos sus bits
            for(byte b=0;b<8;b++)
            {
                //Si el pixel está activo...lo activamos
                if((1<<b) & pageByte) (*sprite).setPixel(x+c,p*8+b,textColor);
            }
          }
        }
        //El siguiente caracter lo comenzaremos a escribir a una distacia
        //iagual a la anchura del carácter escrito
        x+=chWidth;
      }
    }
  }
  //Hemos terminado de utilizar el archivo de fuentes
  _f.close();
  //Todo correcto
  return true;
}

bool RoJoABC16::print(String fileNameFon,String text,RoJoSprite16 *sprite,uint16_t textColor)
{
  //Crea un sprite con el texto indicado
  //Se indica el color del texto. El fondo será negro
  return print(fileNameFon,text,sprite,textColor,0);
}

bool RoJoABC16::print(String fileNameFon,String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor,uint16_t borderColor)
{
  //Crea un sprite de texto con borde
  //Devuelve false ante cualquier error

  //Creamos el sprite que contendrá el texto sin bordes
  RoJoSprite16 textSprite;
  if(!print(fileNameFon,text,&textSprite,borderColor,backColor)) return false;
  //Fijamos tamaño de sprite final
  //Será 2 pixel mayor que el del texto simple en cada dimenssión
  (*sprite).setSize(textSprite.width()+2,textSprite.height()+2);
  //Lo pintamos del color de fondo
  (*sprite).clear(backColor);

  //Copiamos el sprite de texto con el fondo transparente movido un
  //pixel en cada una de las direcciones
  (*sprite).copy(2,1,&textSprite,backColor);
  (*sprite).copy(0,1,&textSprite,backColor);
  (*sprite).copy(1,2,&textSprite,backColor);
  (*sprite).copy(1,0,&textSprite,backColor);
  //Cambiamos el color del texto del sprite por el definitivo
  textSprite.replaceColor(borderColor,textColor);
  //Copiamos el sprite de texto con el fondo transparente en
  //la posición correcta
  (*sprite).copy(1,1,&textSprite,backColor);
  //Hemos terminado de utilizar el sprite de texto simple
  textSprite.clean();
  //Todo correcto
  return true;
}

#endif

