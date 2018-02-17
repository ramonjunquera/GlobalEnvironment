/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20180211
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

//Reserva de memoria para variables estáticas
uint32_t RoJoSprite::freq_SPI=25000000; //Frecuencia de bus SPI cuando utilizamos una SD. Por defecto 20MHz
byte RoJoSprite::pinCS_SPI=SS; //Pin CS de SPI cuando utilizamos una SD. Lo inicializamos con el pin CS por defecto

RoJoSprite::RoJoSprite()
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

void RoJoSprite::setPage(int16_t x,int16_t page,byte mask,byte color)
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

void RoJoSprite::setPixel(int16_t x,int16_t y,byte color)
{
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
      SPIFFS.begin(pinCS_SPI,SPI,freq_SPI);
    #elif defined(ESP8266)
      //Puesto que ESP8266 también tiene su propia librería SD, los parámetros de 
      //begin también son distintos
      SPIFFS.begin(pinCS_SPI,freq_SPI);
    #else
      //Si no es un ESP, es un Arduino. Usamos los parámetros de la librería SD.h estándar
      SPIFFS.begin(freq_SPI,pinCS_SPI);
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
        SPIFFS.begin(pinCS_SPI,SPI,freq_SPI);
      #elif defined(ESP8266)
        //Puesto que ESP8266 también tiene su propia librería SD, los parámetros de 
        //begin también son distintos
        SPIFFS.begin(pinCS_SPI,freq_SPI);
      #else
        //Si no es un ESP, es un Arduino. Usamos los parámetros de la librería SD.h estándar
        SPIFFS.begin(freq_SPI,pinCS_SPI);
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
      if((*source).getPixel((x*sourceSizeX)/width,(y*sourceSizeY)/height)) setPixel(x,y,1);
}

#endif
