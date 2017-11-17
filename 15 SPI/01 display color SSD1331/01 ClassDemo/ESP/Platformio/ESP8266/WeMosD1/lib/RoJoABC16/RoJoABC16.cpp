/*
  Nombre de la librería: RoJoABC16.h
  Versión: 1.00
  Autor: Ramón Junquera
  Descripción:
    Gestión de fuentes de color (16bits)
    Sólo compatible con las familias de placas ESP32 y ESP8266
*/

/*
  Formato de un archivo de fuentes.

  Código ASCII del carácter mínimo. 1 byte. charMin. Posición: 0
  Código ASCII del carácter máximo. 1 byte. charMax. Posición: 1
  Número de páginas (bytes) de altura de los caracteres. 1 byte. pages. Posición: 2
  
  El número de caracteres que contiene la fuente será
  charCount=charMax-charMin+1

  Array de índices de inicio de datos gráficos con tantas posiciones como caracteres.
  Los índices son unsigned short int (2 bytes).
  charIndexArray = new uint16_t[charCount]
  Posición: 3

  Array de anchuras de caracteres con tantas posiciones como caracteres.
  Las anchuras son de 1 byte. 
  widthArray = new byte[charCount]
  Posición: 3+charCount*2

  Número de bytes que ocupa el array de gráficos. 2 bytes. bitmapCount
  Posición: 3+charCount*3
  No utilizaremos esta información puesto que no leemos este array en memoria
  como se hace en Raspberry

  Array de datos gráficos, con tantas posiciones como caracteres.
  Cada dato gráfico es de 1 byte.
  bitmapArray = new byte[bitmapCount]
  Posición: 5+charCount*3
*/

#include <arduino.h>
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

  //Anotamos que el número de páginas es nulo
  pages=0;
  //Con la placa Lolin32 no es necesario inicializar el sistema de archivos
}

bool RoJoABC16::load(String fileName)
{
  //Cargamos datos de un archivo de fuentes
  //Devuelve false ante cualquier error

  //Cerramos cualquier fuente abierta antes
  close();
  //Abrimos el archivo indicado del SPIFFS como sólo lectura
  f=SPIFFS.open(fileName,"r");
  //Si hubo algún problema...hemos terminado
  if(!f) return false;
  //Leemos ASCII del primer carácter de la fuente
  f.readBytes((char *)&charMin,1);
  //Leemos ASCII del último carácter de la fuente
  f.readBytes((char *)&charMax,1);
  //Leemos el número de páginas
  f.readBytes((char *)&pages,1);
  //Calculamos el número de caracteres de la fuente
  charCount=charMax-charMin+1;
  //Todo correcto
  return true;
}

void RoJoABC16::close()
{
  //Cierra el archivo actual de fuentes

  //Cerramos el archivo de fuentes (si está abierto)
  //La variable pages infica si tenemos algún archivo de fuentes abierto
  if(pages) f.close();
  //Ya no tenemos archivos de fuentes abiertos
  pages=0;
}

RoJoABC16::~RoJoABC16()
{
  //Destructor
  
  close();
}

bool RoJoABC16::charInFont(byte c)
{
  //El carácter pertenece a la fuente?
  return (c>=charMin && c<=charMax);
}

byte RoJoABC16::charWidth(byte c)
{
  //Devuelve la anchura de un carácter

  //Si el no carácter pertenece a la fuente...la anchura es cero
  if(!charInFont(c)) return 0;
  //Definimos la variable que guardará la anchura
  byte width;
  //Posicionamos el puntero de lectura del archivo
  f.seek(3+charCount*2+c-charMin,SeekSet);
  //Leemos la anchura
  f.readBytes((char *)&width,1);
  //La devolvemos
  return width;
}

unsigned short int RoJoABC16::charIndex(byte c)
{
  //Devuelve el índice de inicio de los datos gráficos de un carácter

  //Si el no carácter pertenece a la fuente...el índice es cero
  if(!charInFont(c)) return 0;
  //Definimos la variable que guardará el índice
  unsigned short int index;
  //Posicionamos el puntero de lectura del archivo
  f.seek(3+2*(c-charMin),SeekSet);
  //Leemos el índice
  f.readBytes((char *)&index,2);
  //Lo devolvemos
  return index;
}

void RoJoABC16::print(String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor)
{
  //Crea un sprite con el texto indicado
  //Se indica el color del texto y de fondo

  //Inicialmente la anchura del sprite es 0
  uint16_t spriteWidth=0;
  //Byte gráfico actualmente procesado
  byte pageByte;

  //Calculamos la anchura del sprite
  //Recorremos todos los caracteres del texto
  for(uint16_t i=0;i<text.length();i++)
  {
    //Si el carácter pertenece a la fuente...
    if(charInFont(text[i]))
    {
      //...este carácter será representado
      //Si este no es el primer carácter, añadiremos una
      //columna de separación.
      if(spriteWidth>0) spriteWidth++;
      
      //Añadimos la anchura del carácter
      spriteWidth+=charWidth(text[i]);
    }
  }
  //Tenemos calculada la anchura del sprite en spriteWidth

  //Fijamos el tamaño del sprite
  (*sprite).setSize(spriteWidth,pages*8);
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
      if(charInFont(text[i]))
      {
        //Si no es el primer carácter...dejaremos una separación
        if(x>0) x++;
        //Obtenemos las propiedades del carácter
        byte chWidth = charWidth(text[i]);
        uint16_t chIndex = charIndex(text[i]);
        //Posicionamos la lectura del archivo
        f.seek(5+charCount*3+chIndex,SeekSet);
        //Recorremos todas las páginas de altura
        for(byte p=0;p<pages;p++)
        {
          //Recorremos las columnas que forman el carácter
          for(byte c=0;c<chWidth;c++)
          {
            //Leemos el byte de la página
            f.readBytes((char *)&pageByte,1);
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
}

void RoJoABC16::print(String text,RoJoSprite16 *sprite,uint16_t textColor)
{
  //Crea un sprite con el texto indicado
  //Se indica el color del texto. EL fondo será negro
  print(text,sprite,textColor,0);
}

void RoJoABC16::print(String text,RoJoSprite16 *sprite,uint16_t textColor,uint16_t backColor,uint16_t borderColor)
{
  //Crea un sprite de texto con borde

  //Creamos el sprite que contendrá el texto sin bordes
  RoJoSprite16 textSprite;
  print(text,&textSprite,borderColor,backColor);
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
}





