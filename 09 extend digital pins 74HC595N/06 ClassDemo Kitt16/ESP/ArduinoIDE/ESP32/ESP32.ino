/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Demostración de librería con leds y dos chips
  Material adicional: breadboard, cables, 16 leds, 2x chip 74HC595N, placa ESP32

  Descripción:
  Haremos el efecto Kitt con 16 leds. Para ello necesitamos dos chips encadenados.
  Creamos la configuración de los leds que se encenderán en binario y pedimos que
  se muestre.
  Después calculamos cuál será el siguiente led a encender.

  Resultado:
  Efecto Kitt con 16 leds
*/

#include <Arduino.h>
#include "RoJo74HC595N.h"

//Definición de pines
const byte pinData = 5;
const byte pinLatch = 19;
const byte pinClock = 21;

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines de conexión del 74HC595N
//Y el número de chips conectados en cascada (=2)
RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,2);

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
  if(activeLed==0 || activeLed==15) delta=-delta;
  //Esperamos un momento
  delay(100);
}

