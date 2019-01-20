/*
  Autor: Ramón Junquera
  Tema: Display de segmentos de led de un dígito
  Objetivo: Demostración del uso de la librería
  Material adicional: breadboard, cables, display 1 digit, resistencia de 220 ohmios, placa Arduino UNO

  Descripción:
  La librería RoJoDisplayDigit1 es muy simple.
  Sólo tiene un constructor en el que se definen los pines de los segmentos y un método para mostrar uno
  de los caracteres permitidos.

  El ejemplo muestra en el display todos los caracteres permitidos en secuencia cada segundo.
  Se va alternando el punto decimal para demostar que no está ligado al carácter,

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
#include "RoJoDisplayDigit1.h"

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines a los que hemos conectado cada segmento
RoJoDisplayDigit1 display = RoJoDisplayDigit1(13,12,11,10,9,8,7,6);

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
    display.set(i,dot);
    //Cambiamos el estado del punto para el próximo ciclo
    dot=!dot;
    //Esperamos un momento
    delay(1000);
  }
}
