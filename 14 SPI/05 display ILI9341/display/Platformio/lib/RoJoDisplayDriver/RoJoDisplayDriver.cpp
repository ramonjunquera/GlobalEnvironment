/*
  Nombre de la librería: RoJoILI9486.h
  Versión: 20190628
  Autor: Ramón Junquera
  Descripción:
    Gestión de display RPI_ILI9486 SPI 3.5" 320x480

  Notas:
    En este display, todos los datos que se le envían son de 16 bits.
    Si hay datos de un byte (como los comandos o sus parámetros), aún así
    se deben enviar como valor de 16 bits.
*/

#ifndef RoJoDisplayDriver_cpp
#define RoJoDisplayDriver_cpp

#include <RoJoDisplayDriver.h>

//Máxima frecuencia SPI soportada por el display (dummy)
uint32_t RoJoDisplayDriver::_maxFreqSPI()
{
  return 0;
}

// Inicia una transacción SPI
void RoJoDisplayDriver::_startSPI()
{
  SPI.beginTransaction(_spiSetting);
  digitalWrite(_pinCS,LOW);
}

// Finaliza una transacción SPI
void RoJoDisplayDriver::_endSPI()
{
  digitalWrite(_pinCS,HIGH);
  SPI.endTransaction();
}

// Envía al display un comando con sus correspondientes parámetros.
// Los parámetros son opcionales.
// Después de los parámetros, siempre hay que enviar uno adicional
// con valor negativo para indicar que no hay más.
void RoJoDisplayDriver::_writeCommand(byte command,...)
{
  //Método dummy. Es obligatorio definirlo en el driver del display. Ejemplo
  /*
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
    SPI.transfer((byte)paramValue);
  }
  //Hemos terminado de trabajar con la lista
  va_end(paramList);
  */
}

//Anchura de display (dummy)
uint16_t RoJoDisplayDriver::xMax()
{
  return 0;
}

//Altura de display (dummy)
uint16_t RoJoDisplayDriver::yMax()
{
  return 0;
}

//Define área de dibujo
//Sin gestión de SPI. No se comprueba coherencia de parámetros
void RoJoDisplayDriver::_setCursorRangeY(int16_t y1,int16_t y2)
{
  //Dummy. Ejemplo:
  //0x2B=Page Address Set
  //_writeCommand(0x2B,(byte)(y1>>8),(byte)(y1 & 0xFF),(byte)(y2>>8),(byte)(y2 & 0xFF),-1);
}
void RoJoDisplayDriver::_setCursorRangeX(int16_t x1,int16_t x2)
{
  //Dummy. Ejemplo:
  //0x2A=Column Address Set
  //_writeCommand(0x2A,(byte)(x1>>8),(byte)(x1 & 0xFF),(byte)(x2>>8),(byte)(x2 & 0xFF),-1);
  //Write memory
  //_writeCommand(0x2C,-1);
}
void RoJoDisplayDriver::_setCursorRange(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
  _setCursorRangeY(y1,y2);
  _setCursorRangeX(x1,x2);
}

//Dibuja un rectángulo relleno (dummy)
bool RoJoDisplayDriver::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  return false;
}

//Reset & inicialización (dummy)
void RoJoDisplayDriver::reset()
{
  //Ejemplo:
  //digitalWrite(_pinRES,LOW);
  //delay(20);
  //digitalWrite(_pinRES,HIGH);
  //delay(120);

  //Secuencia de comandos de inicialización
}

//Inicialización
void RoJoDisplayDriver::begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqSPI)
{
  //Llamamos a la inicialización de la clase padre
  RoJoGraph::begin();

  //Si no se ha indicado frecuencia...utilizaremos la máxima
  if(!freqSPI) freqSPI=_maxFreqSPI();
  //Definimos las caraterísticas de la conexión SPI
  _spiSetting=SPISettings(freqSPI,MSBFIRST,SPI_MODE0);
  //Inicializamos las conexiones SPI
  SPI.begin();
  //No se controlará el estado del pin CS por hardware. Lo haremos nosotros
  //Esto nos permite compartir el bus SPI con distintos dispositivos
  //En placas Arduino no es posible desactivar el pin CS por defecto
  #ifndef ARDUINO_ARCH_AVR //Si no es un Arduino...
    SPI.setHwCs(false);
  #endif

  //Guardamos los parámetros en variables internas
  _pinDC=pinDC;
  _pinRES=pinRES;
  _pinCS=pinCS;
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
}

//Dibuja un pixel (dummy)
//Devuelve true si lo consigue si el pixel es visible
bool RoJoDisplayDriver::drawPixel(int16_t x,int16_t y,uint16_t color)
{
  return false;
}

//Dibuja un archivo de sprite en unas coordenadas
//Sobreescribe la información existente
//Respuesta: true si lo consigue
bool RoJoDisplayDriver::drawSprite(int16_t x,int16_t y,String filename)
{
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Definimos las variables donde guardaremos el tamaño
  uint16_t width,height;
  //Leemos la anchura (2 bytes)
  f.read((byte *)&width,2);
  //Leemos la altura (2 bytes)
  f.read((byte *)&height,2);
  //Calculamos el área visible
  displayRange r=visibleRange(x,y,width,height);
  //Si hay área visible...
  if(r.visible)
  {
    uint16_t color;
    uint32_t offset;

    //Nota:
    //El sistema de archivos SPIFFS es interno y no interfiere en las conexiones SPI.
    //No ocurre lo mismo con un lector SD.
    //No podemos abrir comunicación con el display y sin cerrarla, comunicar con
    //el lector SD, porque finalmente fallará uno de los dos.
    //Para evitarlo, sólo abriremos conexión con el display cuando lo necesitemos.
    //Esto obliga a abrir y cerrar las comunicaciones con más frecuencia.
    //Se ha duplicado el desarrollo adaptándolo a cada caso, para evitar
    //penalizaciones contra SPIFFS

    #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
      //Seleccionamos el rango visible
      _startSPI();
        _setCursorRange(r.x1,r.y1,r.x2,r.y2);
      _endSPI();
      //Definimos los límites en int16_t para acelerar el doble bucle
      int16_t ry2=r.y2;
      int16_t rx2=r.x2;
      //Recorremos las filas visibles del display
      for(int16_t yy=r.y1;yy<=ry2;yy++)
      {
        //Calculamos y posicionamos el offset en el archivo
        //f.seek(4+2*(x-r.x1+(yy-r.y1)*width));
        offset=yy-r.y1;
        offset*=width;
        offset+=x-r.x1;
        offset*=2;
        offset+=4;
        f.seek(offset);
        //Recorremos las columnas visibles del display
        for(int16_t xx=r.x1;xx<=rx2;xx++)
        {
          //Leemos el color del archivo
          f.read((byte *)&color,2);
          //Lo enviamos al display
          _startSPI();
            SPI.transfer16(color);
          _endSPI();
        }
      }
    #else //Si utilizamos SPIFFS...
      _startSPI();
        //Seleccionamos el rango visible
        _setCursorRange(r.x1,r.y1,r.x2,r.y2);
        
        //Definimos los límites en int16_t para acelerar el doble bucle
        int16_t ry2=r.y2;
        int16_t rx2=r.x2;
        //Recorremos las filas visibles del display
        for(int16_t yy=r.y1;yy<=ry2;yy++)
        {
          //Calculamos y posicionamos el offset en el archivo
          //f.seek(4+2*(x-r.x1+(yy-r.y1)*width));
          offset=yy-r.y1;
          offset*=width;
          offset+=x-r.x1;
          offset*=2;
          offset+=4;
          f.seek(offset);
          //Recorremos las columnas visibles del display
          for(int16_t xx=r.x1;xx<=rx2;xx++)
          {
            f.read((byte *)&color,2); //Leemos el color del archivo
            SPI.transfer16(color); //Lo enviamos al display
          }
        }
      _endSPI();
    #endif
  }
  //Hemos terminado con el archivo
  f.close();
  //Todo ok
  return true;
}

//Dibuja un archivo bmp en unas coordenadas
//Sobreescribe la información existente
//Respuesta: código de error
byte RoJoDisplayDriver::drawBMP(int16_t x,int16_t y,String filename)
{
  //Tabla de errores:
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe.
  //  2 : La firma del tipo de archivo no coincide
  //  3 : Anchura demasiado grande
  //  4 : Altura demasiado grande
  //  5 : El número de planos no es 1
  //  6 : No tiene 24 bits por pixel
  //  7 : Está comprimido

  //Formato BMP
  //offset # descripción
  //------ - -----------
  //   0   2 "BM" constante
  //   2   4 tamaño de archivo
  //   6   4 reservado
  //  10   4 offset de inicio de datos gráficos
  //  14   4 tamaño de cabecera
  //  18   4 anchura en pixels
  //  22   4 altura en pixels
  //  26   2 número de planos (habitualmente 1)
  //  28   2 bits por pixel (habitualmente 24)
  //  30   4 compresión (habitualmente 0=ninguna)
  //  34   4 tamaño de imágen
  //  38   4 resolución horizontal
  //  42   4 resolución vertical
  //  46   4 tamaño de la tabla de color
  //  50   4 contador de colores importantes

  //Los valores de más de un byte están codificados en little endian (LSB)
  //La imágen se lee de abajo a arriba. El primer pixel es el de abajo a la izquierda
  //El color del pixel codificado en 24 bits también está en little endian -> BGR

  //Declaración de variables
  byte errorCode=0; //Código de error a devolver
  uint16_t width,height; //Anchura y altura
  uint32_t offsetBase; //Offset de datos gráficos. Base (primer pixel)
  byte color[3]; //Componentes de color de un pixel

  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...devolvemos error: no se puede abrir el archivo
  if(!f) return 1;
  //Si la firma de los dos primeros bytes no es "BM"...error
  if(f.read()!='B' || f.read()!='M') errorCode=2;
  else
  {
    //Comprobamos que la anchura está dentro de los límites
    f.seek(20);
    //Si la anchura es mayor de 16 bits...error
    f.read((byte *)&width,2);
    if(width!=0) errorCode=3;
    else
    {
      //Comprobamos que la anltura está dentro de los límites
      f.seek(24);
      //Si la altura es mayor de 16 bits...error
      f.read((byte *)&width,2);
      if(width!=0) errorCode=4;
      else
      {
        //Si el número de planos no es 1...error
        f.read((byte *)&width,2);
        if(width!=1) errorCode=5;
        else
        {
          //Si no tiene 24 bits por pixel...error
          f.read((byte *)&width,2);
          if(width!=24) errorCode=6;
          else
          {
            //Si está comprimido...error
            f.read((byte *)&width,2);
            f.read((byte *)&height,2);
            if(width!=0 || height!=0) errorCode=7;
            else
            {
              //Leemos la anchura
              f.seek(18);
              f.read((byte *)&width,2);
              //Leemos la altura
              f.seek(22);
              f.read((byte *)&height,2);
              //Calculamos el área visible
              displayRange r=visibleRange(x,y,width,height);
              //Si hay área visible...
              if(r.visible)
              {
                //Leemos el offset de inicio de datos gráficos
                f.seek(10);
                f.read((byte *)&offsetBase,4);
                //Convertimos algunos valores a uint32_t para no tener que hacerlo
                //continuamente en el bucle
                uint32_t x32=x,rx1=r.x1,ry1=r.y1,ry2=r.y2,width32=width;
                //El offset con el que trabajaremos será el del primer pixel de la primera línea
                offsetBase+=width32*((uint32_t)height-1)*3;

                //Nota:
                //El sistema de archivos SPIFFS es interno y no interfiere en las conexiones SPI.
                //No ocurre lo mismo con un lector SD.
                //No podemos abrir comunicación con el display y sin cerrarla, comunicar con
                //el lector SD, porque finalmente fallará uno de los dos.
                //Para evitarlo, sólo abriremos conexión con el display cuando lo necesitemos.
                //Esto obliga a abrir y cerrar las comunicaciones con más frecuencia.
                //Se ha duplicado el desarrollo adaptándolo a cada caso, para evitar
                //penalizaciones contra SPIFFS

                #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
                  //Seleccionamos el rango visible
                  _startSPI();
                    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
                  _endSPI();
                  //Recorremos las filas visibles del display
                  for(uint32_t yy=ry1;yy<=ry2;yy++)
                  {
                    //Calculamos y posicionamos el offset en el archivo
                    //f.seek(offsetBase+3*(x-r.x1-width*(yy-r.y1)));
                    f.seek(offsetBase+3*(x32-rx1-width32*(yy-ry1)));
                    //Recorremos las columnas visibles del display
                    for(uint16_t xx=r.x1;xx<=r.x2;xx++)
                    {
                      //Leemos los componentes de color
                        f.read(color,3);
                      //Lo enviamos al display
                      _startSPI();
                        //Lo codificamos en un color 565
                        //Escribimos el color en el pixel procesado
                        //El orden de lectura también es little endian -> BGR
                        SPI.transfer16(getColor(color[2],color[1],color[0]));
                      _endSPI();
                      yield();
                    }
                  }
                #else //Si utilizamos SPIFFS...
                  _startSPI();
                    //Seleccionamos el rango visible
                    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
                    //Recorremos las filas visibles del display
                    for(uint32_t yy=ry1;yy<=ry2;yy++)
                    {
                      //Calculamos y posicionamos el offset en el archivo
                      //f.seek(offsetBase+3*(x-r.x1-width*(yy-r.y1)));
                      f.seek(offsetBase+3*(x32-rx1-width32*(yy-ry1)));
                      //Recorremos las columnas visibles del display
                      for(uint16_t xx=r.x1;xx<=r.x2;xx++)
                      {
                        //Leemos los componentes de color
                        f.read(color,3);
                        //Lo codificamos en un color 565
                        //Escribimos el color en el pixel procesado
                        //El orden de lectura también es little endian -> BGR
                        SPI.transfer16(getColor(color[2],color[1],color[0]));
                      }
                    }
                  _endSPI();
                #endif
              }
            }
          }
        }
      }
    }
  }
  //hemos terminado de utilizar el archivo
  f.close();
  //Devolvemos código de error
  return errorCode;







  //Calculamos el área visible
  displayRange r=visibleRange(x,y,width,height);
  //Si hay área visible...
  if(r.visible)
  {
    uint16_t color;
    uint32_t offset;

    //Nota:
    //El sistema de archivos SPIFFS es interno y no interfiere en las conexiones SPI.
    //No ocurre lo mismo con un lector SD.
    //No podemos abrir comunicación con el display y sin cerrarla, comunicar con
    //el lector SD, porque finalmente fallará uno de los dos.
    //Para evitarlo, sólo abriremos conexión con el display cuando lo necesitemos.
    //Esto obliga a abrir y cerrar las comunicaciones con más frecuencia.
    //Se ha duplicado el desarrollo adaptándolo a cada caso, para evitar
    //penalizaciones contra SPIFFS

    #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
      //Seleccionamos el rango visible
      _startSPI();
        _setCursorRange(r.x1,r.y1,r.x2,r.y2);
      _endSPI();
      //Definimos los límites en int16_t para acelerar el doble bucle
      int16_t ry2=r.y2;
      int16_t rx2=r.x2;
      //Recorremos las filas visibles del display
      for(int16_t yy=r.y1;yy<=ry2;yy++)
      {
        //Calculamos y posicionamos el offset en el archivo
        //f.seek(4+2*(x-r.x1+(yy-r.y1)*width));
        offset=yy-r.y1;
        offset*=width;
        offset+=x-r.x1;
        offset*=2;
        offset+=4;
        f.seek(offset);
        //Recorremos las columnas visibles del display
        for(int16_t xx=r.x1;xx<=rx2;xx++)
        {
          //Leemos el color del archivo
          f.read((byte *)&color,2);
          //Lo enviamos al display
          _startSPI();
            SPI.transfer16(color);
          _endSPI();
        }
      }
    #else //Si utilizamos SPIFFS...
      _startSPI();
        //Seleccionamos el rango visible
        _setCursorRange(r.x1,r.y1,r.x2,r.y2);
        
        //Definimos los límites en int16_t para acelerar el doble bucle
        int16_t ry2=r.y2;
        int16_t rx2=r.x2;
        //Recorremos las filas visibles del display
        for(int16_t yy=r.y1;yy<=ry2;yy++)
        {
          //Calculamos y posicionamos el offset en el archivo
          //f.seek(4+2*(x-r.x1+(yy-r.y1)*width));
          offset=yy-r.y1;
          offset*=width;
          offset+=x-r.x1;
          offset*=2;
          offset+=4;
          f.seek(offset);
          //Recorremos las columnas visibles del display
          for(int16_t xx=r.x1;xx<=rx2;xx++)
          {
            f.read((byte *)&color,2); //Leemos el color del archivo
            SPI.transfer16(color); //Lo enviamos al display
          }
        }
      _endSPI();
    #endif
  }
  //Hemos terminado con el archivo
  f.close();
  //Todo ok
  return true;
}

//Dibuja un sprite en unas coordenadas
//Sobreescribe la información existente
void RoJoDisplayDriver::drawSprite(int16_t x,int16_t y,RoJoSprite16 *sprite)
{
  //Calculamos el área visible
  displayRange r=visibleRange(x,y,sprite->xMax(),sprite->yMax());
  //Si no hay área visible...hemos terminado
  if(!r.visible) return;

  _startSPI();
    //Definimos el rango del cursor
    _setCursorRange(r.x1,r.y1,r.x2,r.y2);
    //Calculamos los límites en int16_t para agilizar el doble bucle
    int16_t ry2=r.y2;
    int16_t rx2=r.x2;
    //Recorremos todas las filas visibles del display
    for(int16_t dy=(int16_t)r.y1;dy<=ry2;dy++)
    {
      //Recorremos todas las columnas visibles del sprite y enviamos el color del pixel
      for(int16_t dx=(int16_t)r.x1;dx<=rx2;dx++) SPI.transfer16(sprite->getPixel(dx-x,dy-y));
      #ifdef ESP8266
        yield(); //Refrescamos WatchDog
      #endif
    }
  _endSPI();
}

//Sincroniza dos sprites y envía las diferencias al display.
//Los sprites deben tener el mismo tamaño.
//Respuesta: true si todo es correcto
bool RoJoDisplayDriver::drawSpriteSync(int16_t x,int16_t y,RoJoSprite16 *source,RoJoSprite16 *destination)
{
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
  _startSPI();
    //Recorremos todas las filas visibles del sprite
    for(int16_t ySprite=r.y1-y;ySprite<=yMaxSprite;ySprite++)
    {
      //Por ahora no se ha inicializado el rango vertical para la fila actual
      selectedRangeY=false;
      //Comenzamos por la primera columna
      xSprite=r.x1-x;
      //Mientras no hayamos procesado todas las columnas...
      while(xSprite<=xMaxSprite)
      {
        //Si el pixel actual no se ha modificado...
        if(source->getPixel(xSprite,ySprite)==destination->getPixel(xSprite,ySprite))
        {
          //...no tenemos en cuenta este pixel. Pasaremos al siguiente
          xSprite++;
        }
        else //El pixel actual ha sido modificado...
        {
          //Si no se ha seleccionado la fila actual...
          if(!selectedRangeY)
          {
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
          while(processedColumn-lastChangedColumn<=5)
          {
            //Si el pixel de la columna procesada ha cambiado...
            if(source->getPixel(processedColumn,ySprite)!=destination->getPixel(processedColumn,ySprite))
            {
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
    }
  _endSPI();
  //Todo Ok
  return true;
}

#endif
