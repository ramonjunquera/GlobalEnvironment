/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20190924
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites

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
      sprite (.spr) color
        offset: 0, len: 1, type: byte, info: color depth = 16
        offset: 1, len: 2, type: uint16_t, info: sprite width in pixels
        offset: 3, len: 2, type: uint16_t, info: sprite height in pixels
        offset: 5, len: 2, type: uint16_t, info: color for pixel (0,0)
        offset: 7, len: 2, type: uint16_t, info: color for pixel (1,0)
        ...
      
      sprite (.spr) monocromo
        offset: 0, len: 1, type: byte, info: color depth = 1
        offset: 1, len: 2, type: uint16_t, info: sprite width in pixels
        offset: 3, len: 2, type: uint16_t, info: sprite height in pixels
        offset: 5, len: 2, type: uint8_t, info: data for page (0,0)
        offset: 7, len: 2, type: uint8_t, info: data for page (1,0)
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

  Notas:
  - Los colores en sprites monocromos tendrán el siguiente significado:
     0: borra
     1: dibuja
     2: invierte
     3: transparente, no cambia nada
     4 o mayor: sobreescribe la página completa
  - Se evita el uso de new/delete para la gestión de memoria. Cuando se solicita la reserva de más
    memoria de la disponible en ESP, se produce una excepción, y en Arduino deja el sistema inestable.
    Con malloc/free no ocurre esto. Simplemente devuelve un puntero nulo.
  - La memoria disponible para variables no está contenida en un único bloque de bytes contiguos.
    Habitualmente está fragmentada en distintos espacios. Por lo tanto, aunque el sistema muestre
    una cantidad de memoria disponible, no se podrá reservar toda ella en una única solicitud.
*/

#ifndef RoJoSprite_h
#define RoJoSprite_h

#include <Arduino.h>
#include <RoJoGraph.h> //Funciones gráficas avanzadas

class RoJoSprite:public RoJoGraph
{
  private:
    uint16_t _xMax,_yMax,_pageMax; //Anchura y altura en pixels y número de páginas
    uint32_t _bitmapSize; //Tamaño de la memoria de vídeo en bytes
    byte *_videoMem1; //Puntero a memoria de vídeo monocromo
    uint16_t *_videoMem16; //Puntero a memoria de vídeo color
    bool _drawSprite1(RoJoSprite *source,int16_t x=0,int16_t y=0,uint16_t invisibleColor=0,bool invisible=false); //Dibuja un sprite sobre otro
    void _drawSprite2(RoJoSprite *source,int16_t x,int16_t y,displayRange *r,uint16_t invisibleColor,bool invisible); //Dibuja un sprite sobre otro
    void _drawPage(uint32_t offset,byte mask,byte color); //Escribe una página en sprites monocromos sin comprobaciones
    uint16_t _getPixel(int16_t x,int16_t y); //Devolvemos color de un pixel. Función interna
    void _drawPixel(uint32_t x,uint32_t y,uint16_t color); //Dibuja un pixel. Sin comprobaciones. Función interna
    byte _getPage(int16_t x,int16_t page); //Devolvemos valor de página. Sin comprobaciones. Función interna
  public:
    RoJoSprite(byte colorDepth=16); //Constructor
    void end(); //Libera el array de gráficos
    bool setSize(uint16_t x,uint16_t y); //Fija un nuevo tamaño
    ~RoJoSprite(); //Destructor
    uint16_t xMax(); //Anchura en pixels
    uint16_t yMax(); //Altura en pixels
    uint16_t pageMax(); //Altura en páginas en sprites monocromos
    bool drawPixel(int16_t x,int16_t y,uint16_t color); //Dibuja un pixel
    bool drawPage(uint16_t x,uint16_t page,byte mask,byte color); //Escribe una página en sprites monocromos
    bool block(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja un rectángulo relleno
    uint16_t getPixel(int16_t x,int16_t y); //Devolvemos color de pixel
    byte getPage(int16_t x,int16_t page); //Devolvemos valor de página
    bool replaceColor(uint16_t source,uint16_t destination); //Cambia los pixels de un color por otro
    bool drawSprite(RoJoSprite *source,int16_t x=0,int16_t y=0); //Dibuja un sprite en unas coordenadas
    bool drawSprite(RoJoSprite *source,int16_t x,int16_t y,uint16_t invisibleColor); //Dibuja un sprite en unas coordenadas
    byte drawSprite(String filename,int16_t x=0,int16_t y=0); //Dibuja un sprite directamente de un archivo
    bool print(String filenameFon,String text,uint16_t textColor,uint16_t backColor=0); //Crea sprite de texto
    bool print(String filenameFon,String text,uint16_t textColor,uint16_t backColor,uint16_t borderColor); //Crea un sprite de texto con borde
    void resize(RoJoSprite *source,uint16_t width,uint16_t height); //Redimensiona un sprite
    bool save(String filename); //Guarda la información del sprite en un archivo
    bool copy(RoJoSprite *source,uint16_t x1=0,uint16_t y1=0,uint16_t x2=0,uint16_t y2=0); //Extrae parte de un sprite creando otro
    byte colorDepth(); //Devuelve la profundidad de color en bits
    byte loadSprite(String filename); //Carga la información del sprite desde un archivo
    byte loadBMP(String filename); //Carga un archivo bmp en el sprite
    byte rotate(RoJoSprite *source,uint16_t angle); //Crea un nuevo sprite rotando el origen
    byte flipH(RoJoSprite *source); //Crea un nuevo sprite invertido horizontalmente
    byte flipV(RoJoSprite *source); //Crea un nuevo sprite invertido varticalmente
    void clear(uint16_t color=0); //Borra el área de dibujo
}; //Punto y coma obligatorio para que no de error

#ifdef __arm__
  #include <RoJoSprite.cpp> //Para guardar compatibilidad con RPi
#endif

#endif

