/*
  Autor: Ramón Junquera
  Tema: Timer - Ticker
  Fecha: 20180621
  Objetivo: Demo simple de uso
  Material: placa ESP8266

  Descripción:
  Un timer es un sistema para que cada cierto tiempo llame a una función determinada.
  Las placas ESP8266 tienen este concepto, pero no funciona igual que en las placas
  Arduino en las que se debe programar con registros del procesador.
  En este caso se sustituye por un simple objeto contenido en la librería Ticker.h

  Un objeto Ticker permite hacer llamadas periódicas a una determinada función.

  Se pueden crear tantos objetos Ticker como la memoria lo permita.

  En el siguiente ejemplo se muestra cómo crear un objeto Ticker y cómo asignarle
  una función.

  El programa define una función que cambia el estado del led integrado.
  Se crea un objeto Ticker y se le asigna esta función para que sea llamada cada
  segundo.

  Resultado:
  El led integrado parpadea cada segundo
*/

//Incluimos la librería de la clase Ticker
#include <Ticker.h>

//Creamos un objeto Ticker
Ticker blinker;

void setup() 
{
  //Configuramos el pin del led integrado como salida
  pinMode(BUILTIN_LED, OUTPUT);
  //El objeto blinker llamará a la función Blink() cada segundo
  blinker.attach(1,Blink);
}
void loop() 
{
  //No tenemos nada especial que hacer en la función loop()
}

void Blink()
{
  //Cambia el estado del led integrado
  digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
}
