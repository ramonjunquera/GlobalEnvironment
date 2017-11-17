/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Demostración de librería con leds y un chip
  Material: breadboard, cables, 8 leds, chip 74HC595N, placa Arduino

  Descripción:
  Haremos el efecto Kitt con 8 leds. Para ello necesitamos un sólo chip.
  Creamos la configuración de los leds que se encenderán en binario y pedimos que
  se muestre.
  Después calculamos cuál será el siguiente led a encender.

  Resultado:
  Efecto Kitt con 8 leds
*/

#include <Arduino.h>
#include "RoJo74HC595N.h"

//Definición de pines
const byte pinData = 2;
const byte pinLatch = 3;
const byte pinClock = 4;

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines de conexión del 74HC595N
//Y el número de chips conectados en cascada (=1)
RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,1);

byte activeLed=0;
int8_t delta=1;

void setup()
{
  //Nada especial que configurar
}

void loop()
{
  //Calculamos en binario la configuración de leds que deben estar activos
  //Solo encendemos uno cada vez
  display.show(1<<activeLed);
  //Calculamos cuál será el siguiente led encendido
  activeLed+=delta;
  //Si hemos llegdo a un extremo...cambiamos de dirección
  if(activeLed==0 || activeLed==7) delta=-delta;
  //Esperamos un momento
  delay(100);
}

