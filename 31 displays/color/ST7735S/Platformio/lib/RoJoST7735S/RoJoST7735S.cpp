/*
  Nombre de la librería: RoJoST7735S.h
  Versión: 20191103
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RoJoST7735S SPI 80x160
*/

#ifndef RoJoST7735S_cpp
#define RoJoST7735S_cpp

#include <RoJoST7735S.h>

// Envía al display un comando con sus correspondientes parámetros.
// Los parámetros son opcionales.
// Después de los parámetros, siempre hay que enviar uno adicional
// con valor negativo para indicar que no hay más.
void RoJoST7735S::_writeCommand(byte command,...) {
  //ST7735S envía la información (comando y parámetros) en paquetes de 8bits (byte)
  //El comando se envía en modo comando y sus parámetros en modo datos

  //Definimos la función con número de parámetros variable
  //Para este tipo de funciones es obligatorio un parámetro inicial
  //En este caso: byte command
  //... representa la lista variable de parámetros
  //... siempre debe ser el último parámetro

  //Definimos la variable que contendrá la lista de parámetros de la función
  va_list paramList;
  //Cargamos la lista con los parametros de la función.
  //Se debe indicar a partir de qué parámetro comienza la lista (por eso es obligatorio uno)
  va_start(paramList,command);

  //Nota:
  //No se puede saber cuántos elementos tiene la lista de parámetros
  //Las técnicas más comunes para trabajar con ellas son:
  //- El primer parámetro (obligatorio) indica el número de elementos
  //- El último parámetro de la lista tiene un valor especial para indicar que no hay más

  int paramValue; //Declaramos variable en la que extraeremos los valores de la lista
  digitalWrite(_pinDC,LOW); //Modo comandos
  SPI.transfer(command); //Enviamos el comando
  //Todos los parámetros deben pasarse en modo datos
  //Lo activamos ahora y nos aseguramos que quedará así al finalizar
  digitalWrite(_pinDC,HIGH); //Modo datos
  //Extraemos el valor de la lista y si es válido...
  while((paramValue=va_arg(paramList,int))>=0) {
    //...enviamos el parámetro
    SPI.transfer(paramValue);
  }
  //Hemos terminado de trabajar con la lista
  va_end(paramList);
}

// Inicia comunicación
void RoJoST7735S::_startCOMM() {
  SPI.beginTransaction(_spiSetting);
  digitalWrite(_pinCS,LOW);
}

// Finaliza comunicación
void RoJoST7735S::_endCOMM() {
  digitalWrite(_pinCS,HIGH);
  SPI.endTransaction();
}

// Activa/Desactiva el modo hibernación
void RoJoST7735S::sleep(bool mode) {
  //En hibernación se desactiva del display, pero permite seguir dibujando
  //Cuando se salga de hibernación y se vuelva a activar se mostrará el resultado
  _startCOMM();
    if(mode) {
      _writeCommand(0x10,-1); //Sleep IN
      _writeCommand(0x28,-1); //Display OFF
    } else {
      _writeCommand(0x29,-1); //Display ON
      _writeCommand(0x11,-1); //Sleep OUT
      delay(125); //Se deben esperar al menos 120ms
    }
  _endCOMM();
}

//Reset & inicialización
void RoJoST7735S::reset() {
  //Hard reset
  digitalWrite(_pinRES,LOW);
  delay(20);
  digitalWrite(_pinRES,HIGH);
  delay(120);
  //Soft reset
  _startCOMM();
    _writeCommand(0x01,-1); //Comando soft reset
  _endCOMM();
  delay(125); //Se deben esperar al menos 120ms
  //Secuencia de inicialización
  _startCOMM();
    //0xB1 : FRMCTR1 : Frame rate control (in normal mode/ful colors)
    //  RTNA = 0x01 = 1 (4 bits)
    //  FPA = 0x2C = 44 (6 bits)
    //  BPA = 0x2D = 45 (6 bits)
    //  fosc = 850Hz (constante)
    //Frame rate = fosc/((RTNA*2+40)*(LINE+FPA+BPA+2))
    _writeCommand(0xB1,0x01,0x2C,0x2D,-1);
    //0xB2 : FRMCTR2 : Frame control (in iddle mode/8-colors)
    //  RTNB = 0x01 = 1 (4 bits)
    //  FPB = 0x2C = 44 (6 bits)
    //  BPB = 0x2D = 45 (6 bits)
    //  fosc = 850Hz (constante)
    //Frame rate = fosc/((RTNB*2+40)*(LINE+FPB+BPB+2))
    _writeCommand(0xB2,0x01,0x2C,0x2D,-1);
    //0xB3 : FRMCTR3 : Frame rate control (in partial mode/full colors)
    //  RTNC = 0x01 = 1 (4 bits)
    //  FPC = 0x2C = 44 (6 bits)
    //  BPC = 0x2D = 45 (6 bits)
    //  RTND = 0x01 = 1 (4 bits)
    //  FPD = 0x2C = 44 (6 bits)
    //  BPD = 0x2D = 45 (6 bits)
    //  fosc = 850Hz (constante)
    //Dot inversion mode:
    //  Frame rate = fosc/((RTNC*2+40)*(LINE+FPC+BPC+2))
    //Column inversion mode
    //  Frame rate = fosc/((RTND*2+40)*(LINE+FPD+BPD+2))
    _writeCommand(0xB3,0x01,0x2C,0x2D,0x01,0x2C,0x2D,-1);
    //0xB4 : INVCTR : Display inversion control
    //  param1 = 0x07 = 0b00000111
    //    bit 0 = NLC = 1 = Inversion setting in full colors partial mode = column inversion
    //    bit 1 = NLB = 1 = Inversion settings in idle mode = column inversion
    //    bit 2 = BLA = 1 = Inversion setting in full colors normal mode = column inversion
    _writeCommand(0xB4,0x07,-1);
    //0xC0 : PWCTR1 : Power control 1
    //  param1 = 0xA2 = 0b10100010
    //    bit 0-4 = VRHP = 0b00010 -> GVDD = 4.6
    //    bit 5-7 = AVDD = 0b101 -> AVDD = 5
    //  param2 = 0x02 = 0b00000010
    //    bit 0-4 = VRHN = 0b00010 -> GVCL = -4.6
    //  param3 = 0x84 = 0b10000100
    //    bit 0 = VRHP (bit 5) = 0
    //    bit 0 = VRHN (bit 5) = 0
    //    bit 6-7 = MODE = 0b10 -> AUTO
    _writeCommand(0xC0,0xA2,0x02,0x84,-1);
    //0xC1 : PWCTR2 : Power control 2
    //  param1 = 0xC5 = 0b11000101
    //    bit 0-1 = VGHBT = 0b01 ->  VHHBT = 3*AVDD-0.5
    //    bit 2-3 = VGLSEL = 0b01 -> VGLSEL = -10
    //    bit 6-7 = VGH25 = 0b11 -> VGH25 = 2.4
    _writeCommand(0xC1,0xC5,-1);
    //0xC2 : PWCTR3 : Power control 3 (in normal mode/full colors)
    //  param 1 = 0x0A = 0b00001010
    //    bit 0-2 = APA = 0b010 -> Amount of current in operational amplifier = medium low
    //    bit 3-5 = SAPA = 0b001 -> Amount of current in operational amplifier = small
    //    bit 6-7 = DCA (bits 8-9) = 0b00
    //  param 2 = DCA (bits 0-7)= 0x00 = 0b00000000
    //  DCA = 0b0000000000
    _writeCommand(0xC2,0x0A,0x00,-1);
    //0xC3 : PWCTR4 : Power control 4 (in idle mode/8-colors)
    //  param 1 = 0x8A = 0b10001010
    //    bit 0-2 = APB = 0b010 -> Amount of current in operational amplifier = medium low
    //    bit 3-5 = SAPB = 0b001 -> Amount of current in operational amplifier = small
    //    bit 6-7 = DCB (bits 8-9)= 0b10
    //  param 2 = DCB (bits 0-7)= 0x2A = 0b00101010
    //  DCB = 0b0100101010
    _writeCommand(0xC3,0x8A,0x2A,-1);
    //0xC4 : PWCTR5 : Power control 5 (in partial mode/full colors)
    //  param 1 = 0x8A = 0b10001010
    //    bit 0-2 = APC = 0b010 -> Amount of current in operational amplifier = medium low
    //    bit 3-5 = SAPC = 0b001 -> Amount of current in operational amplifier = small
    //    bit 6-7 = DCC (bits 8-9)= 0b10
    //  param 2 = DCC (bits 0-7)= 0xEE = 0b11101110
    //  DCB = 0b0111101110
    _writeCommand(0xC4,0x8A,0xEE,-1);
    //0xC5 : VMCTR1 : VCOM control 1
    //  param 1 = 0x0E = 0b00001110
    //    bit 0-5 = VCOMS = 0b001110 -> VCOM voltage setting = -0.775
    _writeCommand(0xC5,0x0E,-1);
    //0x36 : MADCTL : Memory data access control
    //  param 1 = 0xC8 = 0b11001000
    //    bit 2 = MH = 0 = Horizontal refresh order -> LCD horizontal refresh left to right
    //    bit 3 = RGB = 1 = RGB-BGR order -> Color selector switch control = BGR color filter panel
    //    bit 4 = ML = 0 = Vertical refresh order -> LCD vertical refresh direction control = LCD vertical refresh top to bottom
    //    bit 5 = MV = 0 = Row/Column exchange
    //    bit 6 = MX = 1 = Column address order
    //    bit 7 = MY = 1 = Row address order
    _writeCommand(0x36,0xC8,-1);
    //0x3A : COLMOD : Interface pixel format
    //  param 1 = 0x05 = 0b00000101
    //    bit 0-2 = IFPF = 0b101 -> MCU interface color format = 16-bit/pixel
    _writeCommand(0x3A,0x05,-1);
    //0x21 : INVON : Display inversion on
    _writeCommand(0x21,-1);
    //0xE0 : GMCTRP1 : Gamma ('+' polarity) correction characteristics setting
    //  param 1 (6 bits) = VRF0P = 0x02 = Variable resistor VRHP
    //  param 2 (6 bits) = VOS0P = 0x1C = the voltage of V0 grayscale
    //  param 3 (6 bits) = PK0P = 0x07 = the voltage of V1 grayscale
    //  param 4 (6 bits) = PK1P = 0x12 = the voltage of V3 grayscale
    //  param 5 (6 bits) = PK2P = 0x37 = the voltage of V4 grayscale
    //  param 6 (6 bits) = PK3P = 0x32 = the voltage of V12 grayscale
    //  param 7 (6 bits) = PK4P = 0x29 = the voltage of V20 grayscale
    //  param 8 (6 bits) = PK5P = 0x2D = the voltage of V28 grayscale
    //  param 9 (6 bits) = PK6P = 0x29 = the voltage of V36 grayscale
    //  param 10 (6 bits) = PK7P = 0x25 = the voltage of V44 grayscale
    //  param 11 (6 bits) = PK8P = 0x2B = the voltage of V52 grayscale
    //  param 12 (6 bits) = PK9P = 0x39 = the voltage of V56 grayscale
    //  param 13 (6 bits) = SELV0P = 0x00 = the voltage of V60 grayscale
    //  param 14 (6 bits) = SELV1P = 0x01 = the voltage of V62 grayscale
    //  param 15 (6 bits) = SELV62P = 0x03 = the voltage of V63 grayscale
    //  param 16 (6 bits) = SELV63P = 0x10 = Variable resistor VRLP
    _writeCommand(0xE0,0x02,0x1C,0x07,0x12,0x37,0x32,0x29,0x2D,0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10,-1);
    //0xE1 : GMCTRN1 : Gamma ('-' polarity) correction characteristics setting
    //  param 1 (6 bits) = VRF0N = 0x03 = Variable resistor VRHN
    //  param 2 (6 bits) = VOS0N = 0x1D = the voltage of V0 grayscale
    //  param 3 (6 bits) = PK0N = 0x07 = the voltage of V1 grayscale
    //  param 4 (6 bits) = PK1N = 0x06 = the voltage of V3 grayscale
    //  param 5 (6 bits) = PK2N = 0x2E = the voltage of V4 grayscale
    //  param 6 (6 bits) = PK3N = 0x2C = the voltage of V12 grayscale
    //  param 7 (6 bits) = PK4N = 0x29 = the voltage of V20 grayscale
    //  param 8 (6 bits) = PK5N = 0x2D = the voltage of V28 grayscale
    //  param 9 (6 bits) = PK6N = 0x2E = the voltage of V36 grayscale
    //  param 10 (6 bits) = PK7N = 0x2E = the voltage of V44 grayscale
    //  param 11 (6 bits) = PK8N = 0x37 = the voltage of V52 grayscale
    //  param 12 (6 bits) = PK9N = 0x3F = the voltage of V56 grayscale
    //  param 13 (6 bits) = SELV0N = 0x00 = the voltage of V60 grayscale
    //  param 14 (6 bits) = SELV1N = 0x00 = the voltage of V62 grayscale
    //  param 15 (6 bits) = SELV62N = 0x02 = the voltage of V63 grayscale
    //  param 16 (6 bits) = SELV63N = 0x10 = Variable resistor VRLN
    _writeCommand(0xE1,0x03,0x1D,0x07,0x06,0x2E,0x2C,0x29,0x2D,0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10,-1);
    //0x13 : NORON : Normal display mode on
    _writeCommand(0x13,-1);
  _endCOMM();
  //Rotación por defecto=0: 80x160. Vertical. En M5Stick-C el botón M5 abajo
  rotation(0);
  //Borramos el display
  clear();
  //Salimos del modo de bajo consumo
  sleep(false);
}

//Inicialización
void RoJoST7735S::begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqCOMM,byte pinCLK,byte pinMOSI,int8_t pinMISO) {
  //En M5Stick-C es necesario activar la alimentación al display desde el chip
  //AXP192 que gestiona la batería
  #ifdef ARDUINO_M5Stick_C
    Wire1.begin(21,22);
    Wire1.beginTransmission(0x34);
    Wire1.write(0x12);  
    Wire1.write(0x4D); //Enable DC-DC1, OLED_VDD, 5B V_EXT
    Wire1.endTransmission();
  #endif
  //Este display tiene una profundidad de color de 16 bits (color)
  _colorDepth=16;
  //Definimos las caraterísticas de la conexión SPI
  _spiSetting=SPISettings(freqCOMM,MSBFIRST,SPI_MODE0);
  //Indicamos los pines SPI.
  //El pin MISO no se utiliza para este display
  SPI.begin(pinCLK,pinMISO,pinMOSI);
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  //En placas Arduino no es posible desactivar el pin CS por defecto
  #ifndef ARDUINO_ARCH_AVR //Si no es un Arduino...
    SPI.setHwCs(false);
  #endif
  //Guardamos los parámetros en variables internas
  _pinDC=pinDC;
  _pinCS=pinCS;
  _pinRES=pinRES;
  //Siempre escribiremos en los pines DC, RES y CS
  pinMode(_pinDC,OUTPUT);
  pinMode(_pinRES,OUTPUT);
  pinMode(_pinCS,OUTPUT);
  //Inicializamos el estado de los pines
  digitalWrite(_pinRES,HIGH); //Comenzamos sin reiniciar el display
  digitalWrite(_pinDC,HIGH); //Comenzamos enviando datos
  digitalWrite(_pinCS,HIGH); //Comenzamos sin seleccionar el chip
  //Reseteamos el display
  reset();
  //Llamamos a la inicialización de la clase padre
  RoJoGraph::begin(); //Principalmente inicializa SPIFFS
}

// Configura la rotación
void RoJoST7735S::rotation(byte r) {
  //     M5Stick C
  // r : posición botón M5   : orientación
  //---  -------------------   -----------
  // 0 : abajo                 vertical (*)
  // 1 : derecha               apaisado
  // 2 : arriba                vertical
  // 3 : izquierda             apaisado
  
  //Nos aseguramos que es un valor permitido
  r%=4;
  
  _startCOMM();
    //Código correspondiente a la rotación
    _writeCommand(0x36,_rotationCodes[r],-1);
    //Si la rotación es impar
    if(r%2) {
      //El formato será apaisado
      _x0=_y0Default;
      _y0=_x0Default;
      _xMax=_yMaxDefault;
      _yMax=_xMaxDefault;
    } else {
      //El formato será vertical
      _x0=_x0Default;
      _y0=_y0Default;
      _xMax=_xMaxDefault;
      _yMax=_yMaxDefault;
    }
  _endCOMM();
}

//Anchura de display (dummy)
uint16_t RoJoST7735S::xMax() {
  return _xMax;
}

//Altura de display (dummy)
uint16_t RoJoST7735S::yMax() {
  return _yMax;
}

//Define área de dibujo
//Sin gestión de SPI. No se comprueba coherencia de parámetros
void RoJoST7735S::_setCursorRangeY(int16_t y1,int16_t y2) {
  //0x2B=Page Address Set
  _writeCommand(0x2B,-1);
  SPI.transfer16(_y0+y1);
  SPI.transfer16(_y0+y2);
}
void RoJoST7735S::_setCursorRangeX(int16_t x1,int16_t x2) {
  //0x2A=Column Address Set
  _writeCommand(0x2A,-1);
  SPI.transfer16(_x0+x1);
  SPI.transfer16(_x0+x2);
  //Write memory
  _writeCommand(0x2C,-1);
}
void RoJoST7735S::_setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2) {
  _setCursorRangeY(y1,y2);
  _setCursorRangeX(x1,x2);
}

//Dibuja un pixel
//Devuelve true si lo consigue si el pixel es visible
bool RoJoST7735S::drawPixel(int16_t x,int16_t y,uint16_t color) {
  //Si las coordenadas están fuera de rango...terminamos
  if(x<0 || x>=(int16_t)_xMax || y<0 || y>=(int16_t)_yMax) return false;

  _startCOMM();
    _setCursorRange(x,y,x,y);
    SPI.transfer16(color);
  _endCOMM();
  return true;
}

//Dibuja un rectángulo relleno de un color
//Devuelve true si tiene parte visible
bool RoJoST7735S::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color) {
  //Intercambiamos coordenadas erróneas
  if(x1>x2) {int16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {int16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Calculamos el área visible
  displayRange r=visibleRange(x1,y1,x2-x1+1,y2-y1+1);
  //Si no hay área visible...hemos terminado
  if(!r.visible) return false;

  _startCOMM();
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    for(uint16_t y=r.y1;y<=r.y2;y++)
      for(uint16_t x=r.x1;x<=r.x2;x++)
        SPI.transfer16(color);
  _endCOMM();
  
  //Tiene parte visible
  return true;
}

//Dibuja un sprite en unas coordenadas
//Sobreescribe la información existente
bool RoJoST7735S::drawSprite(RoJoSprite *sprite,int16_t x,int16_t y) {
  //Calculamos el área visible
  displayRange r=visibleRange(x,y,sprite->xMax(),sprite->yMax());
  //Si no hay área visible...hemos terminado
  if(!r.visible) return false;

  _startCOMM();
    //Definimos el rango del cursor
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    //Calculamos el rango visible del sprite origen
    int16_t dx1=r.x1-x,dx2=r.x2-x,dy1=r.y1-y,dy2=r.y2-y;
    //Recorremos todas las filas visibles del sprite origen
    for(int16_t dy=dy1;dy<=dy2;dy++) {
      //Recorremos todas las columnas visibles del sprite origen y enviamos el color del pixel
      for(int16_t dx=dx1;dx<=dx2;dx++) SPI.transfer16(sprite->getPixel(dx,dy));
      #ifdef ESP8266
        yield(); //Refrescamos WatchDog
      #endif
    }
  _endCOMM();
  return true;
}

//Dibuja un sprite directamente de un archivo
byte RoJoST7735S::drawSprite(String filename,int16_t x,int16_t y) {
  //Este método no es imprescindible, puesto que ya está definido en RoJoGraph
  //Lo hacemos para optimizarlo

  //Tabla de errores (los primeros son los de infoSprite):
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe
  //  2 : La profundidad de color no está reconocida
  //  3 : La profundidad de color no coincide con la del display

  //Declaración de variables
  uint16_t width,height; //Anchura y altura
  byte colorDepth; //Profundidad de color
  //Leemos los valores del archivo bmp
  byte errorCode=infoSprite(filename,&width,&height,&colorDepth);
  //Si tenemos algún error...lo devolvemos
  if(errorCode) return errorCode;
  //No tenemos errores

  //Si la profundidad de color no coincide con la del display...terminamos con error
  if(_colorDepth!=colorDepth) return 3;

  //Calculamos el área visible
  displayRange r=visibleRange(x,y,width,height);
  //Si no hay área visible...hemos terminado ok
  if(!r.visible) return 0;

  uint16_t color;
  uint32_t rowLength=width*2; //Número de bytes que contiene una línea
  uint32_t offsetBase=5+2*(r.x1-x); //Offset de datos gráficos. Se la columna inicial
  uint32_t ry2=r.y2,rx2=r.x2,y32=y;

  //Nota:
  //Diferenciaremos si el sistema de archivos es SPIFFS o SD
  //Podemos utilziar la conexión SPI con el display al mismo tiempo que trabajamos con SPIFFS sin interferencias
  //Con una SD no es posible, porque ambos dispositivos utilizan la conexión SPI. Por lo tanto, tendremos
  //que asegurarnos de mantener sólo una conexión SPI en cada momento.
  //Si definimos un rango de dibujo en el display, lo recordará aunque finalicemos la transacción.
  //Incluso recordará la posición del cursor para la escritura del siguiente dato gráfico

  //Leemos los datos gráficos
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
    _startCOMM();
      //Definimos rango de escritura en display
      _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    _endCOMM();
    //Recorremos las filas visibles del display
    for(uint32_t yy=(uint32_t)r.y1;yy<=ry2;yy++) {
      //Posicionamos offset en archivo
      f.seek(offsetBase+rowLength*(yy-y32));
      //Recorremos las columnas visibles del display
      for(uint32_t xx=r.x1;xx<=rx2;xx++) {
        //Leemos el color
        f.read((byte *)&color,2);
        //Dibujamos el pixel
        _startCOMM();
          SPI.transfer16(color);
        _endCOMM();
      }
    }
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
    _startCOMM();
      //Definimos rango de escritura en display
      _setCursorRange(r.x1,r.y1,r.x2,r.y2);
      //Recorremos las filas visibles del display
      for(uint32_t yy=(uint32_t)r.y1;yy<=ry2;yy++) {
        //Posicionamos offset en archivo
        f.seek(offsetBase+rowLength*(yy-y32));
        //Recorremos las columnas visibles del display
        for(uint32_t xx=r.x1;xx<=rx2;xx++) {
          //Leemos el color
          f.read((byte *)&color,2);
          //Dibujamos el pixel
          SPI.transfer16(color);
        }
      }
    _endCOMM();
  #endif
  
  //hemos terminado de utilizar el archivo
  f.close();
  //Todo Ok
  return 0;
}

//Sincroniza dos sprites y envía las diferencias al display.
//Los sprites deben tener el mismo tamaño.
//Respuesta: true si todo es correcto
bool RoJoST7735S::drawSpriteSync(RoJoSprite *source,RoJoSprite *destination,int16_t x,int16_t y) {
  //Se detectan las diferencias entre los dos sprites y se escriben sobre el sprite destino
  //y se envían al display.
  //Finalmente el sprite destino queda igual que el origen.

  //Anotamos las medidas del sprite origen
  int16_t xMaxSprite=source->xMax();
  int16_t yMaxSprite=source->yMax();
  //Si los sprites tienen distinto tamaño...terminamos con error
  if(xMaxSprite!=(int16_t)destination->xMax() || yMaxSprite!=(int16_t)destination->yMax()) return false;
  //Comprobamos si tiene parte visible
  displayRange r=visibleRange(x,y,xMaxSprite,yMaxSprite);
  //Si no es visible...terminamos correctamente
  if(!r.visible) return true;
  //El sprite es total o parcialmente visible
  //En el display se dibujará el sprite en el rango: r.x1,r.y1,r.x2,r.y2
  //Se mostrará el siguiente rango del sprite: r.x1-x,r.y1-y,r.x2-x,r.y2-y
  //Es más sencillo recorrer las filas y columnas del sprite y si se detectan
  //diferencias, hacer la conversión a coordenadas de display
  
  //Calculamos la última fila y columna a procesar en el sprite
  //Reaprovechamos variables
  xMaxSprite=r.x2-x;
  yMaxSprite=r.y2-y;

  bool selectedRangeY; //Se ha seleccionado el rango vertical con la fila procesada?
  int16_t xSprite; //Columna procesada. Coordenada x del sprite
  _startCOMM();
    //Recorremos todas las filas visibles del sprite
    for(int16_t ySprite=r.y1-y;ySprite<=yMaxSprite;ySprite++) {
      //Por ahora no se ha inicializado el rango vertical para la fila actual
      selectedRangeY=false;
      //Comenzamos por la primera columna
      xSprite=r.x1-x;
      //Mientras no hayamos procesado todas las columnas...
      while(xSprite<=xMaxSprite) {
        //Si el pixel actual no se ha modificado...
        if(source->getPixel(xSprite,ySprite)==destination->getPixel(xSprite,ySprite)) {
          //...no tenemos en cuenta este pixel. Pasaremos al siguiente
          xSprite++;
        }
        else { //El pixel actual ha sido modificado...
          //Si no se ha seleccionado la fila actual...
          if(!selectedRangeY) {
		        //...lo hacemos ahora. Coinvertimos a coordenadas de display
		        _setCursorRangeY(y+ySprite,y+ySprite);
		        //y lo anotamos
		        selectedRangeY=true;
		      }
          //Consideramos este pixel como procesado
          //Actualizamos su valor en el sprite destino
          destination->drawPixel(xSprite,ySprite,source->getPixel(xSprite,ySprite));
          //Por ahora la última columna modificada es la primera
          int16_t lastChangedColumn=xSprite;
          //Columna procesada = la siguiente a la primera
          int16_t processedColumn=xSprite+1;
          //Mientras llevemos menos de 5 pixels sin modificar...
          while(processedColumn-lastChangedColumn<=5) {
            //Si el pixel de la columna procesada ha cambiado...
            if(source->getPixel(processedColumn,ySprite)!=destination->getPixel(processedColumn,ySprite)) {
              //...anotamos que la última columna con cambios es la actual
              lastChangedColumn=processedColumn;
              //Consideramos este pixel como procesado
              //Actualizamos su valor en el sprite destino
              destination->drawPixel(processedColumn,ySprite,source->getPixel(processedColumn,ySprite));
            }
            //Aumentamos la posición de la columna procesada
            processedColumn++;
          } //end while
          //Seleccionamos como rango horizontal desde la columna actual hasta la última modificada
          //Convertimos a coordenadas de display
          _setCursorRangeX(x+xSprite,x+lastChangedColumn);
          //Enviamos los datos gráficos
          for(int16_t x0=xSprite;x0<=lastChangedColumn;x0++) SPI.transfer16(source->getPixel(x0,ySprite));
          //La primera columna pasará a ser la actual
          xSprite=processedColumn;
        }
      }
      #ifdef ESP8266
        yield(); //Refrescamos WatchDog
      #endif
    }
  _endCOMM();
  //Todo Ok
  return true;
}

#endif