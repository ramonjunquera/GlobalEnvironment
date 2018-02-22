/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20180222
  Autor: Ramón Junquera
  Descripción:
    Gestión de sprites color blanco y negro
  Notas:
    - Los drivers de ESP ya incluyen su propia librería SD.h. Si aparecen problemas por usar múltiples librerías con
      el mismo nombre, es conveniente eliminar la librería SD.h de Arduino al compilar para ESP.
*/

#ifndef RoJoSprite_h
#define RoJoSprite_h

//Si declaramos la siguiente línea utilizaremos una tarjeta SD como almacenamiento por defecto
//en vez de un sistema SPIFFS
#define UseSD

#include <Arduino.h>
#ifdef UseSD //Si debemos utilizar una terjeta SD...
  #define SPIFFS SD //Cuando referenciemos a SPIFFS se redireccionará a SD
  #include <SD.h> //Librería de gestión SD
#endif

class RoJoSprite
{
  private:
    byte *_bitmap; //Array de gráficos
    uint16_t _xMax; //Anchura en pixels
    uint16_t _pagesMax; //Altura en páginas (bytes)
    //Aplica una máscara al byte de una página (de la memoria de vídeo)
    void _mask(uint16_t page,uint16_t x,byte mask,byte color);
    //Intercambia los valores de dos variables enteras
    void _swap(int16_t *a,int16_t *b);
    //Dibuja linea horizontal
    void _lineH(int16_t y,int16_t x1,int16_t x2,byte color,bool withoutBorder);
    //Dibuja un rectángulo relleno
    void _rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,bool withoutBorder);
    //Pin CS de SPI cuando utilizamos una SD
    byte _pinCS_SD;
    //Frecuencia de SPI cuando utilizamos SD
    const uint32_t _freq_SD=25000000; //25MHz
  public:
    RoJoSprite(byte pinCS_SD=SS); //Constructor
    ~RoJoSprite(); //Destructor
    uint16_t width();
    uint16_t heightPages(); //Altura en páginas
    void clean(); //Libera memoria ocupada por el array de gráficos
    void setSize(uint16_t x,uint16_t pages); //Fija un nuevo tamaño para el sprite
    byte getPage(uint16_t x,uint16_t page); //Devolvemos el contenido de una columna de una página
    void drawPage(int16_t x,int16_t page,byte mask,byte color); //Guarda el byte de una columna de una página
    void drawPixel(int16_t x,int16_t y,byte color); //Dibuja pixel
    bool getPixel(int16_t x,int16_t y); //Obtiene el estado del pixel
    bool load(String fileName); //Carga la información del sprite desde un archivo
    void clear(); //Borra el sprite pintando el fondo de negro
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja una línea recta
    void lineV(int16_t x,int16_t y1,int16_t y2,byte color); //Dibuja línea vertical
    void lineH(int16_t y,int16_t x1,int16_t x2,byte color); //Dibuja línea horizontal
    void save(String fileName); //Guarda la información del sprite en un archivo
    void resize(uint16_t width,uint16_t height,RoJoSprite *source);
    void getSprite(int16_t x1,int16_t page1,int16_t x2,int16_t page2,RoJoSprite *sprite); //Crea sprite con parte del indicado
    void drawSpritePage(int16_t x,int16_t page,RoJoSprite *sprite,byte color); //Dibuja un sprite en una página y columna
    void drawSprite(int16_t x,int16_t y,RoJoSprite *sprite,byte color); //Dibuja un sprite en unas coordenadas
    void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor,byte borderColor); //Dibuja rectángulo con borde y relleno
    void rect(int16_t x1,int16_t y1,int16_t x2,int16_t y2,byte fillColor); //Dibuja un rectángulo relleno
    byte pinCS_SD(); //Devuelve el pin CS de la SD
}; //Punto y coma obligatorio para que no de error

#endif

