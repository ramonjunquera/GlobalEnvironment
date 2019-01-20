/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Averiguar qué ocurre si ocurre si lees un pin configurado como salida y que además no tiene nada conectado?
  Fecha: 20180523
  Material adicional: placa Arduino
  Descripción:
  Configuraremos el pin 5 como salida.
  Alternaremos su estado cada segundo.
  Para conocer el resultado de la lectura del pin 5, configuraremos como salida el pin 13, por tener el led integrado en
  placa y lo utilizaremos como espejo del 5, replicando su estado.
  
  Resultado:
  Aunque el pin 5 está configurado como salida, es posible leerlo. Se obtendrá el último valor escrito.
  Si conectamos un led en el pin 5 y a tierra (con su correpondiente resistencia), el resultado no varía.
  Atención. Si configuramos un pin como lectura y no conectamos nada en el, siempre seguiremos obteniendo una señal
  digital, pero será aleatoria.
*/

#include <Arduino.h>

const byte testPin = 5;
const byte ledPin = 13;

void setup()
{
  //Configuramos ambos pines como salida 
  pinMode(ledPin, OUTPUT);
  pinMode(testPin,OUTPUT);
}

void loop()
{
  //Ponemos el pin de test en estado alto
  digitalWrite(testPin, HIGH);
  //Copiamos el estado del pin de test en el 13
  digitalWrite(ledPin,digitalRead(testPin));
  //Esperamos un segundo
  delay(1000);
  //Ponemos el pin de test en estado bajo
  digitalWrite(testPin, LOW);
  //Copiamos el estado del pin de test en el 13
  digitalWrite(ledPin,digitalRead(testPin));
  //Esperamos un segundo
  delay(1000);
}
