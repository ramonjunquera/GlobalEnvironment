 /*
  Nombre de la librería: RoJoOLED.h para ATtiny85
  Autor: Ramón Junquera
  Fecha: 20191029
  Descripción:
    Gestión de display OLED I2C 0.96" 128x64 SSD1306
*/

#ifndef RoJoOLED_dev_h
#define RoJoOLED_dev_h

#include <Arduino.h>

class RoJoOLED {
  private:
    const byte _oledId=0x3C; //Identificador del display en en bus I2C
    void _setCursorRange(byte x1=0,byte page1=0,byte x2=127,byte page2=7);
  public:
    bool begin(); //Inicialización
    void clear(byte value=0); //Escribe el valor en todo el display
    void lineV(byte x); //Dibuja una línea vertical
}; //Punto y coma obligatorio para que no de error

#endif
