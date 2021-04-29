/*
  Autor: Ramón Junquera
  Tema: chip de extensión de pines digitales 74HC595N
  Objetivo: Obtener caracteres de keypad4x4 y caracteres especiales
  Material adicional: breadboard, cables, display 1 digit, chip 74HC595N, placa Arduino, keypad4x4

  Descripción:
  Se mostrará en el display de segmentos de led de un dígito los caracteres correspondientes
  a las pulsaciones detectadas en el teclado keypad4x4.
  Con un switch analizamos qué hacer ante cada posible pulsación.
  Si la pulsación no es válida se gestionará en el caso default y no haremos nada.
  Si el caráctere a mostrar es * los sustituiremos por º.
  Si el caracter a mostrar es #, crearemos un nuevo carácter de tres líneas horizontales.

  Nota:
  No se utiliza el punto decimal. Podría eliminarse ese cable.

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
  Se muestran los caracteres pulsados en el teclado
*/

#include <Arduino.h>
#include "RoJo74HC595N.h"
#include "RoJoKeypad4x4.h"

//Definición de pines
const byte pinData = 2;
const byte pinLatch = 3;
const byte pinClock = 4;

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines de conexión del 74HC595N
//Y el número de chips conectados en cascada (=1)
RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,1);
//Objeto de gestión. Indicamos todos los pines de conexión
RoJoKeypad4x4 pad(13,12,11,10,9,8,7,6);

void setup()
{
  //Nada especial que configurar
}

void loop()
{
  switch(pad.get())
  {
    case '0':
      display.showInt(0);
      break;
    case '1':
      display.showInt(1);
      break;
    case '2':
      display.showInt(2);
      break;
    case '3':
      display.showInt(3);
      break;
    case '4':
      display.showInt(4);
      break;
    case '5':
      display.showInt(5);
      break;
    case '6':
      display.showInt(6);
      break;
    case '7':
      display.showInt(7);
      break;
    case '8':
      display.showInt(8);
      break;
    case '9':
      display.showInt(9);
      break;
    case 'A':
      display.set(0,10,false);
      display.show();
      break;
    case 'B':
      display.set(0,11,false);
      display.show();
      break;
    case 'C':
      display.set(0,12,false);
      display.show();
      break;
    case 'D':
      display.set(0,13,false);
      display.show();
      break;
    case '*':
      //Como no podemos mostrar el símbolo *, mostramos el º
      display.set(0,18,false);
      display.show();
      break;
    case '#':
      //Como no podemos mostrar el símbolo # mostramos uno especial con 3 líneas horizontales
      display.set(0,0b01001001);
      display.show();
      break;
    default:
      break;
  }
}

