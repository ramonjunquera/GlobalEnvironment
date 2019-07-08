/*
  Nombre de la librería: RoJoSprite16.h
  Versión: 20190628
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color 16bits

    El sistema de archivos donde se guardan y leen los sprites es seleccionable
    mediante la constante global del preprocesador ROJO_PIN_CS_SD
    Estas constantes se definen en el archivo platformio.ini.

    Selección del sistema de archivos.
      Si se declara la constante ROJO_PIN_CS_SD y se le asigna un valor, se
      selecciona la SD como sistema de archivos.
      El valor de ROJO_PIN_CS_SD corresponde con el pin CS de la SD.
      Si no se declara esta constante, se utilizará SPIFFS.
      Ej.: build_flags = -D ROJO_PIN_CS_SD=15

    Tabla de compatibilidad de sistemas de archivos:

            SD SPIFFS
    Arduino SI NO
    ESP     SI SI
    RPi     NO SI

    Formatos de archivo
      sprite color (.s16)
        offset: 0, len: 2, type: uint16_t, info: sprite width
        offset: 2, len: 2, type: uint16_t, info: sprite heigth
        offset: 4, len: 2, type: uint16_t, info: color for pixel (0,0)
        offset: 6, len: 2, type: uint16_t, info: color for pixel (1,0)
        ...
      fuente de texto (.fon)
        offset: 0, len: 1, type: byte, info: charMin
        offset: 1, len: 1, type: byte, info: charMax
        offset: 2, len: 1, type: byte, info: charPages
        offset: 3, len: 1, type: byte, info: charWidth for char 0 (=charMin)
        offset: 4, len: 2, type: uint16_t, info: charOffset = graphic data offset for char 0 (=charMin)
        offset: 6, len: 1, type: byte, info: charWidth for char 1
        offset: 7, len: 2, type: uint16_t, info: charOffset = graphic data offset for char 1
        ...
        offset: charOffset for char 0, len: charPages, type: byte, info: grafic data for char 0
*/

#ifndef RoJoSprite16_cpp
#define RoJoSprite16_cpp

#include <RoJoSprite16.h>

//Constructor
RoJoSprite16::RoJoSprite16()
{
  //Inicialmente el tamaño es nulo
	_xMax=_yMax=0;
  _bitmapMem=NULL;
}

//Libera el array de gráficos
void RoJoSprite16::end()
{
  //Si tiene memoria reservada...la liberamos
  if(_bitmapMem) delete[] _bitmapMem;
  //Ya no tiene memoria reservada
  _bitmapMem=NULL;
  //Ya no tiene dimensión
  _xMax=_yMax=0;
}

//Fija un nuevo tamaño para el sprite
//Respuesta: true si se consigue redimensionar
bool RoJoSprite16::setSize(uint16_t x,uint16_t y)
{
  //Llamamos a la inicialización de la clase padre
  //Lo hacemos en este método porque RoJoSprite16 no tiene método begin
  RoJoGraph::begin();
  //Si no tiene dimensión...terminamos con error
  if(x*y==0) return false;
  //Eliminamos cualquier sprite anterior
  end();
  //Calculamos el tamaño del bitmap en pixels
  uint32_t bitmapSize=(uint32_t)x * (uint32_t)y;
  //Creamos un nuevo array gráfico lleno de ceros
  _bitmapMem = new uint16_t[bitmapSize]();
  //Si no ha funcionado...terminamos con error
  if(!_bitmapMem) return false;
  //Guardamos los valores dimensión desde los parámetros
  _xMax=x;
  _yMax=y;
  //Todo Ok
  return true;
}

//Destructor
RoJoSprite16::~RoJoSprite16()
{
	end(); //Liberamos array de gráfico
}

//Anchura del sprite
uint16_t RoJoSprite16::xMax()
{
  return _xMax;
}

//Altura del sprite
uint16_t RoJoSprite16::yMax()
{
  return _yMax;
}

//Dibuja un pixel
//Devuelve true si lo consigue si el pixel es visible
bool RoJoSprite16::drawPixel(int16_t x,int16_t y,uint16_t color)
{
  //Comprobamos si es visible
  displayRange r=visibleRange(x,y,1,1);
  //Si no es visible...terminamos
  if(!r.visible) return false;
  //El punto tiene coordenadas dentro de los límites del sprite

  //Calculamos la posición del pixel
  uint32_t offset=(uint32_t)x + (uint32_t)y * (uint32_t)_xMax;
  //Guardamos color
  _bitmapMem[offset]=color;
  //Todo ok
  return true;
}

//Dibuja un rectángulo relleno de un color
//Devuelve true si tiene parte visible
bool RoJoSprite16::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color)
{
  //Intercambiamos coordenadas erróneas
  if(x1>x2) {int16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {int16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Comprobamos si hay área visible
  displayRange r=visibleRange(x1,y1,x2-x1+1,y2-y1+1);
  //Si no es visible...terminamos
  if(!r.visible) return false;
  //Definimos los límites máximos en uint16_t para acerelar el doble bucle
  int16_t ry2=r.y2;
  int16_t rx2=r.x2;
  //Llenamos el área visible del bloque
  for(int16_t y=r.y1;y<=ry2;y++)
    for(int16_t x=r.x1;x<=rx2;x++)
      drawPixel(x,y,color);
  //Tiene parte visible
  return true;
}

//Devolvemos color de un pixel
//Si seleccionamos unas coordenadas fuera de rango, devuelve 0
uint16_t RoJoSprite16::getPixel(int16_t x,int16_t y)
{
  //No comprobamos si el sprite está inicializado, porque en el constructor
  //se asigna un tamaño inicial de 0x0

  //Comprobamos si es visible
  displayRange r=visibleRange(x,y,1,1);
  //Si no es visible devolvemos 0
  if(!r.visible) return 0;
  //El punto tiene coordenadas dentro de los límites del sprite

  //Calculamos la posición del pixel
  uint32_t offset=(uint32_t)x + (uint32_t)y * (uint32_t)_xMax;
  //Devolvemos el color de la posición
  return _bitmapMem[offset];
}

//Carga la información del sprite desde un archivo
//Devuelve true si lo consigue
bool RoJoSprite16::load(String fileName)
{
  end(); //Eliminamos cualquier sprite anterior
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(fileName,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(fileName,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Definimos las variables donde guardaremos el tamaño
  uint16_t width,height;
  //Leemos la anchura (2 bytes)
  f.read((byte *)&width,2);
  //Leemos la altura (2 bytes)
  f.read((byte *)&height,2);
  //Si no hemos podido fijar el tamaño del sprite...
  if(!setSize(width,height))
  {
    //...cerramos el archivo a leer
    f.close();
    //Devolvemos error
    return false;
  } 
  //Calculamos el tamaño del bitmap en bytes
  uint32_t bitmapSize=(uint32_t)width * (uint32_t)height *2;
  //Leemos el resto del archivo y lo guardamos en el array bitmap
  f.read((byte *)_bitmapMem,bitmapSize);
  //Cerramos el archivo
  f.close();
	//Todo ok
	return true;
}

//Carga la información del sprite desde un archivo
//Devuelve el código de error
byte RoJoSprite16::loadBMP(String fileName)
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
  //  8 : No hay memoria suficiente

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
  uint32_t offset; //Offset de datos gráficos
  byte color[3]; //Componentes de color de un pixel

  end(); //Eliminamos cualquier sprite anterior
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(fileName,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(fileName,"r"); //Abrimos el archivo en SPIFFS
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
              //Leemos el offset de inicio de detos gráficos y nos posicionamos en él
              f.seek(10);
              f.read((byte *)&offset,4);
              f.seek(offset);
              //Si no hemos podido fijar el tamaño del sprite...error
              if(!setSize(width,height)) errorCode=8;
              else
              {
                //Recorreremos todos los pixels de la imagen
                //Recordemos que en BMP la primera línea es la inferior
                for(int32_t y=height;y>=0;y--)
                  for(uint16_t x=0;x<width;x++)
                  {
                    //Leemos los componentes de color
                    f.read(color,3);
                    //Lo codificamos en un color 565
                    //Escribimos el color en el pixel procesado
                    //El orden de lectura también es little endian -> BGR
                    drawPixel(x,y,getColor(color[2],color[1],color[0]));
                  }
              }
            }
          }
        }
      }
    }
  }
  //hemos terminado de utilizar el archivo
  f.close();
  //Si tenemos algún error...liberamos memoria del sprite
  if(errorCode) end();
  //Devolvemos código de error
  return errorCode;
}

//Cambia los pixels de un color por otro
void RoJoSprite16::replaceColor(uint16_t source,uint16_t destination)
{
  //Calculamos el tamaño del bitmap en pixels
  uint32_t bitmapSize=(uint32_t)_xMax * (uint32_t)_yMax;
  //Recorremos todos los bytes del array gráfico...escribiendo el color en todos los pixels
  for(uint32_t p=0;p<bitmapSize;p++)
  {
    //Si el color del pixel es el que buscamos...lo reemplazamos
    if(_bitmapMem[p]==source) _bitmapMem[p]=destination;
  }
}

//Copia un sprite sobre el actual, en unas coordenadas.
//Se indica qué color se debe tomar como invisible y si se debe tener en cuenta.
//Método privado
void RoJoSprite16::_drawSprite(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor,bool invisible)
{
  //Comprobamos si hay área visible
  displayRange r=visibleRange(x,y,source->_xMax,source->_yMax);
  //Si no es visible...terminamos
  if(!r.visible) return;
  //El sprite tiene área visible

  uint16_t color;
  //Anotamos las coordenadas máximas en int16_t para acelerar el doble bucle
  int16_t ry2=r.y2;
  int16_t rx2=r.x2;
  //Recorremos las filas visibles del sprite destino
  for(int16_t dy=(int16_t)r.y1;dy<=ry2;dy++)
  {
    //Recorremos las columnas visibles del sprite destino
    for(int16_t dx=(int16_t)r.x1;dx<=rx2;dx++)
    {
      //Obtenemos el color del pixel procesado del sprite origen
      color=source->getPixel(dx-x,dy-y);
      //Si no está activo el modo invisible o el color no coincide con el invisible...
      //Si el pixel es opaco...lo dibujamos
      if(!invisible || color!=invisibleColor) drawPixel(dx,dy,color);
    }
  }
}

//Copia un sprite sobre otro en unas coordenadas
void RoJoSprite16::drawSprite(int16_t x,int16_t y,RoJoSprite16 *source)
{
  _drawSprite(x,y,source);
}

//Copia un sprite sobre otro en unas coordenadas tomando un color como invisible
void RoJoSprite16::drawSprite(int16_t x,int16_t y,RoJoSprite16 *source,uint16_t invisibleColor)
{
  _drawSprite(x,y,source,invisibleColor,true);
}

//Crea un sprite con el texto indicado basado en una fuente
//Se indica el color del texto y de fondo (opcional)
//Respuesta: true si lo consigue
bool RoJoSprite16::print(String fileNameFon,String text,uint16_t textColor,uint16_t backColor)
{
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(fileNameFon,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(fileNameFon,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...hemos terminado
  if(!f) return false;
  //Definimos variables de trabajo
  byte charMin; //Primer carácter ASCII incluido en librería
  byte charMax; //Último carácter ASCII incluido en librería
  byte charCurrent; //Carácter procesado
  byte charWidth; //Anchura de carácter procesado
  byte charPages=0; //Altura en páginas
  //Leemos ASCII del primer carácter de la fuente
  f.readBytes((char *)&charMin,1);
  //Leemos ASCII del último carácter de la fuente
  f.readBytes((char *)&charMax,1);
  //Leemos el número de páginas
  f.readBytes((char *)&charPages,1);

  //Calculamos la anchura del sprite = suma de la anchura de todos los
  //caracteres y espacios separadores
  //Inicialmente la anchura del sprite es 0
  uint16_t spriteWidth=0;
  //Recorremos todos los caracteres del texto
  for(uint16_t i=0;i<text.length();i++)
  {
    //Extraemos el carácter procesado
    charCurrent=text[i];
    //Si el carácter pertenece a los caracteres de la fuente...
    //(si no pertenece a la fuente, no lo tenemos en cuenta)
    if(charCurrent>=charMin && charCurrent<=charMax)
    {
      //...este carácter será representado
      //Si este no es el primer carácter, añadiremos una
      //columna de separación.
      if(spriteWidth>0) spriteWidth++;
      //Obtenemos la anchura del carácter
      f.seek(3+3*(charCurrent-charMin));
      f.read((byte *)&charWidth,1);
      //Añadimos la anchura del carácter
      spriteWidth+=charWidth;
    }
  }
  //Tenemos calculada la anchura del sprite en spriteWidth

  //Hemos terminado de utilizar el archivo de fuentes
  f.close();
  
  //Si el sprite no tiene anchura...terminamos con error
  if(spriteWidth==0) return false;
  //El sprite tiene anchura!

  //Fijamos el tamaño del sprite
  setSize(spriteWidth,charPages*8);
  //Fijamos el color de fondo
  clear(backColor);
  //Dibujamos el texto con la función de RoJoGraph y devolvemos respuesta
  return RoJoGraph::print(0,0,fileNameFon,text,textColor);
}

//Crea un sprite con el texto indicado con borde basado en una fuente
//Se indica el color del texto, de fondo y del borde
//Respuesta: true si lo consigue
bool RoJoSprite16::print(String fileNameFon,String text,uint16_t textColor,uint16_t backColor,uint16_t borderColor)
{
  //Crea un sprite de texto con borde
  //Devuelve false ante cualquier error

  //Creamos el sprite que contendrá el texto sin bordes
  RoJoSprite16 textSprite;
  //Si no podemos crear el sprite con el texto...terminamos con error
  if(!textSprite.print(fileNameFon,text,borderColor,backColor)) return false;
  //Fijamos tamaño de sprite final
  //Será 2 pixels mayor que el del texto simple en cada dimensión
  setSize(textSprite._xMax+2,textSprite._yMax+2);
  //Lo pintamos del color de fondo
  clear(backColor);

  //Copiamos el sprite de texto con el fondo transparente movido un
  //pixel en cada una de las direcciones
  drawSprite(2,1,&textSprite,backColor);
  drawSprite(0,1,&textSprite,backColor);
  drawSprite(1,2,&textSprite,backColor);
  drawSprite(1,0,&textSprite,backColor);
  //Cambiamos el color del texto del sprite por el definitivo
  textSprite.replaceColor(borderColor,textColor);
  //Copiamos el sprite de texto con el fondo transparente en
  //la posición correcta
  drawSprite(1,1,&textSprite,backColor);
  //Hemos terminado de utilizar el sprite de texto simple
  textSprite.end();
  //Todo correcto
  return true;
}

//Redimensiona un sprite
void RoJoSprite16::resize(uint16_t width,uint16_t height,RoJoSprite16 *source)
{
  //Borramos el sprite actual y creamos una nuevo con el tamaño indicado
  setSize(width,height);
  //Anotamos las dimensiones del sprite origen para facilitar la nomenclatura
  uint32_t sourceSizeX=source->_xMax;
  uint32_t sourceSizeY=source->_yMax;
  //Recorremos todas las filas
  for(uint32_t y=0;y<height;y++)
  {
    //Recorremos todas las columnas
    for(uint32_t x=0;x<width;x++)
    {
      //Calculamos el color del pixel procesado y lo aplicamos
      drawPixel(x,y,source->getPixel((x*sourceSizeX)/_xMax,(y*sourceSizeY)/_yMax));
    }
  }
}

//Guarda el sprite en un archivo
//Respuesta: true si se consigue
bool RoJoSprite16::save(String fileName)
{
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(fileName,FILE_WRITE); //Creamos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(fileName,"w"); //Creamos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Calculamos el tamaño del bitmap en bytes
  uint32_t bitmapSize=(uint32_t)_xMax * (uint32_t)_yMax *2;
  //Escribimos la anchura
  f.write((byte *)&_xMax,2);
  //Escribimos la altura
  f.write((byte *)&_yMax,2);
  //Escribimos el array de bitmap
  f.write((byte *)_bitmapMem,bitmapSize);
  //Cerramos el archivo
  f.close();
  //Todo Ok
  return true;
}

//Copia un sprite en otro, total o parcialmente
//Respuesta: true si todo es correcto
bool RoJoSprite16::copy(RoJoSprite16 *source,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
  //Si las dimensiones están mal...devolvemos error
  if(x1>x2 || y1>y2 || x2>=source->_xMax || y2>=source->_yMax) return false;
  //Si no tiene dimensiones...
  if(x1+x2+y1+y2==0)
  {
    //...tomaremos las dimensiones del sprite
    x1=y1=0;
    x2=source->xMax()-1;
    y2=source->yMax()-1;
  }
  //Calculamos el tamaño del sprite
  uint16_t spriteWidth=x2-x1+1;
  uint16_t spriteHeight=y2-y1+1;
  //Borramos el sprite actual
  end();
  //Si no podemos aplicarle el tamaño...devolvemos error
  if(!setSize(spriteWidth,spriteHeight)) return false;
  //Copiamos el contenido del sprite origen al actual
  for(uint16_t y=0;y<spriteHeight;y++)
    for(uint16_t x=0;x<spriteWidth;x++)
      drawPixel(x,y,source->getPixel(x+x1,y+y1));
  //Todo Ok
  return true;
}

#endif
