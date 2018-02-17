/*
  Nombre de la librería: RoJoSprite.h
  Versión: 20180211
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
  public:
    static byte pinCS_SPI; //Pin CS de SPI cuando utilizamos una SD
    static uint32_t freq_SPI; //Frecuencia de bus SPI cuando utilizamos una SD
    RoJoSprite(); //Constructor
    ~RoJoSprite(); //Destructor
    uint16_t width();
    uint16_t heightPages(); //Altura en páginas
    void clean(); //Libera memoria ocupada por el array de gráficos
    void setSize(uint16_t x,uint16_t pages); //Fija un nuevo tamaño para el sprite
    byte getPage(uint16_t x,uint16_t page); //Devolvemos el contenido de una columnas de una página
    void setPage(int16_t x,int16_t page,byte mask,byte color); //Guarda el byte de una columna de una página
    void setPixel(int16_t x,int16_t y,byte color); //Dibuja pixel
    bool getPixel(int16_t x,int16_t y); //Obtiene el estado del pixel
    bool load(String fileName); //Carga la información del sprite desde un archivo
    void clear(); //Borra el sprite pintando el fondo de negro
    void line(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t color); //Dibuja una línea recta
    void save(String fileName); //Guarda la información del sprite en un archivo
    void resize(uint16_t width,uint16_t height,RoJoSprite *source);
}; //Punto y coma obligatorio para que no de error

#endif

