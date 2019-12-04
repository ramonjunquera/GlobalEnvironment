/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20190924
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites
*/

#ifndef RoJoSprite_cpp
#define RoJoSprite_cpp

#include <RoJoSprite.h>

//Constructor
//Parámetro: profundidad de color en bits
//Valores permitidos: 1=monocromo, 16=color
RoJoSprite::RoJoSprite(byte colorDepth) {
  _colorDepth=colorDepth; //Guardamos la profundidad de color
  _videoMem1=NULL; _videoMem16=NULL; //Inicialmente no hay memoria de vídeo
  end(); //Reseteamos el sprite
}

//Libera el array de gráficos
void RoJoSprite::end() {
  if(_colorDepth==1) { //Monocromo
    if(_videoMem1) {
      free(_videoMem1);
      _videoMem1=NULL;
    } 
  }
  else if(_colorDepth==16) { //Color
    if(_videoMem16) {
      free(_videoMem16);
      _videoMem16=NULL;
    } 
  }
  //Ya no tiene dimensión
  _xMax=_yMax=_pageMax=_bitmapSize=0;
}

//Fija un nuevo tamaño en pixels y profundidad de color para el sprite
//Devuelve true si consigue aplicar el nuevo tamaño
bool RoJoSprite::setSize(uint16_t x,uint16_t y) {
  //Si no tiene dimensión...terminamos con error
  if(x+y==0) return false;
  //Si no tiene un profundidad de color reconocida...terminamos con error
  if(_colorDepth!=1 && _colorDepth!=16) return false;
  //Llamamos a la inicialización de la clase padre
  begin();
  //Eliminamos cualquier sprite anterior
  end();
  //Reservamos la memoria de vídeo y la vaciamos
  if(_colorDepth==1) { //Si es monocromo...
    //Calculamos el número de páginas necesarias para contener la altura en pixels
    uint32_t _pageMaxTmp=y/8;
    if(y%8) _pageMaxTmp++;
    //Calculamos el tamaño de la memoria de vídeo
    uint32_t bitmapSize=(uint32_t)x * _pageMaxTmp; //Columnas por páginas
    //Reservamos memoria
    _videoMem1=(byte*)malloc(bitmapSize);
    //Si no hemos podido...terminamos con error
    if(!_videoMem1) return false;
    //Limpiamos la memoria reservada
    memset((void*)_videoMem1,0,bitmapSize);
    //Guardamos la dimensión del sprite
    _xMax=x; //Número de columnas: anchura en pixels
    _pageMax=_pageMaxTmp; //Número de páginas
    _yMax=_pageMax*8; //Altura en pixels
    _bitmapSize=bitmapSize; //Tamaño de la memoria de vídeo en bytes
  } 
  else { //Si es color...
    //Calculamos el tamaño de la memoria de vídeo
    uint32_t bitmapSize=(uint32_t)x * (uint32_t)y * 2;
    //Reservamos memoria
    _videoMem16=(uint16_t*)malloc(bitmapSize);
    //Si no hemos podido...terminamos con error
    if(!_videoMem16) return false;
    //Limpiamos la memoria reservada
    memset((void*)_videoMem16,0,bitmapSize);
    //Guardamos los valores dimensión
    _xMax=x;
    _yMax=y;
    _bitmapSize=bitmapSize; //Tamaño de la memoria de vídeo en bytes
  } 
  //Todo Ok
  return true;
}

//Destructor
RoJoSprite::~RoJoSprite() {
	end(); //Liberamos array de gráfico
}

//Anchura en pixels
uint16_t RoJoSprite::xMax() {
  return _xMax;
}

//Altura en pixels
uint16_t RoJoSprite::yMax() {
  return _yMax;
}

//Altura en páginas
uint16_t RoJoSprite::pageMax() {
  return _pageMax;
}

//Escribe una página en sprites monocromos sin comprobaciones
//Método interno
void RoJoSprite::_drawPage(uint32_t offset,byte mask,byte color) {
  //Aplica una máscara al byte de una página
  //Dependiendo del color
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe
  
  switch(color) {
    case 0: //Desactivar pixels
      _videoMem1[offset]&=~mask;
      break;
    case 1: //Activar pixels
      _videoMem1[offset]|=mask;
      break;
    case 2: //Invertir pixels
      _videoMem1[offset]^=mask;
      break;
    //case 3: //Transparente
    case 4: //Escribe la máscara tal cual (sobreescribe)
      _videoMem1[offset]=mask;
      break;
    default: //Cualquier otra opción
      break;
  }
}

//Escribe una página en sprites monocromos
//Devuelve true si lo consigue
bool RoJoSprite::drawPage(uint16_t x,uint16_t page,byte mask,byte color) {
  //Aplica una máscara al byte de una página
  //Dependiendo del color
  //  color 0: borra
  //  color 1: dibuja
  //  color 2: invierte
  //  color 3: transparente
  //  color 4: sobrerescribe

  //Si no es un sprite monocromo...terminamos con error
  if(_colorDepth!=1) return false;
  //Si está fuera de rango...terminamos con error
  if(x>=_xMax || page>=_pageMax) return false;
  //Calculamos el offset
  uint32_t offset=(uint32_t)x+(uint32_t)page * (uint32_t)_xMax;
  //Llamamos al método privado que lo aplica
  _drawPage(offset,mask,color);
  //Todo Ok
  return true;
}

//Dibuja un pixel. Sin comprobaciones. Función interna
void RoJoSprite::_drawPixel(uint32_t x,uint32_t y,uint16_t color) {
  //Si es a color...escribimos el color del pixel
  if(_colorDepth==16) _videoMem16[x + y * (uint32_t)_xMax]=color;
  //Si es monocromo...escribimos la máscara que corresponde al color
  else _drawPage(x+(uint32_t)_xMax * (y/8),1<<(y%8),color);
}

//Dibuja un pixel
//Devuelve true si es visible
bool RoJoSprite::drawPixel(int16_t x,int16_t y,uint16_t color) {
  //Comprobamos si es visible
  displayRange r=visibleRange(x,y,1,1);
  //Si no es visible...terminamos
  if(!r.visible) return false;
  //El punto tiene coordenadas dentro de los límites del sprite
  _drawPixel(x,y,color);
  //Todo ok
  return true;
}

//Dibuja un rectángulo relleno de un color
//Devuelve true si tiene parte visible
bool RoJoSprite::block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color) {
  //Mejora de la rutina de RoJoGraph para sprites monocromos
  //En vez de escribir pixel a pixel, trabaja con páginas

  //Intercambiamos coordenadas en orden incorrecto
  if(x1>x2) {int16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {int16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Comprobamos si hay área visible
  displayRange r=visibleRange(x1,y1,x2-x1+1,y2-y1+1);
  //Si no es visible...terminamos
  if(!r.visible) return false;
  //El bloque tiene parte visible

  //Variables para acelerar los bucles
  uint32_t offsetY,_xMax32=_xMax,rx2=r.x2;
  
  //Distinguimos entre sprites color y monocromos...
  if(_colorDepth==16) { //Color...
    //Variables para acelerar los bucles
    uint32_t ry2=r.y2;
    //Recorremos todas las filas y columnas y dibujamos el pixel
    for(uint32_t y=(uint32_t)r.y1;y<=ry2;y++) {
      offsetY=y*_xMax32;
      for(uint32_t x=(uint32_t)r.x1;x<=rx2;x++) _videoMem16[offsetY+x]=color;
    }
  }
  else if(_colorDepth==1) { //Monocromo...
    //Si el color es transparente...hemos terminado
    if(color>2) return true;
    //Dibujaremos el bloque con líneas verticales
    //Calcularemos la página inicial y final de la línea
    //Cada página calculada la escribiremos en todas las columnas visibles

    //Calculamos las páginas del punto inicial y final
    byte p1=r.y1/8,p2=r.y2/8;
    //Calculamos el el offset de página inicial y final
    byte o1=r.y1%8,o2=r.y2%8;
    //Sabemos que el low-bit del byte de una página está arriba

    //Variables para acelerar los bucles
    byte pageMask;
    //Si la recta comienza y termina en la misma página...
    if(p1==p2) {
      //Calculamos la máscara de página y offset
      pageMask=(1<<(o2+1))-(1<<o1);
      offsetY=(uint32_t)p1*_xMax32;
      //Aplicamos la máscara a todas las columnas
      for(uint32_t x=r.x1;x<=rx2;x++) _drawPage(offsetY+x,pageMask,color);
    } 
    else { //La recta no comienza y termina en la misma página
      //Calculamos la máscara de la página de inicio
      pageMask=256-(1<<o1);
      offsetY=(uint32_t)p1*_xMax32;
      //La aplicamos a todas las columnas
      for(uint32_t x=r.x1;x<=rx2;x++) _drawPage(offsetY+x,pageMask,color);
      //Calculamos la máscara de la página de fin
      pageMask=(1<<(o2+1))-1;
      offsetY=(uint32_t)p2*_xMax32;
      //La aplicamos a todas las columnas
      for(uint32_t x=r.x1;x<=rx2;x++) _drawPage(offsetY+x,pageMask,color);
      //Rellenamos las páginas intermedias (si las hay)
      for(byte p=p1+1;p<p2;p++) {
        //Calculamos el offset de esta página
        offsetY=(uint32_t)p*_xMax32;
        //Aplicamos el relleno a todas las columnas
        for(uint32_t x=r.x1;x<=rx2;x++) _drawPage(offsetY+x,0xFF,color);
      }
    }
  }
  else return false; //Profundidad de color no reconocida
  //Todo Ok
  return true;
}

//Devolvemos color de un pixel. Función interna
uint16_t RoJoSprite::_getPixel(int16_t x,int16_t y) {
  uint32_t _xMax32=_xMax;
  //Si es un sprite color...devolvemos color del pixel
  if(_colorDepth==16) return _videoMem16[x + y*_xMax32];
  //Es un sprite monocromo
  return (_videoMem1[(uint32_t)x + (uint32_t)(y/8)*_xMax32] & (1<<(y%8))) > 0;
}

//Devolvemos color de un pixel
//Si seleccionamos unas coordenadas fuera de rango, devuelve 0
uint16_t RoJoSprite::getPixel(int16_t x,int16_t y) {
  //No comprobamos si el sprite está inicializado, porque en el constructor
  //se asigna un tamaño inicial de 0x0

  //Comprobamos si es visible
  displayRange r=visibleRange(x,y,1,1);
  //Si no es visible devolvemos 0
  if(!r.visible) return 0;
  //El punto tiene coordenadas dentro de los límites del sprite
  return _getPixel(x,y);
}

//Devolvemos valor de página. Sin comprobaciones. Función interna
byte RoJoSprite::_getPage(int16_t x,int16_t page) {
  return _videoMem1[(uint32_t)x+ (uint32_t)page * (uint32_t)_xMax];
}

//Devolvemos valor de página
//Sólo para sprites monocromos
//Si no es monocromo o está fuera de rango, devuelve 0
byte RoJoSprite::getPage(int16_t x,int16_t page) {
  //Si es un sprite color (que no soporta páginas) o no es visible...devolvemos 0
  if(_colorDepth==16 || x<0 || x>=(int16_t)_xMax || page<0 || page>=(int16_t)_pageMax) return 0;
  //Devolvemos válor de página
  return _getPage(x,page);
}

//Carga la información del sprite desde un archivo
//Devuelve error
byte RoJoSprite::loadSprite(String filename) {
  //Tabla de errores (primeros errores como RoJoGraph::drawSprite)
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe
  //  2 : La profundidad de color no está reconocida
  //  3 : La profundidad de color no coincide con la del display
  //  4 : No hay memoria suficiente para cargar el archivo

  //Nota:
  //Tenemos que leer un archivo con formato de sprite y cargarlo en memoria.
  //No necesitamos reposicionarlo ni calcular áreas visibles.
  //Podríamos cargar la información gráfica con RoJoGraph::drawSprite, es genérico
  //para cualquier display y muy lento.
  //Escribimos nuestra propia rutina aprovechando que la codificación de datos
  //gráficos para un sprite es la misma en memoria que en archivo.
  //El resultado es una rutina muy optimizada.

  //Definición de variables
  uint16_t width,height;
  byte colorDepth;

  //Consultamos los datos del archivo de sprite
  byte errorCode=infoSprite(filename,&width,&height,&colorDepth);
  if(errorCode) return errorCode;
  if(_colorDepth!=colorDepth) return 3;
  if(!setSize(width,height)) return 4;
  //Parámetros correctos

  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_READ); //Abrimos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"r"); //Abrimos el archivo en SPIFFS
  #endif
  //No comprobamos si ha abierto el archivo porque acabamos de usar infoSprite

  //Posicionamos el offset al inicio de los datos gráficos
  f.seek(5);

  #ifdef ESP8266
    //Desactivamos WatchDog porque vamos a leer el contenido de un archivo spr directamente
    //a memoria. Si el archivo es un poco grande, pdría tardar más que la interrupción de
    //WDT y fallar
    ESP.wdtDisable();
  #endif
  //Leemos el resto del archivo y lo guardamos en el array de la memoria de vídeo que corresponde
  if(_colorDepth==1) f.read(_videoMem1,_bitmapSize);
  else f.read((byte*)_videoMem16,_bitmapSize);
  #ifdef ESP8266
    ESP.wdtEnable(1000); //Reactivamos las interrupciones de WatchDog
  #endif
  //Cerramos el archivo
  f.close();
	//Todo ok
	return 0;
}

//Cambia los pixels de un color por otro
//Sólo para sprites color
//Devuelve true si lo consigue
bool RoJoSprite::replaceColor(uint16_t source,uint16_t destination) {
  //Si el sprite no es color...terminamos con error
  if(_colorDepth!=16) return false;
  //Si los colores son iguales...no hay que hacer nada. Todo Ok
  if(source==destination) return true;

  //Calculamos el tamaño de pixels de la memoria de vídeo
  //Será la mitad del número de bytes
  uint32_t pixels=_bitmapSize/2;
  //Recorremos todos los bytes del array gráfico...escribiendo el color en todos los pixels
  for(uint32_t p=0;p<pixels;p++) {
    //Si el color del pixel es el que buscamos...lo reemplazamos
    if(_videoMem16[p]==source) _videoMem16[p]=destination;
  }
  //Todo Ok
  return true;
}

//Copia un sprite sobre el actual, en unas coordenadas.
//Complemento a _drawSprite1 pero sin comprobaciones de visibilidad ni profundidad de color
void RoJoSprite::_drawSprite2(RoJoSprite *source,int16_t x,int16_t y,displayRange *r,uint16_t invisibleColor,bool invisible) {
  #ifdef ESP8266
    //Desactivamos WatchDog porque vamos trabajar en memoria con bucles
    //sin refrescar interrupciones de WatchDog. Así evitaremos que falle
    ESP.wdtDisable();
  #endif
  if(_colorDepth==16) { //Si es color...
    uint16_t color;
    //Variables para optimizar los bucles
    int32_t ry2=r->y2,rx1=r->x1,rx2=r->x2,offsetYSource,y32=y,x32=x;
    //Recorremos las filas visibles del sprite destino
    for(int32_t dy=(int32_t)r->y1;dy<=ry2;dy++) {
      offsetYSource=dy-y32;
      //Recorremos las columnas visibles del sprite destino
      for(int32_t dx=rx1;dx<=rx2;dx++) {
        //Obtenemos el color del pixel procesado del sprite origen
        color=source->_getPixel(dx-x32,offsetYSource);

        //Si no está activo el modo invisible o el color no coincide con el invisible...
        //Si el pixel es opaco...lo dibujamos
        if(!invisible || color!=invisibleColor) _drawPixel(dx,dy,color);
      }
    }
  }
  else { //Si es monocromo...
    //Si la coordenada vertical no es múltiplo de 8...
    if(y%8) {
      //No podemos copiar el sprite por páginas
      //Tenemos que hacerlo pixel a pixel :-(
      byte color;
      //Variables para optimizar los bucles
      int32_t ry2=r->y2,rx1=r->x1,rx2=r->x2,offsetYSource,y32=y,x32=x;
      //Recorremos las filas visibles del sprite destino
      for(int32_t dy=(int32_t)r->y1;dy<=ry2;dy++) {
        offsetYSource=dy-y32;
        //Recorremos las columnas visibles del sprite destino
        for(int32_t dx=rx1;dx<=rx2;dx++) {
          //Obtenemos el color del pixel procesado del sprite origen
          color=source->_getPixel(dx-x32,offsetYSource); 
          //Si debemos tener en cuenta transparencias...
          if(invisible) {
            //...los pixels apagados siempre serán transparente
            //Si el pixel está encendido...lo dibujamos teniendo en cuenta el 'invisibleColor'
            if(color) _drawPixel(dx,dy,invisibleColor);
          }
          //No debemos tener en cuenta las transparencias
          //Escribimos los pixels tal cual existen en la imágen original
          else _drawPixel(dx,dy,color);
        }
      }
    }
    else { //La coordenada vertical es múltiplo de 8...
      //Podemos copiar el sprite por páginas
      //Convertimos los valores verticales a páginas
      y/=8; r->y1/=8; r->y2/=8;
      //Si no debemos tener en cuenta las transparencias...sobreescribiremos
      if(!invisible) invisibleColor=4;

      //Variables para optimizar los bucles
      int16_t offsetYSource;
      int32_t offsetYDestination,_xMax32=_xMax,x32=x,y32=y,ry2=r->y2,rx1=r->x1,rx2=r->x2;
      //Recorremos las páginas visibles del sprite destino
      for(int32_t dy=r->y1;dy<=ry2;dy++) {
        offsetYSource=dy-y32;
        offsetYDestination=dy*_xMax32;
        //Recorremos las columnas visibles del sprite destino
        for(int32_t dx=rx1;dx<=rx2;dx++)
          //Obtenemos el valor de la página procesada del sprite origen
          //y lo escribimos en el sprite actual con el color/transparencia calculada
          _drawPage(offsetYDestination+dx,source->_getPage(dx-x32,offsetYSource),invisibleColor);
      }
    }
  }
  #ifdef ESP8266
    //Reactivamos las interrupciones de WatchDog
    ESP.wdtEnable(1000);
  #endif
}

//Copia un sprite sobre el actual, en unas coordenadas.
//Se puede indicar un color como invisible
//En monocromo 'invisibleColor' será el modo de escritura: 0=borrar,1=escribir,2=invertir
//Devuelve true si tiene parte visible
//Método privado
bool RoJoSprite::_drawSprite1(RoJoSprite *source,int16_t x,int16_t y,uint16_t invisibleColor,bool invisible) {
  //Si la profundidad de color es distinta en el sprite actual y origen...terminamos con error
  if(_colorDepth!=source->_colorDepth) return false;
  //Comprobamos si hay área visible
  displayRange r=visibleRange(x,y,source->_xMax,source->_yMax);
  //Si no es visible...terminamos con error
  if(!r.visible) return false;
  //El sprite tiene área visible
  _drawSprite2(source,x,y,&r,invisibleColor,invisible);
  //Todo Ok
  return true;
}

//Copia un sprite sobre otro en unas coordenadas
//Devuelve true si escribe algo
bool RoJoSprite::drawSprite(RoJoSprite *source,int16_t x,int16_t y) {
  return _drawSprite1(source,x,y);
}

//Copia un sprite sobre otro en unas coordenadas tomando un color como invisible
//En monocromo 'invisibleColor' será el modo de escritura: 0=borrar,1=escribir,2=invertir
//Devuelve true si escribe algo
bool RoJoSprite::drawSprite(RoJoSprite *source,int16_t x,int16_t y,uint16_t invisibleColor) {
  return _drawSprite1(source,x,y,invisibleColor,true);
}

//Dibuja un sprite directamente de un archivo
byte RoJoSprite::drawSprite(String filename,int16_t x,int16_t y) {
  //Método prestado de RoJoGraph
  return RoJoGraph::drawSprite(filename,x,y);
}

//Crea un sprite con el texto indicado basado en una fuente
//Se indica el color del texto y de fondo (opcional)
//Respuesta: true si lo consigue
bool RoJoSprite::print(String filenameFon,String text,uint16_t textColor,uint16_t backColor) {
  //Dimensiones del sprite
  uint16_t width,heigth;
  //Si no hemos podido calcular las dimensiones del texto...terminamos con error
  if(!infoPrint(filenameFon,text,&width,&heigth)) return false;
  //Si el sprite no tiene dimensión...terminamos con error
  if(width+heigth==0) return false;
  //Fijamos el tamaño del sprite. Si no podemos...terminamos con error
  if(!setSize(width,heigth)) return false;
  //Fijamos el color de fondo
  clear(backColor);
  //Dibujamos el texto con la función de RoJoGraph y devolvemos respuesta
  return printOver(filenameFon,text,textColor);
}

//Crea un sprite con el texto indicado con borde basado en una fuente
//Se indica el color del texto, de fondo y del borde
//Respuesta: true si lo consigue
bool RoJoSprite::print(String filenameFon,String text,uint16_t textColor,uint16_t backColor,uint16_t borderColor) {
  //Creamos el sprite que contendrá el texto sin bordes
  RoJoSprite textSprite(_colorDepth);

  if(_colorDepth==16) { //Si el sprite es a color...
    //Si no podemos crear el sprite con el texto...terminamos con error
    if(!textSprite.print(filenameFon,text,borderColor,backColor)) return false;
    //Fijamos tamaño de sprite final
    //Será 2 pixels mayor que el del texto simple en cada dimensión
    setSize(textSprite._xMax+2,textSprite._yMax+2);
    //Lo pintamos del color de fondo
    clear(backColor);
    //Copiamos el sprite de texto con el fondo transparente movido un
    //pixel en cada una de las direcciones
    drawSprite(&textSprite,2,1,backColor);
    drawSprite(&textSprite,0,1,backColor);
    drawSprite(&textSprite,1,2,backColor);
    drawSprite(&textSprite,1,0,backColor);
    //Cambiamos el color del texto del sprite por el definitivo
    textSprite.replaceColor(borderColor,textColor);
    //Copiamos el sprite de texto con el fondo transparente en
    //la posición correcta
    drawSprite(&textSprite,1,1,backColor);
  }
  else if(_colorDepth==1) { //Si el sprite es monocromo...
    //Si no podemos crear el sprite con el texto...terminamos con error
    if(!textSprite.print(filenameFon,text,1)) return false;
    //Fijamos tamaño de sprite final
    //Será 2 pixels mayor que el del texto simple en cada dimensión
    setSize(textSprite._xMax+2,textSprite._yMax+2);
    //Lo pintamos del color de fondo
    clear(backColor);
    //Copiamos el sprite de texto con el fondo transparente movido un
    //pixel en cada una de las direcciones
    drawSprite(&textSprite,2,1,borderColor);
    drawSprite(&textSprite,0,1,borderColor);
    drawSprite(&textSprite,1,2,borderColor);
    drawSprite(&textSprite,1,0,borderColor);
    //Copiamos el sprite de texto con el fondo transparente en
    //la posición correcta
    drawSprite(&textSprite,1,1,textColor);
  }
  else return false; //Profundidad de color no reconocida
  //Ya no necesitamos el sprite de texto
  textSprite.end();
  //Todo Ok
  return true;
}

//Redimensiona un sprite
void RoJoSprite::resize(RoJoSprite *source,uint16_t width,uint16_t height) {
  //Borramos el sprite actual y creamos una nuevo con el tamaño indicado
  setSize(width,height);
  //Variables para optimizar los bucles
  uint32_t sourceSizeX=source->_xMax,sourceSizeY=source->_yMax,width32=width,height32=height,offsetY;
  //Recorremos todas las filas
  for(uint32_t y=0;y<height;y++) {
    offsetY=(y*sourceSizeY)/height32;
    //Recorremos todas las columnas
    for(uint32_t x=0;x<width;x++)
      //Calculamos el color del pixel procesado y lo aplicamos
      _drawPixel(x,y,source->_getPixel((x*sourceSizeX)/width32,offsetY));
  }
}

//Guarda el sprite en un archivo
//Respuesta: true si se consigue
bool RoJoSprite::save(String filename) {
  //Si no tiene dimensión...terminamos con error
  if(_xMax*_yMax==0) return false;

  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.begin(ROJO_PIN_CS_SD);
    File f=SD.open(filename,FILE_WRITE); //Creamos el archivo en la SD
  #else //Si utilizamos SPIFFS...
    //...ya se inicializó en el constructor
    File f=SPIFFS.open(filename,"w"); //Creamos el archivo en SPIFFS
  #endif
  //Si hubo algún problema...devolvemos error
  if(!f) return false;
  //Escribimos la profundidad de color
  f.write(&_colorDepth,1);
  //Escribimos la anchura
  f.write((byte *)&_xMax,2);
  //Escribimos la altura
  f.write((byte *)&_yMax,2);
  //Escribimos el array de bitmap
  if(_colorDepth==1) f.write(_videoMem1,_bitmapSize);
  else f.write((byte *)_videoMem16,_bitmapSize);
  //Cerramos el archivo
  f.close();
  //Todo Ok
  return true;
}

//Extrae parte de un sprite creando otro
//Respuesta: true si todo es correcto
bool RoJoSprite::copy(RoJoSprite *source,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
  //Si los sprites tienen distinta profundidad de color...terminamos con error
  if(_colorDepth != source->_colorDepth) return false;
  //Intercambiamos coordenadas en orden incorrecto
  if(x1>x2) {uint16_t tmp;tmp=x1;x1=x2;x2=tmp;}
  if(y1>y2) {uint16_t tmp;tmp=y1;y1=y2;y2=tmp;}
  //Si las dimensiones están fuera de rango...devolvemos error
  if(x2>=source->_xMax || y2>=source->_yMax) return false;
  //Si no tiene dimensiones...
  if(x1+x2+y1+y2==0) {
    //...tomaremos las dimensiones del sprite
    x1=y1=0;
    x2=source->xMax()-1;
    y2=source->yMax()-1;
  }
  //Calculamos el tamaño del sprite
  uint16_t spriteWidth=x2-x1+1,spriteHeight=y2-y1+1;
  //Si no podemos redimensionar el sprite actual...devolvemos error
  if(!setSize(spriteWidth,spriteHeight)) return false;
  //Variables para optimizar los bucles
  uint16_t offsetY;
  //Copiamos el contenido del sprite origen al actual pixel a pixel
  for(uint16_t y=0;y<spriteHeight;y++) {
    offsetY=y+y1;
    for(uint16_t x=0;x<spriteWidth;x++)
      _drawPixel(x,y,source->_getPixel(x+x1,offsetY));
  }
  //Todo Ok
  return true;
}

//Devuelve la profundidad de color en bits
byte RoJoSprite::colorDepth() {
  return _colorDepth;
}

//Carga un archivo bmp en el sprite
byte RoJoSprite::loadBMP(String filename) {
  //Tabla de errores (los 8 primeros como RoJoGraph::drawBMP)
  //  0 : No hay errores. Todo correcto
  //  1 : No se puede abrir el archivo. Posiblemente no existe.
  //  2 : La firma del tipo de archivo no coincide
  //  3 : Anchura demasiado grande
  //  4 : Altura demasiado grande
  //  5 : El número de planos no es 1
  //  6 : No tiene 24 bits por pixel (no es a color)
  //  7 : Está comprimido
  //  8 : La profundidad de color no coincide con la del display
  //  9 : No hay memoria suficiente para cargar el archivo

  uint16_t width,height;
  byte colorDepth;
  byte errorCode=infoBMP(filename,&width,&height,&colorDepth);
  if(errorCode) return errorCode;
  if(_colorDepth!=colorDepth) return 8;
  if(!setSize(width,height)) return 9;
  drawBMP(filename);
  return 0; //Todo Ok
}

//Crea un nuevo sprite rotando el origen
//El parámetro angle puede valer 90, 180 o 270 grados
//Devuelve código de error:
//  0 : no hay errores
//  1 : La profundidad de color es distinta para el origen y el destino
//  2 : El ángulo de rotación no es 90, 180 ni 270 grados
//  3 : No hay memoria suficiente
byte RoJoSprite::rotate(RoJoSprite *source,uint16_t angle) {
  //Cuando se rota un sprite monocromo 90 o 270 grados, se ajusta la altura del
  //destino a páginas completas, por lo tanto puede aumentar su tamaño.
  //Por eso siempre se deben recorrer los pixels del origen, nunca del destino.
  if(_colorDepth!=source->_colorDepth) return 1;
  //Variables para optimizar los bucles
  uint16_t offset,_yMaxS=source->_yMax,_xMaxS=source->_xMax;
  switch (angle) {
    case 90:
      if(!setSize(source->_yMax,source->_xMax)) return 3;
      for(uint16_t y=0;y<_yMaxS;y++) {
        offset=_yMaxS-1-y;
        for(uint16_t x=0;x<_xMaxS;x++) _drawPixel(offset,x,source->_getPixel(x,y));
      }
      break;
    case 180:
      if(!setSize(source->_xMax,source->_yMax)) return 3;
      for(uint16_t y=0;y<_yMax;y++) {
        offset=_yMax-1-y;
        for(uint16_t x=0;x<_xMax;x++)
          _drawPixel(_xMax-1-x,offset,source->_getPixel(x,y));
      }
      break;
    case 270:
      if(!setSize(source->_yMax,source->_xMax)) return 3;
      for(uint16_t x=0;x<_xMaxS;x++) {
        offset=_xMaxS-1-x;
        for(uint16_t y=0;y<_yMaxS;y++) _drawPixel(y,offset,source->_getPixel(x,y));
      }
      break;
    default: //Ángulo desconocido
      return 2;
      break;
  }
  return 0; //Todo Ok
}

//Crea un nuevo sprite invertido horizontalmente
//Devuelve código de error:
//  0 : no hay errores
//  1 : La profundidad de color es distinta para el origen y el destino
//  2 : No hay memoria suficiente
byte RoJoSprite::flipH(RoJoSprite *source) {
  if(_colorDepth!=source->_colorDepth) return 1;
  if(!setSize(source->_xMax,source->_yMax)) return 2;
  //Rutinas separadas en función de la profundidad de color
  if(_colorDepth==16) { //Color
    //Variables para optimizar los bucles
    uint32_t _xMax32=_xMax,_yMax32=_yMax,offsetY,offsetX;
    for(uint32_t y=0;y<_yMax32;y++) {
      offsetY=y*_xMax32;
      offsetX=_xMax32-1+offsetY;
      for(uint32_t x=0;x<_xMax32;x++)
        _videoMem16[offsetX-x]=source->_videoMem16[x+offsetY];
    }
  }
  else { //Monocromo
    //Trabajamos con páginas, no con pixels. Es más rápido
    //Variables para optimizar los bucles
    uint32_t _pageMax32=_pageMax,_xMax32=_xMax,offsetY,offsetX;
    for(uint32_t y=0;y<_pageMax32;y++) {
      offsetY=y*_xMax32;
      offsetX=_xMax32-1+offsetY;
      for(uint32_t x=0;x<_xMax32;x++)
        _videoMem1[offsetX-x]=source->_videoMem1[x+offsetY];
    }
  }
  return 0; //Todo Ok  
}

//Crea un nuevo sprite invertido verticalmente
//Devuelve código de error:
//  0 : no hay errores
//  1 : La profundidad de color es distinta para el origen y el destino
//  2 : No hay memoria suficiente
byte RoJoSprite::flipV(RoJoSprite *source) {
  if(_colorDepth!=source->_colorDepth) return 1;
  if(!setSize(source->_xMax,source->_yMax)) return 2;
  //Rutinas separadas en función de la profundidad de color
  if(_colorDepth==16) { //Color
    //Variables para optimizar los bucles
    uint32_t _yMax32=_yMax,_xMax32=_xMax,offsetY0,offsetY1;
    for(uint32_t y=0;y<_yMax32;y++) {
      offsetY0=y*_xMax32; //Offset de fila en sprite origen
      offsetY1=(_yMax32-1-y)*_xMax32; //Offset de fila en sprite destino
      for(uint32_t x=0;x<_xMax32;x++)
        _videoMem16[x+offsetY1]=source->_videoMem16[x+offsetY0];
    }
  }
  else { //Monocromo
    //Trabajamos con páginas, no con pixels. Es más rápido
    //Variables para optimizar los bucles
    byte valuePage,valuePage1=0;
    uint32_t _pageMax32=_pageMax,_xMax32=_xMax,offsetY0,offsetY1;
    for(uint32_t y=0;y<_pageMax32;y++) {
      offsetY0=y*_xMax32; //Offset de fila en sprite origen
      offsetY1=(_pageMax32-1-y)*_xMax32; //Offset de fila en sprite destino
      for(uint32_t x=0;x<_xMax32;x++) {
        //Leemos el valor de la página
        valuePage=source->_videoMem1[x+offsetY0];
        //Cambiamos el orden de los bits de valuePage y guardamos el resultado en valuePage1
        for(byte b=0;b<8;b++) {
          valuePage1<<=1;
          valuePage1|=valuePage & 1;
          valuePage>>=1;
        }
        //Guardamos el valor de página en orden inverso en sprie destino
        _videoMem1[x+offsetY1]=valuePage1;
      }
    }
  }
  return 0; //Todo Ok  
}

//Borra el área de dibujo
//En monocromo el color será el modo: 0=borrar,1=pintar,2=invertir
void RoJoSprite::clear(uint16_t color) {
  #ifdef ESP8266
    //Desactivamos WatchDog porque vamos escribir en memoria con un bucle
    //sin refrescar interrupciones de WatchDog. Así evitaremos que falle
    ESP.wdtDisable();
  #endif
  //Redefinimos el método heredado de RoJoGraph para que sea más eficiente
  if(_colorDepth==16) for(int32_t i=_bitmapSize/2-1;i>=0;i--) _videoMem16[i]=color;
  else if(_colorDepth==1) for(uint32_t i=0;i<_bitmapSize;i++) _drawPage(i,0xFF,color);
  #ifdef ESP8266
    //Reactivamos las interrupciones de WatchDog
    ESP.wdtEnable(1000);
  #endif
}

#endif
