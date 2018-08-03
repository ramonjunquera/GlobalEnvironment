/*
  Nombre de la librería: RoJoLCD5110.h
  Versión: 20180222
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
    const byte xMax=84;
    const byte yMax=48;
    const byte pagesMax=6;
    RoJoSprite videoMem;
    void reset();
    void show();
    void begin(byte pinRST,byte pinDC);
    void begin(byte pinRST,byte pinDC,byte pinCS);
    void begin(byte pinRST,byte pinDC,byte pinCS,uint32_t spiFreq);
    void reverse(bool reserveMode);
    void setContrast(byte c);
 
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
  digitalWrite(_pinCS_display,LOW);
}

void RoJoLCD5110::_endSPI()
{
  //Finaliza una transacción SPI
  digitalWrite(_pinCS_display,HIGH);
  SPI.endTransaction();
}

void RoJoLCD5110::reset()
{
  //Resetea el display
  digitalWrite(_pinRST_display,LOW);
  delay(10);
  digitalWrite(_pinRST_display,HIGH);
  delay(10);
}

void RoJoLCD5110::_writeCommand(byte c)
{
  //Envía un byte como comando al SPI

  //Enviaremos un comando
  digitalWrite(_pinDC_display,LOW);
  //Escribimos el buffer recibido con todos sus comandos
  SPI.transfer(c);
}

void RoJoLCD5110::_writeData(byte *buffer,uint16_t len)
{
	//Envía un número de bytes de datos gráficos por SPI a partir
	//de un puntero

	//Se enviarán datos gráficos
	digitalWrite(_pinDC_display,HIGH);
  //Se envía el buffer
  #ifdef ARDUINO_ARCH_AVR //Si es un Arduino...
    SPI.transfer(buffer,len);
  #else //Si cualquier otro caso (ESP o RPi)...
    SPI.writeBytes(buffer,len);
  #endif
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
      for(byte i=0;i<42;i++) buffer[i]=(*videoMem).getPage(v*42+i,p);
      //Enviamos la información del buffer
      _writeData(buffer,42);
    }
  }
  //Finalizamos la transacción SPI
  _endSPI();
}

void RoJoLCD5110::begin(byte pinRST_display,byte pinDC_display, byte pinCS_display, byte pinCS_SD)
{
  //Inicialización del display

  //Guardamos los parámetros en variables internas
  _pinDC_display=pinDC_display;
  _pinRST_display=pinRST_display;
  _pinCS_display=pinCS_display;
  
  //Siempre escribiremos en los pines DC, RES y CS
  pinMode(_pinDC_display,OUTPUT);
  pinMode(_pinRST_display,OUTPUT);
  pinMode(_pinCS_display,OUTPUT);

  digitalWrite(_pinRST_display,HIGH); //Comenzamos sin reiniciar el display
  digitalWrite(_pinDC_display,HIGH); //Comenzamos enviando datos
  digitalWrite(_pinCS_display,HIGH); //Comenzamos sin enviar datos
  
  //Definimos las características de la conexión SPI del display
  _spiSetting = SPISettings(_freq_display, MSBFIRST, SPI_MODE0);

  //Inicializamos las conexiones SPI
  SPI.begin();
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  //En placas Arduino no es posible desactivar el pin CS por defecto
  #ifndef ARDUINO_ARCH_AVR //Si no es un Arduino...
    SPI.setHwCs(false);
  #endif

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

  //Creamos el sprite de la memoria de vídeo
  videoMem = new RoJoSprite(pinCS_SD);
  //Dimensionamos el sprite de la memoria de vídeo
  (*videoMem).setSize(xMax,pagesMax);
  //Enviamos la memoria de vídeo al display
  show();
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

RoJoLCD5110::~RoJoLCD5110()
{
  //Destructor

  //Borramos el sprite con la memoria de vídeo
  delete videoMem;
}

#endif
