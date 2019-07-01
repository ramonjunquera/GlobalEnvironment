/*
  Nombre de la librería: RoJoILI9341.h
  Versión: 20190629
  Autor: Ramón Junquera
  Descripción:
    Gestión de display ILI9341 SPI 240x320

  Notas:
    En este display, todos los datos que se le envían son de 16 bits.
    Si hay datos de un byte (como los comandos o sus parámetros), aún así
    se deben enviar como valor de 16 bits.
*/

#ifndef RoJoILI9341_cpp
#define RoJoILI9341_cpp

#include <RoJoILI9341.h>

//Máxima frecuencia SPI soportada por el display
uint32_t RoJoILI9341::_maxFreqSPI()
{
  return 79999999;
}

// Envía al display un comando con sus correspondientes parámetros.
// Los parámetros son opcionales.
// Después de los parámetros, siempre hay que enviar uno adicional
// con valor negativo para indicar que no hay más.
void RoJoILI9341::_writeCommand(byte command,...)
{
  //ILI9341 envía la información (comando y parámetros) como datos de 8bits
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
  digitalWrite(_pinDC,HIGH);
  //Extraemos el valor de la lista y si es válido...
  while((paramValue=va_arg(paramList,int))>=0)
  {
    //...enviamos el parámetro
    SPI.transfer(paramValue);
  }
  //Hemos terminado de trabajar con la lista
  va_end(paramList);
}

// Configura la rotación
void RoJoILI9341::rotation(byte r)
{
  // r : posición conectores : orientación : código
  //---  -------------------   -----------   -------
  // 0 : abajo                 vertical      0b01001000 = BGR(00001000) + MX(01000000)
  // 1 : derecha               apaisado      0b00101000 = BGR(00001000) + MV(00100000)
  // 2 : arriba                vertical      0b10001000 = BGR(00001000) + MY(10000000)
  // 3 : izquierda             apaisado      0b11101000 = BGR(00001000) + MV(00100000) + MX(01000000) + MY(10000000)
  
  //Nos aseguramos que es un valor permitido
  r%=4;
  
  _startSPI();
    //Memory access control
    //Código correspondiente a la rotación
    _writeCommand(0x36,_rotationCodes[r],-1);
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

// Activa/Desactiva el modo hibernación
void RoJoILI9341::sleep(bool mode)
{
  //En hibernación se desactiva del display, pero permite seguir dibujando
  //Cuando se salga de hibernación y se vuelva a activar se mostrará el resultado
  _startSPI();
    if(mode)
    {
      _writeCommand(0x10,-1); //Sleep IN
      _writeCommand(0x28,-1); //Display OFF
      delay(5);
    }
    else
    {
      _writeCommand(0x29,-1); //Display ON
      _writeCommand(0x11,-1); //Sleep OUT
      delay(120);
    }
  _endSPI();
}

//Define área de dibujo
//Sin gestión de SPI. No se comprueba coherencia de parámetros
void RoJoILI9341::_setCursorRangeY(int16_t y1,int16_t y2)
{
  //0x2B=Page Address Set
  _writeCommand(0x2B,(byte)(y1>>8),(byte)(y1 & 0xFF),(byte)(y2>>8),(byte)(y2 & 0xFF),-1);
}
void RoJoILI9341::_setCursorRangeX(int16_t x1,int16_t x2)
{
  //0x2A=Column Address Set
  _writeCommand(0x2A,(byte)(x1>>8),(byte)(x1 & 0xFF),(byte)(x2>>8),(byte)(x2 & 0xFF),-1);
  //Write memory
  _writeCommand(0x2C,-1);
}

//Dibuja un rectángulo relleno de un color
//Devuelve true si tiene parte visible
bool RoJoILI9341::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  //Intercambiamos coordenadas erróneas
  if(x1>x2) {int16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {int16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Calculamos el área visible
  displayRange r=visibleRange(x1,y1,x2-x1+1,y2-y1+1);
  //Si no hay área visible...hemos terminado
  if(!r.visible) return false;

  _startSPI();
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    for(uint16_t y=r.y1;y<=r.y2;y++)
      for(uint16_t x=r.x1;x<=r.x2;x++)
        SPI.transfer16(color);
  _endSPI();
  
  //Tiene parte visible
  return true;
}

//Anchura de display (dummy)
uint16_t RoJoILI9341::xMax()
{
  return _xMax;
}

//Altura de display (dummy)
uint16_t RoJoILI9341::yMax()
{
  return _yMax;
}

//Reset & inicialización
void RoJoILI9341::reset()
{
  
  //Hard reset
  digitalWrite(_pinRES,LOW);
  delay(20);
  digitalWrite(_pinRES,HIGH);
  delay(120);

  //En ILI9486 tras un hard reset se debe hacer un soft reset
  _startSPI();
    _writeCommand(0x01,-1); //Comando soft reset
    delay(5);
  _endSPI();

  //Comenzamos una transacción
  _startSPI();
    //Power Control B
    //  Power Control: disabled
    //  Discharge path: enabled
    _writeCommand(0xCF,0x00,0x81,0x30,-1);
    //Power on sequence control.
    //  CP1 soft start: disabled
    //  En_vlc: 1st frame enabled
    //  En_ddvdh: 4th frame enabled
    //  En_vgh: 2nd frame enabled
    //  En_vgl: 3rd frame enabled
    //  DDVDH enhanced mode: enabled
    _writeCommand(0xED,0x64,0x03,0x12,0x81,-1);
    //Driver timing control A
    //  Gate driver non-overlap timing control: default + 1 unit
    //  EQ timing control: default - 1 unit
    //  CR timing control: default - 1 unit
    //  Pre-charge timing control: default - 2 unit
    _writeCommand(0xE8,0x85,0x00,0x78,-1);
    //Power Control A
    //  vcore control: 1.6V
    //  ddvdh control: 5.6V
    _writeCommand(0xCB,0x39,0x2C,0x00,0x34,0x02,-1);
    //Pump ratio control
    //  DDVDH: 2xVCI
    _writeCommand(0xF7,0x20,-1);
    //Driver timing control B
    //  VG_SW_T1. EQ to GND :0 unit
    //  VG_SW_T2. EQ to DDVDH: 0 unit
    //  VG_SW_T3. EQ to DDVDH: 0 unit
    //  VG_SW_T4. EQ to GND: 0 unit
    _writeCommand(0xEA,0x00,0x00,-1);
    //Power control 1
    //  GVDD level: 4.60V
    _writeCommand(0xC0,0x23,-1);
    //Power control 2
    //  Factor used in the step-up circuits: DDVDH=VCI x 2, VHG=VCI x 7, VGL=-VCI x 4
    _writeCommand(0xC1,0x10,-1);
    //VCOM control 1
    //  VCOMH: 4.250V
    //  VCOML: -1.500V
    _writeCommand(0xC5,0x3E,0x28,-1);
    //VCOM control 2
    //  nVM: 0
    //  VCOMH: VMH - 58, VCOML: VMH - 58
    _writeCommand(0xC7,0x86,-1);
    //Memory Access Control
    //  Row Address Order: false
    //  Column Address Order: true
    //  Row / Column Exchange: false
    //  LCD vertical refresh direction control: false
    //  Color selector switch control: BGR color filter panel
    //  LCD horizontal refresh direction control: false
    _writeCommand(0x36,0x48,-1);
    //Vertical Scrolling Start Address: 0
    _writeCommand(0x37,0x00,0x00,-1);
    //COLMOD. Pixel Format Set
    //  RGB interface format: 16 bits/pixel
    //  MCU interface format: 16 bits/pixel
    _writeCommand(0x3A,0x55,-1);
    //Frame Control (In Normal Mode/Full Colors)
    //  DIVA. Division ratio for internal clocks when Normal Mode: fosc
    //  RTNA. Frame Rate: 79 Hz = 24 clocks per line
    _writeCommand(0xB1,0x00,0x18,-1);
    //Display Function Control
    //  Gate outputs in non-display area: Interval scan
    //  Source outputs on non-display area. Positive polarity: V63
    //  Source outputs on non-display area. Negative polarity: V0
    //  VCOM output on non-display area. Positive polarity: VCOML
    //  VCOM output on non-display area. Negative polarity: VCOMH
    //  Scan Cycle: 5 frames
    //  Frequency FLM: 85ms
    //  Liquid crystal type: Normally white
    //  Gate Output Scan Direction & Sequence: G1 -> G320
    //  Source Output Scan Direction: S1 -> S720
    //  LCD Driver Line: 320 lines
    //  External fosc: undefined
    _writeCommand(0xB6,0x08,0x82,0x27,0x00,-1);
    //3 gamma control: disabled
    _writeCommand(0xF2,0x00,-1);
    //Gamma set: gamma curve 1
    _writeCommand(0x26,0x01,-1);
    //Positive Gamma Correction
    _writeCommand(0xE0,0x0F,0x32,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00,-1);
    //Negative Gamma Correction
    _writeCommand(0xE1,0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F,-1);
  _endSPI();

  //Rotación 0: 240x320. Vertical. Conector abajo
  rotation(0);
  //Borramos el display
  clear();
  //Salimos del modo de bajo consumo
  sleep(false);
}

//Dibuja un pixel
//Devuelve true si lo consigue si el pixel es visible
bool RoJoILI9341::drawPixel(int16_t x,int16_t y,uint16_t color)
{
  //Si las coordenadas están fuera de rango...terminamos
  if(x<0 || x>=(int16_t)_xMax || y<0 || y>=(int16_t)_yMax) return false;

  _startSPI();
    _setCursorRange(x,y,x,y);
    SPI.transfer16(color);
  _endSPI();
  return true;
}

#endif
