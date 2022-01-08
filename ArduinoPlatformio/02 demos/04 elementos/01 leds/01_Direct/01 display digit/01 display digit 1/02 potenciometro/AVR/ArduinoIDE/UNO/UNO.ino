/*
  Autor: Ramón Junquera
  Tema: Display de segmentos de led de un dígito
  Objetivo: Demostración de uso de librería
  Material adicional: breadboard, cables, display 1 digit, resistencia de 220 ohmios,
    potenciómetro de 10 Kohmios, Arduino UNO
    
  Resultado:
  Mostraremos el valor correspondiente a la intensidad del potenciómetro en el display
  de segmentos de un dígito.

  Nota:
  En este ejercicio no utilizamos el punto decimal, por eso no se ha cableado.
*/

#include <Arduino.h>
#include "RoJoDisplayDigit1.h"

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines a los que hemos conectado cada segmento
RoJoDisplayDigit1 display = RoJoDisplayDigit1(13,12,11,10,9,8,7,6);

const byte pinPot=A5; //Pin del potenciómetro

void setup()
{
  //Nada especial que configurar
}

void loop()
{
  //Mostramos en el display el dígito que corresponde a la intensidad del potenciómetro
  //Puesto que las señales analogicas tienen una resolución de 10 bits = 1024 valores.
  //Si sólo tenemos 10 dígitos a mostrar tendremos que dividor el rango en 10 secciones.
  //Esta es la razón por la que dividimos la señal analógica por 102.4
  display.set(analogRead(pinPot)/102.4,false);
}
