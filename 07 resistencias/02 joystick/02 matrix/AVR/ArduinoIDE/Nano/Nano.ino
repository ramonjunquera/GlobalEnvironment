/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180319
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Ahora que hemos aprendido a leer los valores del joystick, utilizaremos una matriz de leds
  monocromos de 8x8 gestionada por un chip MAX7219, para representar la posición.

  En este ejemplo no utilizaremos la función map para convertir el valor leido a uno en un rango entre
  0 y 7. La razón es que la función debería ser x=map(x,0,1023,0,7)
  Con esto la x sólo tomaría el valor 7 con 1023. Esto es demasiado cerrado.
  Utilizamos algo más simple.
  Si el rango de valores de entrada es de 1024 y queremos reducirlo a 8 posibilidades, dividimos:
  1024/8=128. Así que si dividimos el valor de entrada por 128, estaremos dando 128 valores distintos a cada
  valor de salida. Esto es bastante más flexible.

  En el programa utilizaremos el botón para decidir si se enciende el led que marca la posición en la matrix.
  Si pulsamos el botón, desaparece.
  
  Resultado:
  Vemos los valores leidos del joystick en pantalla en tiempo real
*/

#include <Arduino.h>
#include "RoJoMAX7219SD.h" //Librería de gestión de MAX7219

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;

//Definición de pines
const byte pinButton=5;
const byte pinDIN_display=4;
const byte pinCS_display=3;
const byte pinCLK_display=2;

void setup()
{
  //Configuramos el pin del botón del joy como entrada con resistencias de pullup activas
  pinMode(pinButton,INPUT_PULLUP);
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);
}

void loop()
{
  //Leemos la entrada analógica para x
  int x=analogRead(A0);
  //Leemos la entrada analógica para y
  int y=analogRead(A1);
  //Leemos el estado del pulsador. 1=suelto, 0=presionado
  bool b=digitalRead(pinButton);

  //Convertimos las coordenadas a un valor entre 0 y 7
  x/=128;
  y/=128;
  //Borramos la memoria de video
  display.videoMem->clear();
  //Si el botón no está pulsado...activamos el led en las coordenadas calculadas del joystick
  if(b) display.videoMem->drawPixel(x,y,1);
  //Mostramos la memoria de vídeo en la matriz
  display.show();
  //Esperamos un momento
  delay(50);
}
