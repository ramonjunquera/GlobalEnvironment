/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja
  Objetivo: Funcionamiento de una fotorresistencia infrarroja
  Material: 2 breadboards, 2 fotorresistencias IR, 2 leds IR, 2 resistencias de 10Kohmios,
     2 leds normales, cables, Arduino UNO y Arduino Nano

  Descripción:
  Basándonos en el circuito anterior, montaremos dos emisores y dos receptores en paralelo.
  Comprobaremos si efectívamente son focales y envían la luz hacia un punto sin demasiada dispersión,
  o realmente emiten en todas direcciones.
  Utilizaremos 2 protoboards para poder alejar más el receptor del emisor.
  Seguiremos manteniendo los circuitos por separado, aunque ya comprobamos que no es necesario. Pero esta
  distribución la aprovecharemos para futuros proyectos.
  Esta vez eliminamos los interruptores. Los emisores estarán encendidos contínuamente.
  Cada receptor tendrá un led normal como testigo de que está recibiendo señal.
  El objetivo es que cada emisor sólo active su led de control con la luz que le llega de su emisor.

  Para ahorrar alguna resistencia de 220 ohmios, alimentaremos los emisores con 3.3V.

  Si mantenemos mucho tiempo encendidos los emisores es posible que se calienten.

  La distancia entre receptores no debe ser muy pequeña porque sino nos costará que cada uno se active
  de manera independiente.
 
  Resultado:
  Cuando impedimos que la luz de un emisor llege a su receptor, su led de control se apaga.
  Efectívamente los emisores son focales y no tienen mucha dispersión.
*/

#include <Arduino.h>

void setup()
{
  //Los pines 4 y 5 son de salida para encender los leds de control
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop()
{
  //El estado del pin 5 será el mismo que el del pin digital 3
  digitalWrite(5,digitalRead(3));
  //El estado del pin 4 será el mismo que el del pin digital 2
  digitalWrite(4,digitalRead(2));
}
