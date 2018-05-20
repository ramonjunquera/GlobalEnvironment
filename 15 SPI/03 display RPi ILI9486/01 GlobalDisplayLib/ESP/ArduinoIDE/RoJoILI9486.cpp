/*
  Nombre de la librería: RoJoILI9486.h
  Versión: 20180519
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RPI_ILI9486 SPI 3.5" 320x480
    Permite la gestión de sprites
*/

#ifndef RoJoILI9486_cpp
#define RoJoILI9486_cpp

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI
#include "RoJoILI9486.h" //Gestión del display
#include "RoJoSprite16.h" //Gestión de sprites color


#ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
  #error Arduino family has not SPIFFS
#elif defined(ESP32) //Si es un ESP32...
  #include <SPIFFS.h> 
#else //Si cualquier otra cosa (ESP8266 o RPi)...
  #include <FS.h>
#endif

void RoJoILI9486::_startSPI()
{
  //Inicia una transacción SPI
  SPI.beginTransaction(_spiSetting);
}

void RoJoILI9486::_endSPI()
{
  //Finaliza una transacción SPI
  SPI.endTransaction();
}

void RoJoILI9486::_writeCommand(byte command)
{
  //Escribe un comando en el display

  digitalWrite(_pinDC_display,LOW); //Enviaremos un comando
  _changeCS(false); //Seleccionamos el display
  SPI.transfer(command); //Enviamos el comando
  _changeCS(true); //Deseleccionamos el display
  digitalWrite(_pinDC_display,HIGH); //Lo siguiente a enviar será un dato
}

void RoJoILI9486::_writeData(byte d)
{
  //Escribe un dato de 8bits en el display

  _changeCS(false); //Seleccionamos el display
  SPI.transfer(d); //Enviamos el dato
  _changeCS(true); //Deseleccionamos el display
}

void RoJoILI9486::_swap(int16_t *a,int16_t *b)
{
  //Intercambia los valores de dos variables enteras
  int t=*a;*a=*b;*b=t;
}

void RoJoILI9486::softReset()
{
  _startSPI();
    _writeCommand(0x01); //Comando soft reset
    delay(5);
  _endSPI();

  //Comenzamos una transacción
  _startSPI();
    _writeCommand(0x3A); //Selecciona la profundidad de color de pixel (Interface Pixel Format)
    _writeData(0x55); // 01010101 = 0101 (RGB 16 bits/pixel) + 0101 (CPU 16 bits/pixel). Sólo usamos el formato de 16bits, no de 18.

    _writeCommand(0xC2); //Selecciona la frecuencia de los circuitos de setup (Power Control 3)
    _writeData(0x44); // 01000100 = 0100 (setup cycle for circuits 1/4/5 = 2H) + 0100 (setup cycle for circuits 2/3 = 8H)

    _writeCommand(0xC5); //(VCOM Control)
    _writeData(0x00); //NV memory is not programmed
    _writeData(0x00); //VCM_REG = -2
    _writeData(0x00); //VCOM value from NV memory
    _writeData(0x00); //VCM_OUT = -2

    _writeCommand(0xE0); //Set the gray scale voltage to adjust the gamma characteristics of the TFT panel (PGAMCTRL: Positive Gamma Control)
    _writeData(0x0F); //VP0
    _writeData(0x1F); //VP1
    _writeData(0x1C); //VP2
    _writeData(0x0C); //VP4
    _writeData(0x0F); //VP6
    _writeData(0x08); //VP13
    _writeData(0x48); //VP20
    _writeData(0x98); //VP36 & VP27
    _writeData(0x37); //VP43
    _writeData(0x0A); //VP50
    _writeData(0x13); //VP57
    _writeData(0x04); //VP59
    _writeData(0x11); //VP61
    _writeData(0x0D); //VP62
    _writeData(0x00); //VP63

    _writeCommand(0xE1); //Set the gray scale voltage to adjust the gamma characteristics of the TFT panel (NGAMCTRL: Negative Gamma Correction)
    _writeData(0x0F); //VN0
    _writeData(0x32); //VN1
    _writeData(0x2E); //VN2
    _writeData(0x0B); //VN4
    _writeData(0x0D); //RVN6
    _writeData(0x05); //VN13
    _writeData(0x47); //VN20
    _writeData(0x75); //VN36 & VN27
    _writeData(0x37); //VN43
    _writeData(0x06); //VN50
    _writeData(0x10); //VN57
    _writeData(0x03); //VN59
    _writeData(0x24); //VN61
    _writeData(0x20); //VN62
    _writeData(0x00); //VN63

    _writeCommand(0x20); //(Display Inversion OFF)
  _endSPI();

  //Rotación 0: 320x480. Vertical. Conector arriba dcha
  rotation(0);
  //Encendemos el display
  on();
  //Salimos del modo de bajo consumo
  sleepOut();
}

void RoJoILI9486::hardReset()
{
  //Hace un hard reset
  
  //Reseteo de hardware
  digitalWrite(_pinRES_display,LOW);
  delay(20);
  digitalWrite(_pinRES_display,HIGH);
  delay(150);
  //Siempre despues de un hard reset se debe hacer un soft reset
  softReset();
}

void RoJoILI9486::sleepOut()
{
  //Sale del modo de bajo consumo

  _startSPI();
    _writeCommand(0x11); //Sleep OUT
    delay(120);
  _endSPI();
}

void RoJoILI9486::sleepIn()
{
  //Entra en modo de bajo consumo

  //En este modo se desactiva la pantalla, pero se conserva la memoria de vídeo
  //y sigue adminitiendo nuevos comandos.

  _startSPI();
    _writeCommand(0x10); //Sleep IN
    delay(5);
  _endSPI();
}

void RoJoILI9486::on()
{
  //Activa el display

  _startSPI();
    _writeCommand(0x29); //Display ON
  _endSPI();
}

void RoJoILI9486::off()
{
  //Activa el display

  _startSPI();
    _writeCommand(0x28); //Display OFF
  _endSPI();
}

void RoJoILI9486::begin(byte pinRES,byte pinDC, byte pinCS)
{
  //Inicialización del display

  //Inicializamos las conexiones SPI
  SPI.begin();
  
  //Guardamos los parámetros en variables internas
  _pinRES_display=pinRES;
  _pinDC_display=pinDC;
  _pinCS_display=pinCS;
  
  //Siempre escribiremos en los pines DC, RES y CS
  pinMode(_pinDC_display,OUTPUT);
  pinMode(_pinRES_display,OUTPUT);
  pinMode(_pinCS_display,OUTPUT);

  digitalWrite(_pinRES_display,HIGH); //Comenzamos sin reiniciar el display
  digitalWrite(_pinDC_display,HIGH); //Comenzamos enviando datos
  digitalWrite(_pinCS_display,HIGH); //Comenzamos sin seleccionar el chip
  
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  SPI.setHwCs(false);

  //Inicializamos el acceso al sistema de archivos
  SPIFFS.begin();

  hardReset();
}

void RoJoILI9486::_setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
  //Fija el rango de horizontal y vertical del cursor
  //Funcion interna. Sin gestión de SPI
  //No comprobamos coherencia de parámetros

  _writeCommand(0x2A); //0x2A=Column Address Set
  _writeData((byte)(x1>>8));
  _writeData((byte)(x1 & 0xFF));
  _writeData((byte)(x2>>8));
  _writeData((byte)(x2 & 0xFF));
  _writeCommand(0x2B); //0x2B=Page Address Set
  _writeData((byte)(y1>>8));
  _writeData((byte)(y1 & 0xFF));
  _writeData((byte)(y2>>8));
  _writeData((byte)(y2 & 0xFF));
}

void RoJoILI9486::drawPixel(int16_t x,int16_t y,uint16_t color)
{
  //Dibuja pixel en ciertas coordenadas
  //Realmente dibuja un rectángulo de 1 pixel
  
  rect(x,y,1,1,color);
}

uint16_t RoJoILI9486::getColor(byte r,byte g,byte b)
{
  //Obtiene el código de color en base a sus componentes
    
  //La profundidad de color de cada canal es:
    
  //  Canal : color : bits
  //  -----   -----   ----
  //    R     red       5
  //    G     green     6
  //    B     blue      5
  //La composición final de los 16 bits quedará así:
  // RRRRRGGG GGGBBBBB
    
  uint16_t c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;
  return c;
}

void RoJoILI9486::clear()
{
  //Pinta la pantalla de negro
  //Dibujamos un rectángulo negro que ocupa toda la pantalla

  rect(0,0,xMax(),yMax(),0);
}

void RoJoILI9486::getComponents(uint16_t color,byte *r,byte *g,byte *b)
{
  //Descompone un color en sus componentes
  
  //El color lo tenemos en formato
  // RRRRRGGG GGGBBBBB
  *r = color >> 11;
  *g = (color >> 5) & 0b111111;
  *b = color & 0b11111;
}

void RoJoILI9486::rect(int16_t x,int16_t y,uint16_t xLength,uint16_t yLength,uint16_t fillColor)
{
  //Dibuja un rectángulo relleno
  //Parámetros:
  //x,y coordenadas de inicio
  //xLangth,yLength tamaño
  //fillColor color de relleno

  //Calculamos el área visible
  struct displayRange r;
  visibleRange(x,y,xLength,yLength,&r);
  //Si no hay área visible...hemos terminado
  if(!r.visible) return;

  _startSPI();
    //Definimos el rango del cursor
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    _writeCommand(0x2C); //Write memory
    _changeCS(false); //Seleccionamos el display
    //Recorremos todas las filas visibles
    for(int16_t y0=r.y1;y0<=r.y2;y0++)
    {
      //Recorremos todas las columnas visibles y enviamos el color
      for(int16_t x0=r.x1;x0<=r.x2;x0++) SPI.transfer16(fillColor);
      //Refrescamos watchdog
      yield();
    }
    _changeCS(true); //Deseleccionamos el display
  _endSPI();
}

void RoJoILI9486::drawSprite(int16_t x,int16_t y,RoJoSprite16 *sprite)
{
  //Dibuja un sprite en unas coordenadas
  //Sobreescribe la información existente

  //Obtenemos el tamaño del sprite
  uint16_t xLength=(*sprite).width();
  uint16_t yLength=(*sprite).height();
  //Calculamos el área visible
  struct displayRange r;
  visibleRange(x,y,xLength,yLength,&r);
  //Si no hay área visible...hemos terminado
  if(!r.visible) return;

  _startSPI();
    //Definimos el rango del cursor
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    _writeCommand(0x2C); //Write memory
    _changeCS(false); //Seleccionamos el display
    //Recorremos todas las filas visibles del sprite
    for(int16_t y0=r.y1-y;y0<=r.y2-y;y0++)
    {
      //Recorremos todas las columnas visibles del sprite y enviamos el color del pixel
      for(int16_t x0=r.x1-x;x0<=r.x2-x;x0++) SPI.transfer16((*sprite).getPixel(x0,y0));
      //Refrescamos WatchDog
      yield();
    }
    _changeCS(true); //Deseleccionamos el display
  _endSPI();
}

bool RoJoILI9486::drawSprite(int16_t x,int16_t y,String fileName)
{
  //Dibuja un sprite en unas coordenadas directamente de un archivo
  //Devuelve false ante cualquier error

  //Abrimos el archivo indicado como sólo lectura
  File f=SPIFFS.open(fileName,"r");
  //Si hubo algún problema...devolvemos error
  if(!f) return false;

  //Tamaño del sprite
  uint16_t xLength,yLength;
  
  //Leemos la anchura (2 bytes)
  f.readBytes((char *)&xLength,2);
  //Leemos la altura (2 bytes)
  f.readBytes((char *)&yLength,2);

  //Estructura para guardar el rango de pantalla que ocupará el sprite
  struct displayRange r;
  //Lo calculamos
  visibleRange(x,y,xLength,yLength,&r);
  //Si tiene área visible...
  if(r.visible)
  {
    _startSPI();
      //Fijamos el área donde dibujaremos el sprite en pantalla
      _setCursorRange(r.x1,r.y1,r.x2,r.y2);
      //Convertimos las coordenadas de pantalla a coordenadas de sprite
      r.x1-=x; r.x2-=x; r.y1-=y; r.y2-=y;
      _writeCommand(0x2C); //Write memory
      //Color del pixel
      uint16_t color;
      _changeCS(false); //Seleccionamos el display
      //Recorremos todas las filas del sprite
      for(int16_t y0=0;y0<yLength;y0++)
      {
        //Recorremos todas las columnas del sprite
        for(int16_t x0=0;x0<xLength;x0++)
        {
          //Leemos el color del pixel
          f.readBytes((char *)&color,2);
          //Si el pixel es visible...lo dibujamos
          if(x0>=r.x1 && x0<=r.x2 && y0>=r.y1 && y0<=r.y2) SPI.transfer16(color);
        }
        //Refrescamos WatchDog
        yield();
      }
      _changeCS(true); //Deseleccionamos el display
    _endSPI();
  }
  //Hemos terminado con el archivo
  f.close();
  //Todo ok
  return true;
}

void RoJoILI9486::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
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
  yield();
}

byte RoJoILI9486::_charWidth(byte c)
{
  //Devuelve la anchura de un carácter

  //Definimos la variable que guardará la anchura
  byte width;
  //Posicionamos el puntero de lectura del archivo
  _f.seek(3+_charCount*2+c-_charMin,SeekSet);
  //Leemos la anchura
  _f.readBytes((char *)&width,1);
  //La devolvemos
  return width;
}

uint16_t RoJoILI9486::_charIndex(byte c)
{
  //Devuelve el índice de inicio de los datos gráficos de un carácter

  //Definimos la variable que guardará el índice
  uint16_t index;
  //Posicionamos el puntero de lectura del archivo
  _f.seek(3+2*(c-_charMin),SeekSet);
  //Leemos el índice
  _f.readBytes((char *)&index,2);
  //Lo devolvemos
  return index;
}

bool RoJoILI9486::print(String fileNameFon,String text,uint16_t textColor,int16_t x,int16_t y)
{
  //Dibuja un texto en una coordenadas, basado en una fuente.
  //Devuelve false ante cualquier error

  //Abrimos el archivo indicado como sólo lectura
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
  //Byte gráfico actualmente procesado
  byte pageByte;

  //Recorremos todos los caracteres del string
  for(byte i=0;i<text.length();i++)
  {
    //Tomamos nota del carácter procesado
    byte ch=text[i];
    //Si el carácter está dentro del rango de la fuente...
    if(ch>=_charMin && ch<=_charMax)
    {
      //Si no es el primer carácter...dejaremos una separación
      if(i>0) x++;
      //Obtenemos las propiedades del carácter
      byte chWidth = _charWidth(ch);
      uint16_t chIndex = _charIndex(ch);
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
              if((1<<b) & pageByte) drawPixel(x+c,y+p*8+b,textColor);
          }
        }
      }
      //El siguiente caracter lo comenzaremos a escribir a una distacia
      //igual a la anchura del carácter escrito
      x+=chWidth;
    }
  }
  //Hemos terminado de utilizar el archivo de fuentes
  _f.close();
  //Todo correcto
  return true;
}

uint16_t RoJoILI9486::xMax()
{
  //Devuelve la anchura
  return _xMax;
}

uint16_t RoJoILI9486::yMax()
{
  //Devuelve la altura
  return _yMax;
}

void RoJoILI9486::rotation(byte r)
{
  //Configura la rotación

  // r : posición conector : orientación : código
  //---  -----------------   -----------   -------
  // 0 : arriba dcha         vertical      0b01001000 = BGR(00001000) + MX(01000000)
  // 1 : arriba izda         apaisado      0b00101000 = BGR(00001000) + MV(00100000)
  // 2 : abajo izda          vertical      0b10001000 = BGR(00001000) + MY(10000000)
  // 3 : abajo dcha          apaisado      0b11101000 = BGR(00001000) + MV(00100000) + MX(01000000) + MY(10000000)
  
  //Nos aseguramos que es un valor permitido
  r%=4;
  
  _startSPI();
    _writeCommand(0x36);  //Memory access control
    _writeData(_rotationCodes[r]); //Código correspondiente a la rotación
    //Si la rotación es impar
    if(r%2)
    {
      //El formato será apaisado
      _xMax=_yMaxDefault;
      _yMax=_xMaxDefault;
    }
    else
    {
      //El formato será vertical
      _xMax=_xMaxDefault;
      _yMax=_yMaxDefault;
    }
  _endSPI();
}

void RoJoILI9486::visibleRange(int16_t x,int16_t y, int16_t xLength, int16_t yLength, displayRange *answer)
{
  //Calcula el rango visible de un área
  //Parámetros:
  //x,y coordenadas de inicial del área
  //xLength,yLength tamaño del área
  //*answer puntero a estructura con datos de respuesta
  //Utiliza internamente los valores xMax,yMax de tamaño de pantalla

  //Si está fuera de pantalla...no será visible
  if(x>=xMax() || y>=yMax() || x+xLength-1<0 || y+yLength-1<0)
    (*answer).visible=false;
  else //El área es total o parcialmente visible
  {
    (*answer).visible=true;
    //Calculamos la zona de pantalla que ocupará
    (*answer).x1=x; if(x<0) (*answer).x1=0;
    (*answer).y1=y; if(y<0) (*answer).y1=0;
    (*answer).x2=x+xLength-1; if((*answer).x2>=xMax()) (*answer).x2=xMax()-1;
    (*answer).y2=y+yLength-1; if((*answer).y2>=yMax()) (*answer).y2=yMax()-1;
  }
}

void RoJoILI9486::_changeCS(bool status)
{
  //Cambia estado del pin CS
  
  //Esta función se crea porque en procesadores rápidos, el display no
  //detecta a tiempo el cambio del estado del pin CS. Necesita un
  //instante.
  //Para consumir un el tiempo necesario, volvemos a aplicar el mismo
  //al pin CS.
  //En procesadores más lentos como los ESP8266, aun a máxima frecuencia
  //de reloj (160MHz) es suficiente con el retraso que supone llamar a
  //la función.
  
  digitalWrite(_pinCS_display,status);
  
  #ifdef __arm__ //Con RPi 
    digitalWrite(_pinCS_display,status);
  #elif defined(ESP32) //Con ESP32
    digitalWrite(_pinCS_display,status);
    digitalWrite(_pinCS_display,status);
    digitalWrite(_pinCS_display,status);
  #endif
}

#endif
