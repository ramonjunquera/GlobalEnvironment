 /*
  Nombre de la librería: RoJoSSD1331_ESP.h
  Versión: 20171016
  Autor: Ramón Junquera
  Descripción:
    Gestión de display OLED SPI 0.95" 96x64 SSD1331 para placas ESP
    Permite la gestión de sprites
    Sólo compatible con las familias de placas ESP32 y ESP8266
    
  Pinout SPI para distintas placas:
  
            ------SPI------- --I2C--
    Modelo  SCK MISO MOSI CS SCL SDA
    ------- --- ---- ---- -- --- ---
    ESP8266 D5   D6   D7  D8 D1  D2
    ESP32   18   19   23   5 22  21

  Mejoras pendientes:
    - Vale la pena crear un doble buffer de video intermedio para acelerar la escritura en el display?. Sólo se 
      dibujarían los pixels que varían. Un buffer contendría la memoria de vídeo con el contenido que se está
      mostrando actualmente y otro sería la memoria de vídeo en de trabajo. La rutina compararía ambas y enviaría
      las diferencias. Esto supones 96x94x2= 12288 bytes = 12 Kb
*/

#ifndef RoJoSSD1331_cpp
#define RoJoSSD1331_cpp

#include <arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI por hardware
#include "RoJoSSD1331.h"
#include "RoJoSprite16.h"

void RoJoSSD1331::_writeData(uint16_t d)
{
  //Escribe un dato (color de pixel) en el display

  //Enviaremos un dato
  digitalWrite(_pinDC,HIGH);
  //Escribimos los dos bytes del dato
  SPI.transfer16(d);
}

void RoJoSSD1331::_writeCommand(byte *buffer,byte len)
{
  //Escribe un número determinado de comandos en el display
  //Se indica el puntero del buffer y el número de bytes

  //Enviaremos un comando
  digitalWrite(_pinDC,LOW);
  //Escribimos el buffer recibido con todos sus comandos
  SPI.writeBytes(buffer,len);
}

void RoJoSSD1331::reset()
{
  //Resetea el display
  digitalWrite(_pinRES,LOW);
  delay(10);
  digitalWrite(_pinRES,HIGH);
  delay(10);
}

void RoJoSSD1331::begin(byte pinDC,byte pinRES,byte pinCS)
{
  //Inicialización del display

  //Guardamos los parámetros en variables internas
  _pinDC=pinDC;
  _pinRES=pinRES;
  _pinCS=pinCS;
  
  //Siempre escribiremos en los pines DC, RES y CS
  pinMode(_pinDC,OUTPUT);
  pinMode(_pinRES,OUTPUT);
  pinMode(_pinCS,OUTPUT);

  digitalWrite(_pinRES,HIGH); //Comenzamos sin reiniciar el display
  digitalWrite(_pinDC,HIGH); //Comenzamos enviando datos
  digitalWrite(_pinCS,HIGH); //Comenzamos sin enviar datos

  //Definimos las caraterísticas de la conexión SPI
  _spiSetting = SPISettings(_spiFreq, MSBFIRST, SPI_MODE3);

  //Inicializamos las conexiones SPI
  SPI.begin();
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  SPI.setHwCs(false);

  _startSPI();
    //Reseteamos el display
    reset();
    //Secuencia de inicialización. Creamos un buffer de bytes con todos los comandos
    byte commandBuffer[]=
    {
      0xAE //Apagamos la pantalla (SSD1331_CMD_DISPLAYOFF)
  
      ,0xA0 //Fijamos orden de componentes de color (SSD1331_CMD_SETREMAP)
      ,0x72 //a RGB
  
      ,0xA1 //Fijamos como primera línea (SSD1331_CMD_STARTLINE)
      ,0x00 //la 0
  
      ,0xA2,0x00 //SSD1331_CMD_DISPLAYOFFSET
  
      ,0xA4 //SSD1331_CMD_NORMALDISPLAY
  
      ,0xA8 //SSD1331_CMD_SETMULTIPLEX
      ,0x3F // 1/64 duty
  
      ,0xAD,0x8E //SSD1331_CMD_SETMASTER
  
      ,0xB0,0x0B //SSD1331_CMD_POWERMODE
  
      ,0xB1,0x31 //SSD1331_CMD_PRECHARGE
  
      ,0xB3 //SSD1331_CMD_CLOCKDIV
      ,0xF0 // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  
      ,0x8A,0x64 //SSD1331_CMD_PRECHARGEA
    
      ,0x8B,0x78 //SSD1331_CMD_PRECHARGEB
    
      ,0x8C,0x64 //SSD1331_CMD_PRECHARGEA
    
      ,0xBB,0x3A //SSD1331_CMD_PRECHARGELEVEL
    
      ,0xBE,0x3E //SSD1331_CMD_VCOMH
    
      ,0x87,0x06 //SSD1331_CMD_MASTERCURRENT
    
      ,0x81,0x91 //SSD1331_CMD_CONTRASTA
    
      ,0x82,0x50 //SSD1331_CMD_CONTRASTB
    
      ,0x83,0x7D //SSD1331_CMD_CONTRASTC
    
      ,0xAF //Encendemos el display SSD1331_CMD_DISPLAYON
    };
  
    //Escribimos todos los comandos del buffer
    _writeCommand(commandBuffer,sizeof(commandBuffer));
  _endSPI();
  //Borramos la pantalla
  clear();
}

void RoJoSSD1331::begin(byte pinDC,byte pinRES,byte pinCS,uint32_t spiFreq)
{
  //Inicialización con parámetro de frecuencia de SPI

  //Guardamos el valor de la frecuencia de conexión SPI
  _spiFreq=spiFreq;
  //Llamamos al begin normal
  begin(pinDC,pinRES,pinCS);
}

void RoJoSSD1331::_setPixel(uint16_t color)
{
  //Dibuja un pixel del color indicado en la posición actual del cursor
  //Método privado: no gestiona transacciones SPI

  //Escribimos el color
  _writeData(color);
}

void RoJoSSD1331::setPixel(uint16_t color)
{
  //Dibuja un pixel del color indicado en la posición actual del cursor
  
  _startSPI();
    //Llamamos al método privado
    _setPixel(color);
  _endSPI();
}

void RoJoSSD1331::setPixel(int x,int y,uint16_t color)
{
  //Dibuja pixel en ciertas coordenadas

  //Si el pixel está fuera de pantalla...hemos terminado;
  if((x<0)||(y<0)||(x>=xMax)||(y>=yMax)) return;

  //Posicionamos el cursor en las coordenadas indicadas
  setCursorRangeX(x,x);
  setCursorRangeY(y,y);
   
  //Escribimos el color
  setPixel(color);
}

uint16_t RoJoSSD1331::getColor(byte r,byte g,byte b)
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

void RoJoSSD1331::_fill(bool f)
{
  //Activa/desactiva el relleno de los rectángulos
  byte commandBuffer[]={0x26,f?(byte)0x01:(byte)0x00};
  _writeCommand(commandBuffer,2);
}

void RoJoSSD1331::_rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB,byte fillR,byte fillG,byte fillB)
{
  //Dibuja un rectángulo. Función interna
  byte commandBuffer[]={0x22,x1,y1,x2,y2,borderR,borderG,borderB,fillR,fillG,fillB};
  _writeCommand(commandBuffer,11);
  //Es necesario darle un tiempo para que termine de dibujar
  delay(3);
}

void RoJoSSD1331::rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB,byte fillR,byte fillG,byte fillB)
{
  //Dibuja un rectángulo relleno

  _startSPI();
    //Nos aseguramos que los rectángulos se rellenen
    _fill(true);
    //Llamamos a la rutina interna de dibujo
    _rect(x1,y1,x2,y2,borderR,borderG,borderB,fillR,fillG,fillB);
  _endSPI();
}

void RoJoSSD1331::rect(byte x1,byte y1,byte x2,byte y2,byte borderR,byte borderG,byte borderB)
{
  //Dibuja un rectángulo sin relleno
  // b = border

  _startSPI();
    //Nos aseguramos que los rectángulos no se rellenen
    _fill(false);
    //Llamamos a la rutina interna de dibujo
    _rect(x1,y1,x2,y2,borderR,borderG,borderB,0,0,0);
  _endSPI();
}

void RoJoSSD1331::rect(byte x1,byte y1,byte x2,byte y2,uint16_t borderColor,uint16_t fillColor)
{
  //Dibuja un rectángulo relleno
  
  //Definimos las variables de componentes de color
  byte borderR,borderG,borderB,fillR,fillG,fillB;
  //Descomponemos los colores
  getComponents(borderColor,&borderR,&borderG,&borderB);
  getComponents(fillColor,&fillR,&fillG,&fillB);
  //Llamamos a la rutina de dibujo
  rect(x1,y1,x2,y2,borderR,borderG,borderB,fillR,fillG,fillB);
}

void RoJoSSD1331::rect(byte x1,byte y1,byte x2,byte y2,uint16_t borderColor)
{
  //Dibuja un rectángulo vacío
  
  //Definimos las variables de componentes de color
  byte borderR,borderG,borderB;
  //Descomponemos los colores
  getComponents(borderColor,&borderR,&borderG,&borderB);
  
  //Llamamos a la rutina de dibujo
  rect(x1,y1,x2,y2,borderR,borderG,borderB);
}

void RoJoSSD1331::copy(byte x1,byte y1,byte x2,byte y2,byte x3,byte y3)
{
  //Copia un área en otra
  //Área original definida por puntos 1 y 2
  //Área destino comienza en punto 3
  //El área destino puede sobre escribir el origen
  byte commandBuffer[]={0x23,x1,y1,x2,y2,x3,y3};
  _startSPI();
    _writeCommand(commandBuffer,7);
  _endSPI();
  //Es necesario darle un tiempo
  delay(3);
}

void RoJoSSD1331::darker(byte x1,byte y1,byte x2,byte y2)
{
  //Hace una zona más oscura
  //Una zona ya oscurecida no se oscurecerá más por volver a
  //aplicar la función
  byte commandBuffer[]={0x24,x1,y1,x2,y2};
  _startSPI();
    _writeCommand(commandBuffer,5);
  _endSPI();
  //Es necesario darle un tiempo
  delay(2);
}

void RoJoSSD1331::clear()
{
  //Pinta la pantalla de negro con un rectángulo
  rect(0,0,xMax-1,yMax-1,0,0,0,0,0,0);
}

void RoJoSSD1331::getComponents(uint16_t color,byte *r,byte *g,byte *b)
{
  //Descompone un color en sus componentes
  
  //El color lo tenemos en formato
  //  GGGBBBBB RRRRRGGG
  *r = color & 0xF8; //0xF8 = B11111000
  *b = (color >> 5) & 0xF8; //0xF8 = B11111000
  *g = ((color << 5) | (color >> 13)) & 0xFC; //0xFC = B11111100
}

void RoJoSSD1331::line(byte x1,byte y1,byte x2,byte y2,byte r,byte g,byte b)
{
  //Dibuja una línea
  byte commandBuffer[]={0x21,x1,y1,x2,y2,r,g,b};
  _startSPI();
    _writeCommand(commandBuffer,8);
  _endSPI();
  delay(1); //Necesita un momento para dibujar la línea
}

void RoJoSSD1331::line(byte x1,byte y1,byte x2,byte y2,uint16_t color)
{
  //Dibuja una línea
  
  byte R,G,B;
  getComponents(color,&R,&G,&B);
  line(x1,y1,x2,y2,R,G,B);
}

void RoJoSSD1331::drawSprite(int x,int y,RoJoSprite16 *sprite)
{
  //Dibuja un sprite en unas coordenadas
  //Sobreescribe la información existente
  //Se recibe el puntero de un array de bytes que contiene la información gráfica
  //El sprite puede tener cualquier tamaño (16bits por dimensión)

  //Obtenemos el tamaño del sprite
  byte spriteSizeY=(*sprite).height();
  byte spriteSizeX=(*sprite).width();
  //Si está fuera de pantalla...hemos terminado
  if(x>=xMax || y>=yMax || x+spriteSizeX-1<0 || y+spriteSizeY-1<0) return;
  //Definimos el rango del cursor
  setCursorRangeX(x,x+spriteSizeX-1);
  setCursorRangeY(y,y+spriteSizeY-1);
  _startSPI();
    //Recorremos todas las filas (tamaño vertical del sprite)
    for(byte spriteY=0;spriteY<spriteSizeY;spriteY++)
    {
      //Si la fila está dentro de los límites visibles...
      if(y+spriteY>=0 && y+spriteY<yMax)
      {
        //Recorremos todas las columnas del sprite
        for(byte spriteX=0;spriteX<spriteSizeX;spriteX++)
        {
          //Si la columna está dentro de los límites visibles...
          if(x+spriteX>=0 && x+spriteX<xMax)
            //...enviamos el pixel
            _setPixel((*sprite).getPixel(spriteX,spriteY));
        }
      }
    }
  _endSPI();
}

void RoJoSSD1331::setCursorRangeX(int x1,int x2)
{
  //Fija el rango de horizontal del cursor

  //Siempre x1<=x2
  if(x1>x2) _swap(&x1,&x2);
  //No permitimos que el rango salga de pantalla
  if(x1<0) x1=0;
  if(x2<0) x2=0;
  if(x1>=xMax) x1=xMax-1;
  if(x2>=xMax) x2=xMax-1;

  //Enviamos comando de fijar rango de columnas (SSD1331_CMD_SETCOLUMN)
  //desde la primera columna indicada (x1) hasta la última (x2)
  byte commandBuffer[]={0x15,(byte)x1,(byte)x2};
  _startSPI();
    _writeCommand(commandBuffer,3);
  _endSPI();
}

void RoJoSSD1331::setCursorRangeY(int y1,int y2)
{
  //Fija el rango de vertical del cursor

  //Siempre y1<=y2
  if(y1>y2) _swap(&y1,&y2);
  //No permitimos que el rango salga de pantalla
  if(y1<0) y1=0;
  if(y2<0) y2=0;
  if(y1>=yMax) y1=yMax-1;
  if(y2>=yMax) y2=yMax-1;

  //Enviamos comando de fijar rango de filas (SSD1331_CMD_SETROW)
  //desde la primera fila indicada (y1) hasta la última (y2)
  byte commandBuffer[]={0x75,(byte)y1,(byte)y2};
  _startSPI();
    _writeCommand(commandBuffer,3);
  _endSPI();
}

void RoJoSSD1331::_swap(int *a,int *b)
{
  //Intercambia los valores de dos variables enteras
  int t=*a;*a=*b;*b=t;
}
  
void RoJoSSD1331::_startSPI()
{
  //Inicia una transacción SPI
  SPI.beginTransaction(_spiSetting);
  digitalWrite(_pinCS,LOW);
}

void RoJoSSD1331::_endSPI()
{
  //Finaliza una transacción SPI
  digitalWrite(_pinCS,HIGH);
  SPI.endTransaction();
}

#endif
