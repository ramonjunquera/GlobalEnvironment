/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Composición personalizada de visualización
  Material adicional: breadboard, cables, 2x display 1 digit, chip 74HC595N, placa Arduino

  Resultado:
  Se mostrarán los números del 0 al 9 en el primer display y en el segundo siempre el
  símbolo de grado (º)

  Nota:
  No se utilizan los puntos decimales. Se podría eliminar el cable.
*/

#include <Arduino.h>
#include "RoJo74HC595N.h"

//Definición de pines
const byte pinData = 2;
const byte pinLatch = 3;
const byte pinClock = 4;

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines de conexión del 74HC595N
//Y el número de chips conectados en cascada (=2)
RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,2);

void setup()
{
  //Fijamos el caracter grado en el segundo display y sin punto decimal.
  display.set(1,18,0);
}

void loop()
{
  //Recorremos todos los números del 0 al 9
  for(byte i=0;i<10;i++)
  {
    //Mostramos en el primer display el número sin punto
    display.set(0,i,0);
    //Y lo mostramos
    display.show();
    //Esperamos un momento
    delay(800);
  }
}

