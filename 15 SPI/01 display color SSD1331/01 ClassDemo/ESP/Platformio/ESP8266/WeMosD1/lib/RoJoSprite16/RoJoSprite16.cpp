/*
  Nombre de la librería: RoJoSprite16.h
  Versión: 20171016
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color 16bits
    Sólo compatible con las familias de placas ESP32 y ESP8266

  Mejoras pendientes:
  - En los microprocesadores la memoria RAM es muy escasa. Guardar el array bitmap en el sistema de archivos.
    El constructor debe incluir como parámetro el nombre del archivo en el que se guardará el array bitmap.
    El archivo se abrirá/cerrará en cada uso para no mantener punteros File abiertos.
    Cuando se crea un sprite tendrá tamaño cero.
    El objeto mantendrá en memoria las resolución y el nombre del archivo.
    Esto permitirá crear sprites mucho más grandes sin consumo de memoria RAM.
*/

#ifndef RoJoSprite16_cpp
#define RoJoSprite16_cpp

#include <arduino.h>
#if defined(ESP32)
  #include <SPIFFS.h> 
#else
  #include <FS.h>
#endif
#include "RoJoSprite16.h"

RoJoSprite16::RoJoSprite16()
{
  //Constructor
  
  //Inicializamos el acceso a archivos de SPIFFS
  SPIFFS.begin();
  
  _xMax=_yMax=0;
  _bitmap=NULL;
}

RoJoSprite16::~RoJoSprite16()
{
  //Destructor
  
  //Liberamos memoria de array gráfico
  clean();
}

uint16_t RoJoSprite16::width()
{
  return _xMax;
}
  
uint16_t RoJoSprite16::height()
{
  return _yMax;
}

void RoJoSprite16::clean()
{
  //Libera memoria ocupada por el array de gráficos
  
  //Si el puntero del array de gráficos está apuntando a una
  //posición de memoria dintinta de cero...
  //Si tenemos array gráfico...
  if(_bitmap)
  {
    //Liberamos su memoria
    delete[] _bitmap;
    //Al liberar la memoria a la que apunta un puntero no cambia
    //el puntero. Lo cambiaremos manualmente
    _bitmap=NULL;
  } 
}


void RoJoSprite16::setSize(uint16_t x,uint16_t y)
{
  //Fija un nuevo tamaño para el sprite
  
  //Guardamos los valores de los parámetros
  _xMax=x;
  _yMax=y;
  //Borramos el array gráfico
  clean();
  //Creamos un nuevo array gráfico
  _bitmap = new uint16_t[x*y];
}

bool RoJoSprite16::load(String fileName)
{
  //Carga la información del sprite desde un archivo
  //Devuelve false ante cualquier error

  uint16_t width,height;

  //Abrimos el archivo indicado del SPIFFS como sólo lectura
  File f=SPIFFS.open(fileName,"r");
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Leemos la anchura (2 bytes)
  f.readBytes((char *)&width,2);
  //Leemos la altura (2 bytes)
  f.readBytes((char *)&height,2);
  //Fijamos el tamaño del sprite
  setSize(width,height);
  //Leemos el resto del archivo y lo guardamos en el array bitmap
  f.readBytes((char *)_bitmap,width*height*2);
  //Cerramos el archivo
  f.close();
  
  //Todo ok
  return true;
}

uint16_t RoJoSprite16::getPixel(uint16_t x,uint16_t y)
{
  //Devolvemos color
  return _bitmap[x+y*_xMax];
}

void RoJoSprite16::_copy(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor,bool invisible)
{
  //Copia un sprite sobre otro en unas coordenadas
  //Se indica qué color se debe tomar como invisible y si se debe tener en cuenta
  //Método privado

  //Tomanos nota del tamaño del sprite a dibujar
  uint16_t sourceSizeX=(*source).width();
  uint16_t sourceSizeY=(*source).height();

  int16_t destinationX; //Columna a escribir
  int16_t destinationY; //Fila a escribir
  uint16_t pixelColor; //Color del pixel procesado
  
  //Recorremos todas las filas del sprite a dibujar
  for(uint16_t sourceY=0;sourceY<sourceSizeY;sourceY++)
  {
    //Calculamos la fila en la que se copiará
    destinationY=y+sourceY;
    //Si la fila está dentro de rango...
    if(destinationY>=0 && destinationY<_yMax)
    {
      //...la dibujamos
      
      //Recorremos todas las columnas del sprite a dibujar
      for(uint16_t sourceX=0;sourceX<sourceSizeX;sourceX++)
      {
        //Calculamos la columna en la que se copiará
        destinationX=x+sourceX;
        //Si la columna está dentro de rango...
        if(destinationX>=0 && destinationX<_xMax)
        {
          //Obtenemos el color del pixel del sprite origen
          pixelColor=(*source).getPixel(sourceX,sourceY);
          //Si no hay que tener en cuenta un color invisible o el color del pixel
          //es distinto al del invisible...
          if(!invisible || pixelColor!=invisibleColor)
          {
            //...podemos copiar el pixel
            setPixel(destinationX,destinationY,pixelColor);
          }
        }
      }
    }
  }  
}

void RoJoSprite16::copy(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor)
{
  //Copia un sprite sobre otro en unas coordenadas
  //Se indica qué color se debe tomar como invisible
  _copy(x,y,source,invisibleColor,true);
}

void RoJoSprite16::copy(short int x,short int y,RoJoSprite16 *source)
{
  //Copia un sprite sobre otro en unas coordenadas
  _copy(x,y,source,0,false);
}

void RoJoSprite16::setPixel(int16_t x,int16_t y,uint16_t color)
{
  //Si las coordenadas están fuera de rango...terminamos
  if(x<0 || x>=_xMax || y<0 || y>=_yMax) return;
  //Guardamos color
  _bitmap[x+y*_xMax]=color;
}

//Funciones excluidas de la versión completa
#if defined(RoJoSprite16Full)

void RoJoSprite16::save(String fileName)
{
  //Guarda la información del sprite en un archivo

  unsigned short int width,height;
  width=_xMax;
  height=_yMax;

  //Si hay algo que escribir...
  if(_bitmap)
  {
    File f=SPIFFS.open(fileName,"w");
    //Escribimos la anchura
    f.write((byte *)&width,2);
    //Escribimos la altura
    f.write((byte *)&height,2);
    //Escribimos el array de bitmap
    f.write((byte *)_bitmap,_xMax*_yMax*2);
    //Cerramos el archivo
    f.close();
  }
}

void RoJoSprite16::clear(uint16_t color)
{
  //Borra el sprite llenando el fondo de un color

  //Calculamos el tamaño del array gráfico
  uint32_t bitmapSize = _xMax*_yMax;
  //Recorremos todos los bytes del array gráfico...escribiendo el color en todos los pixels
  for(uint32_t i=0;i<bitmapSize;i++) _bitmap[i]=color;
}

void RoJoSprite16::clear()
{
  //Borra el sprite pintando el fondo de negro
  clear(0);
}

void RoJoSprite16::replaceColor(uint16_t source,uint16_t destination)
{
  //Cambia los pixels de un color por otro

  //Calculamos el tamaño del array gráfico
  uint32_t bitmapSize = _xMax*_yMax;
  //Recorremos todos los bytes del array gráfico...
  for(uint32_t i=0;i<bitmapSize;i++)
  {
    //Si el color del pixel es el que buscamos...lo reemplazamos
    if(_bitmap[i]==source) _bitmap[i]=destination;
  }
}

void RoJoSprite16::resize(int16_t width,int16_t height,RoJoSprite16 *source)
{
  //Redimensiona un sprite
 
  //Borramos el sprite actual y creamos una nuevo con el tamaño indicado
  setSize(width,height);
  //Anotamos las dimensiones del sprite origen para facilitar la nomenclatura
  uint32_t sourceSizeX=(*source).width();
  uint32_t sourceSizeY=(*source).height();

  //Recorremos todas las filas
  for(uint32_t y=0;y<height;y++)
  {
    //Recorremos todas las columnas
    for(uint32_t x=0;x<width;x++)
    {
      //Calculamos la posición del pixel a tomar desde el origen y lo escribimos en el destino
      _bitmap[y*width+x]=(*source).getPixel((x*sourceSizeX)/_xMax,(y*sourceSizeY)/_yMax);
    }
  }
}

void RoJoSprite16::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  //Dibuja una línea utilizando el algoritmo de Bresenham

  int tmp; //Para swap
  const bool steep=abs(y2-y1)>abs(x2-x1);
  
  if(steep)
  {
    tmp=x1;x1=y1;y1=tmp;
    tmp=x2;x2=y2;y2=tmp;
  }
 
  if(x1>x2)
  {
    tmp=x1;x1=x2;x2=tmp;
    tmp=y1;y1=y2;y2=tmp;
  }
 
  int dx=x2-x1;
  int dy=abs(y2-y1);
 
  int err=dx/2;
  const int ystep=y1<y2?1:-1;
 
  for(;x1<x2;x1++)
  {
    if(steep) setPixel(y1,x1,color);
    else setPixel(x1,y1,color);
 
    err-=dy;
    if(err<0)
    {
      y1+=ystep;
      err+=dx;
    }
  }
}

void RoJoSprite16::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t borderColor)
{
  //Dibuja un rectángulo
  line(x1,y1,x2,y1,borderColor);
  line(x1,y2,x2,y2,borderColor);
  line(x1,y1,x1,y2,borderColor);
  line(x2,y1,x2,y2,borderColor);
}

void RoJoSprite16::clear(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  //Dibuja un rectángulo relleno de un color

  //Recorremos todas las filas y columnas pintando del color indicado
  for(int16_t x=x1;x<=x2;x++)
    for(int16_t y=y1;y<=y2;y++)
      setPixel(x,y,color);
}

void RoJoSprite16::clear(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
  //Dibuja un rectángulo relleno de color negro
  clear(x1,y1,x2,y2,0);
}

#endif //Full version

#endif
