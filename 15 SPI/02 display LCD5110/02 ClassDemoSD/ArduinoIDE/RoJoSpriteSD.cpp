/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20180222
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color blanco y negro
  Notas:
    - Los drivers de ESP ya incluyen su propia librería SD.h. Si aparecen problemas por usar múltiples librerías con
      el mismo nombre, es conveniente eliminar la librería SD.h de Arduino al compilar para ESP.
*/

#ifndef RoJoSprite_cpp
#define RoJoSprite_cpp

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

RoJoSprite::RoJoSprite(byte pinCS_SD)
{
  //Constructor

  #ifndef UseSD //Si no utilizamos una tarjeta SD...
    //Inicializamos el acceso a archivos
    //Puesto que SPIFFS no se puede desconectar, lo inicializamos al crear el
    //objeto y no nos preocupamos más de él
    //Si utilizamos un tarjeta SD, ya la inicializaremos cuando vayamos a utilizarla
    //en los métodos load & save
    SPIFFS.begin();
  #endif

  _pinCS_SD=pinCS_SD;
  
  _xMax=_pagesMax=0;
  _bitmap=NULL;
}

RoJoSprite::~RoJoSprite()
{
	//Destructor
	
	//Liberamos memoria de array gráfico
	clean();
}

uint16_t RoJoSprite::width()
{
	return _xMax;
}
	
uint16_t RoJoSprite::heightPages()
{
	return _pagesMax;
}

void RoJoSprite::_swap(int16_t *a,int16_t *b)
{
  //Intercambia los valores de dos variables enteras
  int16_t t=*a;*a=*b;*b=t;
}

void RoJoSprite::clean()
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
    //Ya no tiene dimensión
    _xMax=_pagesMax=0;
  } 
}

void RoJoSprite::setSize(uint16_t x,uint16_t pages)
{
  //Fija un nuevo tamaño para el sprite

  //Borramos el array gráfico
  clean();
  //Guardamos los valores de los parámetros
  _xMax=x;
  _pagesMax=pages;
  //Creamos un nuevo array gráfico lleno de ceros
  _bitmap = new byte[x*pages]();
}

byte RoJoSprite::getPage(uint16_t x,uint16_t page)
{
  //Devolvemos el contenido de una columnas de una página

  //Si no hay sprite o las coordenadas están fuera de rango...devolvemos 0
  if(!_bitmap || x<0 || x>=_xMax || page<0 || page>=_pagesMax) return 0;
  //Si cumple, devolvemos el byte de la página
  return _bitmap[x+page*_xMax];
}

bool RoJoSprite::getPixel(int16_t x,int16_t y)
{
  //Obtiene el estado del pixel

  //Si no hay sprite o las coordenadas están fuera de rango...devolvemos false
  if(!_bitmap || x<0 || x>=_xMax || y<0 || y>=_pagesMax*8) return false;
  //Si cumple, devolvemos el estado del pixel
  //Calculamos la página
  byte page=y/8;
  //Calculamos la máscara
  byte mask=1<<(y%8);
  return _bitmap[x+page*_xMax] & mask;
}

void RoJoSprite::_mask(uint16_t page,uint16_t x,byte mask,byte color)
{
  //Aplica una máscara al byte de una página
  //Dependiendo del color
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe

  //Calculamos la posición del byte
  uint16_t pos=x+page*_xMax;
  switch(color)
  {
    case 0: //Desactivar pixels
      _bitmap[pos]&=~mask;
      break;
    case 1: //Activar pixels
      _bitmap[pos]|=mask;
      break;
    case 2: //Invertir pixels
      _bitmap[pos]^=mask;
      break;
    case 4: //Escribe la máscara tal cual (sobreescribe)
      _bitmap[pos]=mask;
      break;
    default: //Cualquier otra opción
      break;
  }
}

void RoJoSprite::drawPage(int16_t x,int16_t page,byte mask,byte color)
{
  //Fija el valor de una columna de una página
  //Colores permitidos
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe
  
  //Si no existe el sprite o las coordenadas están fuera de rango...hemos terminamos
  if(!_bitmap || x<0 || x>=_xMax || page<0 || page>=_pagesMax) return;
  //Guardamos color
  _mask(page,x,mask,color);
}

void RoJoSprite::drawPixel(int16_t x,int16_t y,byte color)
{
  //Dibuja un pixel
  //color:
  // 0 = desactivar
  // 1 = activar
  // 2 = invertir
  // 3 = no modificar
  
  //Si no existe el sprite o las coordenadas están fuera de rango...hemos terminamos
  if(!_bitmap || x<0 || x>=_xMax || y<0 || y>=_pagesMax*8) return;
  //Dibuja pixel
  _mask(y/8,x,1<<(y%8),color);
}

bool RoJoSprite::load(String fileName)
{
  //Carga la información del sprite desde un archivo
  //Devuelve false ante cualquier error

  uint16_t width,pages;

  #ifdef UseSD //Si debemos utilizar una SD...
    //Inicializamos el uso de archivos
    //Indicamos la frecuencia y pin CS
    #ifdef ESP32
      //Puesto que ESP32 tiene su propia librería SD, los parámetros de begin
      //son distintos
      SPIFFS.begin(_pinCS_SD,SPI,_freq_SD);
    #elif defined(ESP8266)
      //Puesto que ESP8266 también tiene su propia librería SD, los parámetros de 
      //begin también son distintos
      SPIFFS.begin(_pinCS_SD,_freq_SD);
    #else
      //Si no es un ESP, es un Arduino. Usamos los parámetros de la librería SD.h estándar
      //Lo intentamos hasta conseguirlo
      while(!SPIFFS.begin(_freq_SD,_pinCS_SD));
    #endif
    //Abrimos el archivo indicado como sólo lectura
    File f=SPIFFS.open(fileName);
  #else //Si es SPIFFS...
    //Abrimos el archivo indicado como sólo lectura
    File f=SPIFFS.open(fileName,"r");
  #endif
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Leemos la anchura (2 bytes)
  f.readBytes((char *)&width,2);
  //Leemos la altura en páginas (2 bytes)
  f.readBytes((char *)&pages,2);
  //Fijamos el tamaño del sprite
  setSize(width,pages);
  //Leemos el resto del archivo y lo guardamos en el array bitmap
  f.readBytes((char *)_bitmap,width*pages);
  //Cerramos el archivo
  f.close();

  //Todo ok
  return true;
}

void RoJoSprite::clear()
{
  //Borra el sprite pintando el fondo de negro

  //Si el sprite existe...
  if(_bitmap)
  {
    //Calculamos el número de bytes del sprite
    uint32_t spriteSize=_xMax*_pagesMax;
    //Borramos todos los bytes
    for(uint32_t pos=0;pos<spriteSize;pos++) _bitmap[pos]=0;
  }
}

void RoJoSprite::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  //Dibuja una línea utilizando el algoritmo de Bresenham

  int16_t tmp; //Para swap
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
 
  int16_t dx=x2-x1;
  int16_t dy=abs(y2-y1);
 
  int16_t err=dx/2;
  const int16_t ystep=y1<y2?1:-1;
 
  for(;x1<x2;x1++)
  {
    if(steep) drawPixel(y1,x1,color);
    else drawPixel(x1,y1,color);
 
    err-=dy;
    if(err<0)
    {
      y1+=ystep;
      err+=dx;
    }
  }
}

void RoJoSprite::save(String fileName)
{
  //Guarda la información del sprite en un archivo

  uint16_t width,pages;
  width=_xMax;
  pages=_pagesMax;

  //Si hay algo que escribir...
  if(_bitmap)
  {
    #ifdef UseSD //Si debemos utilizar una SD...
      //Inicializamos el uso de archivos
      //Indicamos la frecuencia y pin CS
      #ifdef ESP32
        //Puesto que ESP32 tiene su propia librería SD, los parámetros de begin
        //son distintos
        SPIFFS.begin(_pinCS_SD,SPI,_freq_SD);
      #elif defined(ESP8266)
        //Puesto que ESP8266 también tiene su propia librería SD, los parámetros de 
        //begin también son distintos
        SPIFFS.begin(_pinCS_SD,_freq_SD);
      #else
        //Si no es un ESP, es un Arduino. Usamos los parámetros de la librería SD.h estándar
        //Lo intentamos hasta conseguirlo
        while(!SPIFFS.begin(_freq_SD,_pinCS_SD));
      #endif
      //Abrimos el archivo indicado como escritura
      File f=SPIFFS.open(fileName,FILE_WRITE);
    #else //Si es SPIFFS...
      //Abrimos el archivo indicado como escritura
      File f=SPIFFS.open(fileName,"w");
    #endif
    //Escribimos la anchura
    f.write((byte *)&width,2);
    //Escribimos el número de páginas
    f.write((byte *)&pages,2);
    //Escribimos el array de bitmap
    f.write((byte *)_bitmap,_xMax*_pagesMax);
    //Cerramos el archivo
    f.close();
  }
}

void RoJoSprite::resize(uint16_t width,uint16_t height,RoJoSprite *source)
{
  //Redimensiona un sprite
  //Ajusta automáticamente el número de páginas necesarias

  //Calculamos el número de páginas necesarias
  uint16_t pages=height/8;
  if(height%8>0) pages++;
  //Borramos el sprite actual y creamos una nuevo con el tamaño indicado
  setSize(width,pages);
  //Anotamos las dimensiones del sprite origen para facilitar la nomenclatura
  uint32_t sourceSizeX=(*source).width();
  uint32_t sourceSizeY=(*source).heightPages()*8;
  //Recorremos todas las filas
  for(uint32_t y=0;y<height;y++)
    //Recorremos todas las columnas
    for(uint32_t x=0;x<width;x++)
      //Si el pixel está activo...lo dibujamos en el redimensionado
      if((*source).getPixel((x*sourceSizeX)/width,(y*sourceSizeY)/height)) drawPixel(x,y,1);
}

void RoJoSprite::getSprite(int16_t x1,int16_t page1,int16_t x2,int16_t page2,RoJoSprite *sprite)
{
  //Crea un sprite con una sección del indicado

  //Borramos el sprite
  (*sprite).clean();
  //Siempre se debe cumplir que x1<=x2
  if(x1>x2) _swap(&x1,&x2);
  //Siempre se debe cumplir que page1<=page2
  if(page1>page2) _swap(&page1,&page2);
  //Si está fuera de pantalla...hemos terminado
  if(x1>=_xMax || x2<0 || page1>=_pagesMax || page2<0) return;
  //Recortamos las zonas no visibles
  if(x1<0) x1=0;
  if(x2>=_xMax) x2=_xMax-1;
  if(page1<0) page1=0;
  if(page2>=_pagesMax) page2=_pagesMax-1;
  //Dimensionamos el sprite
  uint16_t spriteWidth=x2-x1+1;
  uint16_t spritePages=page2-page1+1;
  (*sprite).setSize(spriteWidth,spritePages);
  //Copiamos el contenido del sprite actual al recien creado
  for(byte x=0;x<spriteWidth;x++)
    for(byte page=0;page<spritePages;page++)
      (*sprite).drawPage(x,page,getPage(x1+x,page1+page),4); //4=sobreescribiendo
}

void RoJoSprite::drawSpritePage(int16_t x,int16_t page,RoJoSprite *source,byte color)
{
  //Dibuja un sprite en una página y columna
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe

  int16_t xSprite,pageSprite;

  //Recorremos las páginas del sprite
  for(uint16_t pageCount=0;pageCount<(*source).heightPages();pageCount++)
  {
    //Calculamos la página de pantalla
    pageSprite=page+pageCount;
    //Si es visible...
    if(pageSprite>=0 && pageSprite<_pagesMax)
    {
      //Recorremos todas las columnas
      for(uint16_t xCount=0;xCount<(*source).width();xCount++)
      {
        //Calculamos la columna de pantalla
        xSprite=x+xCount;
        //Si está dentro de la pantalla...
        if(xSprite>=0 && xSprite<_xMax)
        {
          //Dibujamos el byte
          _mask(pageSprite,xSprite,(*source).getPage(xCount,pageCount),color);
        }
      }
    }
  }
}

void RoJoSprite::_lineH(int16_t y,int16_t x1,int16_t x2,byte color,bool withoutBorder)
{
  //Dibuja linea horizontal
  //Si withoutBorder es true no se dibujará el primer y último punto
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente

  //Si el color es transparente...hemos terminado
  if(color>2) return;

  //Si la fila está fuera de rango...hemos terminado
  if(y<0 || y>=_pagesMax*8) return;
  //La fila está en un rango visible
   
  //Siempre se debe cumplir que x1<=x2
  if(x1>x2) _swap(&x1,&x2);

  //Si debemos quitar los puntos extremos...
  if(withoutBorder)
  {
    x1++; x2--;
    //Si la línea no tiene puntos...hemos terminado
    if(x1>x2) return;
  }

  //Si la recta no es visible...hemos terminado
  if(x1>=_xMax || x2<0) return;
  //Toda o parte de la recta es visible

  //Recortamos la parte que no sea visible
  if(x1<0) x1=0;
  if(x2>=_xMax) x2=_xMax-1;

  //Calculamos la página de la recta
  byte page=y/8;
  //Calculamos la máscara
  byte mask=1<<(y%8);

  //Recorremos todas las columnas visibles de la recta
  for(byte x=x1;x<=x2;x++) _mask(page,x,mask,color);
}

void RoJoSprite::lineV(int16_t x,int16_t y1,int16_t y2,byte color)
{
  //Dibuja línea vertical
  //Es un método mucho más rápido que cualquier otra línea
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  
  //Si el color es transparente...hemos terminado
  if(color>2) return;

  //Si la columna está fuera de rango...hemos terminado
  if(x<0 || x>=_xMax) return;
  //La columna está en un rango visible
   
  //Siempre se debe cumplir que y1<=y2
  if(y1>y2) _swap(&y1,&y2);

  //Si la recta no es visible...hemos terminado
  if(y1>=_pagesMax*8 || y2<0) return;
  //Toda o parte de la recta es visible

  //Recortamos la parte que no sea visible
  if(y1<0) y1=0;
  if(y2>=_pagesMax*8) y2=_pagesMax*8-1;

  //Calculamos las páginas del punto inicial y final
  byte p1=y1/8;
  byte p2=y2/8;
  //Calculamos el el offset de página inicial y final
  byte o1=y1%8;
  byte o2=y2%8;

  //Supongo que el high-bit del byte de una página está arriba
  //Si no es así, el desarrollo a partir de este punto no es válido

  //Si la recta comienza y termina en la misma página...
  if(p1==p2)
  {
    //...aplicamos el cambio sólo a byte de esa página
    _mask(p1,x,(1<<(o2+1))-(1<<o1),color);
  }
  else //La recta no comienza y termina en la misma página
  {
    //Calculamos byte de página de inicio
    _mask(p1,x,256-(1<<o1),color);
    //Calculamos byte de página de fin
    _mask(p2,x,(1<<(o2+1))-1,color);
    //Rellenamos las páginas intermedias (si las hay)
    for(byte p=p1+1;p<p2;p++) _mask(p,x,255,color);
  }
}

void RoJoSprite::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,byte borderColor)
{
  //Dibuja rectángulo con borde y relleno
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente

  //Si tiene borde..
  if(borderColor<3)
  {
    //..lo dibujamos
    _lineH(y1,x1,x2,borderColor,true);
    _lineH(y2,x1,x2,borderColor,true);
    if(abs(y1-y2)>0)
    {
      lineV(x1,y1,y2,borderColor);
      lineV(x2,y1,y2,borderColor);
    }
  }
  //Si no tiene relleno..hemos terminado
  if(fillColor>2) return;
  
  //Dibujamos el relleno
  _rect(x1,y1,x2,y2,fillColor,true);
}

void RoJoSprite::lineH(int16_t y,int16_t x1,int16_t x2,byte color)
{
  //Dibuja línea horizontal
  _lineH(y,x1,x2,color,false);
}

void RoJoSprite::_rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,bool withoutBorder)
{
  //Dibuja un rectángulo relleno
  //Si withoutBorder es true, no de dibujará la línea de perímetro
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente

  //Si el color es transparente...hemos terminado
  if(fillColor>2) return;

  //Siempre se debe cumplir que y1<=y2 y x1<=x2
  if(y1>y2) _swap(&y1,&y2);
  if(x1>x2) _swap(&x1,&x2);

  //Si no debemos dibujar los bordes..
  if(withoutBorder)
  {
    //..reducimos en uno los límites del rectángulo
    x1++; x2--; y1++; y2--;
    //Si el rectángulo no tiene área...hemos terminado
    if(x1>x2 || y1>y2) return;
  }
  
  //Si el rectángulo no es visible...hemos terminado
  if(x1>=_xMax || x2<0 || y1>=_pagesMax*8 || y2<0) return;
  //Todo o parte del rectángulo es visible

  //Recortamos la parte que no sea visible
  if(x1<0) x1=0;
  if(x2>=_xMax) x2=_xMax-1;
  if(y1<0) y1=0;
  if(y2>=_pagesMax*8) y2=_pagesMax*8-1;

  //Recorremos todas las columnas y dibujamos las rectas verticales
  for(byte x=x1;x<=x2;x++) lineV(x,y1,y2,fillColor);
}

void RoJoSprite::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor)
{
  //Dibuja un rectángulo relleno
  _rect(x1,y1,x2,y2,fillColor,false);
}

void RoJoSprite::drawSprite(int16_t x,int16_t y,RoJoSprite *sprite,byte color)
{
  //Dibuja un sprite en unas coordenadas
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente

  //Si el color es transparente...hemos terminado
  if(color>2) return;

  //Recorremos todas las filas del sprite
  for(uint16_t yCount=0;yCount<(*sprite).heightPages()*8;yCount++)
  {
    //Recorremos todas las columnas del sprite
    for(uint16_t xCount=0;xCount<(*sprite).width();xCount++)
    {
      //Si el pixel está activo..lo dibujamos en el sprite indicado
      if((*sprite).getPixel(xCount,yCount)) drawPixel(x+xCount,y+yCount,color);
    }
  }
}

byte RoJoSprite::pinCS_SD()
{
  //Devuelve el pin CS de la SD
  return _pinCS_SD;
}

#endif
