/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Demostración de librería con display de segmentos de led
  Material adicional: breadboard, cables, display 1 digit, chip 74HC595N, placa Arduino

  Descripción:
  El ejemplo muestra en el display todos los caracteres permitidos en secuencia.
  Se va alternando el punto decimal para demostar que no está ligado al carácter,

  Para evitar utilziar resistencias con los segmentos de led se conecta el chip a 3.3V en
  vez de a 5V.

  Recordemos la nomenclatura de los segmentos
        a
      ███
    █      █
  f █      █ b
    █  g   █
      ███
    █      █
  e █      █ c
    █      █
      ███     █ h
        d

  Resultado:
  Se muestran todos los caracteres posibles en el display
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

bool dot=false; //Se muestra el punto?

void setup()
{
  //Nada especial que configurar
}

void loop()
{
  //Recorremos todos los caracteres del diccionario...
  for(byte i=0;i<19;i++)
  {
    //...mostramos el dígito en el display, con el punto (si corresponde)
    display.showInt(i);
    //Cambiamos el estado del punto para el próximo ciclo
    dot=!dot;
    //Esperamos un momento
    delay(600);
  }
}

