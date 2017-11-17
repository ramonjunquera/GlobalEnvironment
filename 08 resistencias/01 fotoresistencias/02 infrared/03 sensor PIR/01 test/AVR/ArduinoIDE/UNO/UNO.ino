/*
  Autor: Ramón Junquera
  Tema: Sensor de movimiento PIR
  Objetivo: Aprender a utilizar el sensor de movimiento PIR
  Material: sensor PIR, cables, led, Arduino UNO

  Descripción:
  El ejemplo sólo copia el estado de la señal del sensor al pin del led.
  
  Utilizamos el 13 para poder poner directamente un led en la placa Arduino para que se vea mejor.

  Resultado:
  El led se enciende cada vez que detecta movimiento en su campo de acción.
*/

#include <Arduino.h>

//Definión de pines
const byte pinLed=13;
const byte pinPIR=2;

void setup()
{
  //El pin del led es de salida
  pinMode(pinLed,OUTPUT);
  //El pin del sensor es de entrada
  pinMode(pinPIR,INPUT);
}

void loop()
{
  //Pasamos directamente el estado de la señal del sensor el pin del led
  digitalWrite(pinLed,digitalRead(pinPIR));
}
