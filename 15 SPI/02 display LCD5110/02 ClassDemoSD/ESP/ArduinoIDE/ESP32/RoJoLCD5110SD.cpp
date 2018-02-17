/*
  Nombre de la librería: RoJoLCD5110.h
  Versión: 20180213
  Autor: Ramón Junquera
  Descripción:
    Gestión de display Nokia LCD 5110 SPI 84x48
 
  La siguiente librería permite la gestión del display a través de los
  pines del bus SPI controlados por hardware.

  Descripción de pines del display:
  LCD5110  Descripción
  -------  -----------
     RST    Reset
     CE     Chip select (SPI_CS)
     DC     Data/Command
     DIN    Escritura (SPI_MOSI)
     CLK    Reloj (SPI_CLK)
     VCC    3V3
     LIGHT  Luz de fondo
     GND    GND

  Pin RST
  Debe mantenerse siempre en HIGH.
  Si lo cambiamos a LOW se resetea.

  Pin CE
  Se conecta al pin de chip control que se defina.
   
  Pin DC
  No pertenece al estándar SPI. Es un pin digital se salida. Sirve
  cualquiera. Permite indicarle al display si la información que le
  enviamos debe ser considerada un comando o un dato.
  Los comandos cambian la configuración del display. Los datos se
  escriben directamente en la memoria gráfica.
  Según el estado se considera:
     LOW : comando
     HIGH : datos gráficos
 
  Pin DIN
  Es el pin por el que se escribe en el display. Siempre debe ser
  conectado al pin SPI_MOSI
   
  Pin CLK
  Pin de reloj. Indica cuándo se puede leer el bit enviado. Siempre
  debe ser conectado a SPI_CLK

  Pin VCC
  Alimentación. Siempre conectado a 3.3V.
 
  Pin LIGHT
  Permite encender la luz de fondo del display. Acepta distintos 
  niveles de intensidad (se puede conectar a un pin PWM).
  Los estados de la luz de fondo según el pin son:
    encendida : LOW
    apagada : HIGH
 
  Pin GND
  Siempre conectado a tierra.
 
  Cuando activamos las comunicaciones SPI por hardware hacemos uso de
  todos sus pines.
  En este caso no tenemos en cuenta el pin MISO, pero no podemos 
  utilizarlo para otra cosa porque se sigue escribiendo y leyendo en el.

  Las funciones básicas de escritura son dos:
    _writeCommand para enviar un byte de comandos
    _writeData para enviar un número determinado de datos gráficos
  Puesto que los comandos son muy compactos (un sólo byte), la función
  _writeCommand sólo admite un byte.
  En cambio _writeData permite enviar varios bytes a partir de un
  puntero, puesto que los datos gráficos pocas veces en envían 
  individualmente.

  La resoución es de 84x48. Se necesitan 504 bytes de memoria de vídeo.

  Al ser un display LCD, la velocidad de refresco es bastante baja.
  Cuando se mueve un objeto por pantalla deja "estela". No está
  preparada para imágenes en movimiento.

  Teniendo en cuenta esto, el pequeño tamaño de la memoria de vídeo,
  y que la velocidad de transmisión es aceptable, no vale la pena
  utilizar técnicas de doble buffer de vídeo para acelerar la
  escritura.
  Usaremos una memoria de vídeo simple que será enviada al display
  íntegramente cada vez que refresquemos con la función show().

  Las placas ESP tienen un buffer de salida SPI limitado y además no
  se gestiona su saturación de manera automática. No ocurre lo mismo
  en RPi.
  Esto limita el tamaño de información a enviar.
  Está comprobado que el buffer SPI se satura si le pedimos enviar
  los 84 bytes de una página de una sóla vez.
  Por esa razón enviamos cada página en dos bloques de 42 bytes cada
  uno. Aunque no serái necesario en RPi, dejaremos el sistema así
  para guarda compatibilidad.
  
  Objetos públicos:
    const byte xMax=84
    const byte yMax=48
    const byte pagesMax=6
    byte videoMem[6][84]
    void reset()
    void clear()
    void show()
    void begin(byte pinRST,byte pinDC,byte pinCS)
    void begin(byte pinRST,byte pinDC,byte pinCS,uint32_t spiFreq)
    void setPixel(int16_t x,int16_t y,byte color)
	bool getPixel(int16_t x,int16_t y)
	void lineV(int16_t x,int16_t y1,int16_t y2,byte color)
	void lineH(int16_t y,int16_t x1,int16_t x2,byte color)
	void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor)
	void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,byte borderColor)
	void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte color)
	void drawSpritePage(int16_t x,int16_t page,RoJoSprite *sprite,byte color)
	void drawSprite(int16_t x,int16_t y,RoJoSprite *sprite,byte color)
	void getSprite(int16_t x1,int16_t page1,int16_t x2,int16_t page2,RoJoSprite *sprite)
	void reverse(bool reserveMode)
	void setContrast(byte c)
 
  Nota:
  La librería no tiene en cuenta los pines CS controlados por hardware.
  Se puede definir cualquier pin para CS porque se gestiona por software.
  En placas ESP estos pines se pueden desactivar.
  En Raspberry no. Esto quiere decir que RPi siempre seguirá gestionando
  el pin CS que tenga activo en ese momento.
  La ventaja de definir el pin CS por software es que podemos tener
  varios dispositivos SPI conectados a distintos pines de control y 
  no tendremos interferencias entre ellos.
*/

#ifndef RoJoLCD5110_cpp
#define RoJoLCD5110_cpp

//Si declaramos la siguiente línea utilizaremos una tarjeta SD como almacenamiento por defecto
//en vez de un sistema SPIFFS
#define UseSD

#include <Arduino.h>
#include <SPI.h> //Gestión de comunicaciones SPI
#ifdef UseSD //Si debemos utilizar una tarjeta SD...
  #include "RoJoLCD5110SD.h"
  #include "RoJoSpriteSD.h"
#else //Si debemos utilizar SPIFFS
  #include "RoJoLCD5110.h"
  #include "RoJoSprite.h"
#endif

void RoJoLCD5110::_startSPI()
{
  //Inicia una transacción SPI
  SPI.beginTransaction(_spiSetting);
  digitalWrite(_pinCS,LOW);
}

void RoJoLCD5110::_endSPI()
{
  //Finaliza una transacción SPI
  digitalWrite(_pinCS,HIGH);
  SPI.endTransaction();
}

void RoJoLCD5110::reset()
{
  //Resetea el display
  digitalWrite(_pinRST,LOW);
  delay(10);
  digitalWrite(_pinRST,HIGH);
  delay(10);
}

void RoJoLCD5110::_writeCommand(byte c)
{
  //Envía un byte como comando al SPI

  //Enviaremos un comando
  digitalWrite(_pinDC,LOW);
  //Escribimos el buffer recibido con todos sus comandos
  SPI.transfer(c);
}

void RoJoLCD5110::_writeData(byte *buffer,uint16_t len)
{
	//Envía un número de bytes de datos gráficos por SPI a partir
	//de un puntero

	//Se enviarán datos gráficos
	digitalWrite(_pinDC,HIGH);
	//Se envía el buffer
	SPI.writeBytes(buffer,len);
}

void RoJoLCD5110::clear()
{
  //Borra la memoria de vídeo pública

  for(byte x=0;x<xMax;x++) //Recorremos todas las columnas
    for(byte p=0;p<pagesMax;p++) //Recorremos todas las páginas
      videoMem[p][x]=0; //Lo borramos
}

void RoJoLCD5110::show()
{
	//Envía la memoria de vídeo al display

  byte buffer[42];
  
  //Iniciamos una transacción SPI
  _startSPI();
  //Comenzamos por la primera columna
  _writeCommand(0x80);
  //Comenzamos por la primera página
  _writeCommand(0x40);
  //Recorremos todas las páginas
  for(byte p=0;p<pagesMax;p++)
  {
    //Cada página es enviada en dos veces. 84/2=42 bytes/vez
    //Utilizamos el buffer
    //No podemos enviar directamente la memoria de vídeo
    for(byte v=0;v<2;v++)
    {
      //Copiamos la información a enviar en el buffer
      for(byte i=0;i<42;i++) buffer[i]=videoMem[p][v*42+i];
      //Enviamos la información del buffer
      _writeData(buffer,42);
    }
  }
  //Finalizamos la transacción SPI
  _endSPI();
}

void RoJoLCD5110::begin(byte pinRST,byte pinDC, byte pinCS)
{
  //Inicialización del display

  //Guardamos los parámetros en variables internas
  _pinDC=pinDC;
  _pinRST=pinRST;
  _pinCS=pinCS;
  
  //Siempre escribiremos en los pines DC, RES y CS
  pinMode(_pinDC,OUTPUT);
  pinMode(_pinRST,OUTPUT);
  pinMode(_pinCS,OUTPUT);

  digitalWrite(_pinRST,HIGH); //Comenzamos sin reiniciar el display
  digitalWrite(_pinDC,HIGH); //Comenzamos enviando datos
  digitalWrite(_pinCS,HIGH); //Comenzamos sin enviar datos
  
  //Definimos las caraterísticas de la conexión SPI
  _spiSetting = SPISettings(_spiFreq, MSBFIRST, SPI_MODE0);

  //Inicializamos las conexiones SPI
  SPI.begin();
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  SPI.setHwCs(false);

  //Reseteamos el display
  reset();
  //Comenzamos una transacción
  _startSPI();
    //Secuencia de inicialización
    _writeCommand(0x20|0x01); //Modo extendido: activamos el display y fijamos la escritura horizontal
	  _writeCommand(0x10|0x04); //Fijamos el bias a 4 (normal)
	  _writeCommand(0x80|65); //Contraste a 65
	  _writeCommand(0x20); //Salimos del modo extendido
	  _writeCommand(0x08|0x04); //Modo normal (no inverso)
	  _writeCommand(0x40); //Fijamos coordenada Y=0
	  _writeCommand(0x80); //Fijamos coordenada X=0
  _endSPI();

  //Borramos la memoria de vídeo
  clear();
  //Enviamos la memoria de vídeo al display
  show();
}

void RoJoLCD5110::begin(byte pinRST,byte pinDC,byte pinCS,uint32_t spiFreq)
{
  //Inicialización con parámetro de frecuencia de SPI

  //Guardamos el valor de la frecuencia de conexión SPI
  _spiFreq=spiFreq;
  //Llamamos al begin normal
  begin(pinRST,pinDC,pinCS);
}

void RoJoLCD5110::begin(byte pinRST,byte pinDC)
{
  //Inicialización con pines de reset y data/command

  //Utilizamos el pin CS por defecto de SPI llamando al begin normal
  begin(pinRST,pinDC,SS);
}

bool RoJoLCD5110::_inScreen(int16_t x,int16_t y)
{
  //Están las coordenadas dentro del rango visible?
  return  x>=0 && x<xMax && y>=0 && y<yMax;
}

void RoJoLCD5110::_mask(byte page,byte x,byte mask,byte color)
{
  //Aplica una máscara al byte de una página (de la memoria de vídeo)
  //Dependiendo del color
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe
  switch(color)
  {
    case 0: //Desactivar pixels
      videoMem[page][x]&=~mask;
      break;
    case 1: //Activar pixels
      videoMem[page][x]|=mask;
      break;
    case 2: //Invertir pixels
      videoMem[page][x]^=mask;
      break;
    case 4: //Escribe la máscara tal cual (sobreescribe)
      videoMem[page][x]=mask;
    default: //Cualquier otra opción
      break;
  }
}

void RoJoLCD5110::setPixel(int16_t x,int16_t y,byte color)
{
  //Dibuja un pixel
  //color:
  // 0 = desactivar
  // 1 = activar
  // 2 = invertir
  // 3 = no modificar

  //Si no está en el rango visible...hemos terminado
  if(!_inScreen(x,y)) return;

  //Calculamos y aplicamos la máscara
  _mask(y/8,x,1<<(y%8),color);
}

bool RoJoLCD5110::getPixel(int16_t x,int16_t y)
{
  //Devuelve el estado de un pixel

  //Si no está en el rango visible...está apagado
  if(!_inScreen(x,y)) return false;
  //Las coordenadas son visibles
  return (videoMem[y/8][x] & (1<<(y%8)));
}

void RoJoLCD5110::_swap(int16_t *a,int16_t *b)
{
  //Intercambia los valores de dos variables enteras
  int t=*a;*a=*b;*b=t;
}

void RoJoLCD5110::lineV(int16_t x,int16_t y1,int16_t y2,byte color)
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
  if(x<0 || x>=xMax) return;
  //La columna está en un rango visible
   
  //Siempre se debe cumplir que y1<=y2
  if(y1>y2) _swap(&y1,&y2);

  //Si la recta no es visible...hemos terminado
  if(y1>=yMax || y2<0) return;
  //Toda o parte de la recta es visible

  //Recortamos la parte que no sea visible
  if(y1<0) y1=0;
  if(y2>=yMax) y2=yMax-1;

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

void RoJoLCD5110::_lineH(int16_t y,int16_t x1,int16_t x2,byte color,bool withoutBorder)
{
  //Dibuja linea horizontal
  //Si withoutBorder es true no se diburajá el primer y último punto
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente

  //Si el color es transparente...hemos terminado
  if(color>2) return;

  //Si la fila está fuera de rango...hemos terminado
  if(y<0 || y>=yMax) return;
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
  if(x1>=xMax || x2<0) return;
  //Toda o parte de la recta es visible

  //Recortamos la parte que no sea visible
  if(x1<0) x1=0;
  if(x2>=xMax) x2=xMax-1;

  //Calculamos la páginas de la recta
  byte page=y/8;
  //Calculamos la máscara
  byte mask=1<<(y%8);

  //Recorremos todas las columnas visibles de la recta
  for(byte x=x1;x<=x2;x++) _mask(page,x,mask,color);
}

void RoJoLCD5110::lineH(int16_t y,int16_t x1,int16_t x2,byte color)
{
  //Dibuja línea horizontal
  _lineH(y,x1,x2,color,false);
}

void RoJoLCD5110::_rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,bool withoutBorder)
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
  if(x1>=xMax || x2<0 || y1>=yMax || y2<0) return;
  //Todo o parte del rectángulo es visible

  //Recortamos la parte que no sea visible
  if(x1<0) x1=0;
  if(x2>=xMax) x2=xMax-1;
  if(y1<0) y1=0;
  if(y2>=yMax) y2=yMax-1;

  //Recorremos todas las columnas y dibujamos las rectas verticales
  for(byte x=x1;x<=x2;x++) lineV(x,y1,y2,fillColor);
}

void RoJoLCD5110::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor)
{
  //Dibuja un rectángulo relleno
  _rect(x1,y1,x2,y2,fillColor,false);
}

void RoJoLCD5110::rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,byte borderColor)
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

void RoJoLCD5110::line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte color)
{
  //Dibuja una línea utilizando el algoritmo de Bresenham

  const bool steep=abs(y2-y1)>abs(x2-x1);
  
  if(steep)
  {
    _swap(&x1,&y1);
    _swap(&x2,&y2);
  }
 
  if(x1>x2)
  {
    _swap(&x1,&x2);
    _swap(&y1,&y2);
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

void RoJoLCD5110::drawSpritePage(int16_t x,int16_t page,RoJoSprite *sprite,byte color)
{
  //Dibuja un sprite en una página y columna
  //Colores permitidos:
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe

  int16_t xScreen,pageScreen;

  //Recorremos las páginas del sprite
  for(uint16_t pageCount=0;pageCount<(*sprite).heightPages();pageCount++)
  {
    //Calculamos la página de pantalla
    pageScreen=page+pageCount;
    //Si es visible...
    if(pageScreen>=0 && pageScreen<pagesMax)
    {
      //Recorremos todas las columnas
      for(uint16_t xCount=0;xCount<(*sprite).width();xCount++)
      {
        //Calculamos la columna de pantalla
        xScreen=x+xCount;
        //Si está dentro de la pantalla...
        if(xScreen>=0 && xScreen<xMax)
        {
          //Dibujamos el byte
          _mask(pageScreen,xScreen,(*sprite).getPage(xCount,pageCount),color);
        }
      }
    }
  }
}

void RoJoLCD5110::drawSprite(int16_t x,int16_t y,RoJoSprite *sprite,byte color)
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
      //Si el pixel está activo..lo dibujamos en el display
      if((*sprite).getPixel(xCount,yCount)) setPixel(x+xCount,y+yCount,color);
    }
  }
}

void RoJoLCD5110::getSprite(int16_t x1,int16_t page1,int16_t x2,int16_t page2,RoJoSprite *sprite)
{
  //Toma parte de la memoria de vídeo para crear un sprite

  //Borramos el sprite
  (*sprite).clean();
  //Siempre se debe cumplir que x1<=x2
  if(x1>x2) _swap(&x1,&x2);
  //Siempre se debe cumplir que page1<=page22
  if(page1>page2) _swap(&page1,&page2);
  //Si está fuera de pantalla...hemos terminado
  if(x1>=128 || x2<0 || page1>=8 || page2<0) return;
  //Recortamos las zonas no visibles
  if(x1<0) x1=0;
  if(x2>=128) x2=127;
  if(page1<0) page1=0;
  if(page2>=8) page2=7;
  //Dimensionamos el sprite
  uint16_t spriteWidth=x2-x1+1;
  uint16_t spritePages=page2-page1+1;
  (*sprite).setSize(spriteWidth,spritePages);
  //Copiamos el contenido de pantalla al sprite
  for(byte x=0;x<spriteWidth;x++)
    for(byte page=0;page<spritePages;page++)
      (*sprite).setPage(x,page,videoMem[page1+page][x1+x],4); //4=sobreescribiendo
}

void RoJoLCD5110::reverse(bool reverseMode)
{
	//Fija el display en modo invertido (blanco <-> negro)
	
	//Cuando arranca dejamos el modo invertido desactivado
	//  reverseMode = false -> pixel encendido = negro
	//  reverseMode = true  -> pixel encendido = blanco
		
	_startSPI();
	  //Dependiendo del parámetro aplicamos un modo u otro
	  _writeCommand(reverseMode?0x0D:0x0C);
	_endSPI();
}

void RoJoLCD5110::setContrast(byte c)
{
	//Fija el contraste
	//El rango permitido es [0,127]
	//Cuando arranca se fija por defecto a 65
	
	_startSPI();
	  _writeCommand(0x21); //Activamos el modo extendido
	  _writeCommand(0x80|c); //Fijamos el contraste
	  _writeCommand(0x20); //Modo normal
	_endSPI();
}

#endif
