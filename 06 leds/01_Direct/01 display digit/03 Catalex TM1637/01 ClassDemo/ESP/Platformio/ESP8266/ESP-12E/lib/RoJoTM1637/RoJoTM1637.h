/*
  Nombre de la librería: RoJoTM1637.h
  Versión: 1.0
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión del chip TM1637 para display de leds de 4 dígitos (como el Catalex).
*/

#ifndef RoJoTM1637_h
#define RoJoTM1637_h

#include <arduino.h>
#include <stdint.h>

class RoJoTM1637
{
private:
  //Tiempo de espera de un estado en microsegundos. Mirar explicación de porqué es 3.
  const unsigned int stateDelay = 3;
  byte _pinCLK;
  byte _pinDIO;
  byte _brightness=0;
  //Están activos los dos puntos centrales que se controlan con el punto decimal del segundo dígito?
  bool _doubleDot=false; //Inicialmente no
  //Carácter mostrado en el dígito 1 (segunda posición)(la que también controla los dos puntos).
  byte _charDigit1=16; //Inicialmente un espacio (carácter 16)
  const byte digitsCount=4; //Número de dígitos en el display. 4 en nuestro caso
  void _sendStartMark(); //Envía marca de inicio de transmisión
  bool _sendData(byte value); //Envía byte de datos
  void _sendEndMark(); //Envía marca de fin de transmisión
  const byte _chars[19]={ //Definición de los caracteres que se pueden mostrar en una posición
    B00111111 // 0 = 0
   ,B00000110 // 1 = 1
   ,B01011011 // 2 = 2
   ,B01001111 // 3 = 3
   ,B01100110 // 4 = 4
   ,B01101101 // 5 = 5
   ,B01111101 // 6 = 6
   ,B00100111 // 7 = 7
   ,B01111111 // 8 = 8
   ,B01101111 // 9 = 9
   ,B01110111 //10 = A
   ,B01111100 //11 = b
   ,B00111001 //12 = C
   ,B01011110 //13 = d
   ,B01111001 //14 = E
   ,B01110001 //15 = F
   ,B00000000 //16 = space
   ,B01000000 //17 = -
   ,B01100011 //18 = º
 };
public:
  void begin(byte pinCLK, byte pinDIO); //Inicialización
  ~RoJoTM1637(); //Destructor
  bool enable(); //Activa el chip
  bool disable(); //Desactiva el chip
  bool setBrightness(byte brightness); //Fija el nivel de brillo [0,7]
  bool clear(); //Límpia el display
  bool set1DigitChar(byte digit,byte charCode); //Escribe un carácter en una posición
  bool set2DigitChar(byte digit,byte charCode1,byte charCode2); //Escribe 2 caracteres en dos posiciones consecutivas
  bool set2DigitValue(byte digit,byte value); //Escribe un valor de dos dígitos en un dos posiciones consecutivas
  bool setDoubleDot(bool status); //Indica si se deben mostrar o no los dos puntos centrales
};

#endif
