/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Demostración de librería con dos displays de segmentos de led
  Material adicional: breadboard, cables, 2x display 1 digit, chip 74HC595N, placa Arduino

  Descripción:
  Se mostrarán en los displays los números entre -9 y 99.
  Se demuestra que el método showInt funciona también con números negativos.

  Resultado:
  Se muestran los números del -9 a 99

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
  //Nada especial que configurar
}

void loop()
{
  //Recorremos todos los números entre el -9 y el 99
  for(int16_t i=-9;i<100;i++)
  {
    //...mostramos el dígito en los displays
    display.showInt(i);
    //Esperamos un momento
    delay(200);
  }
}

