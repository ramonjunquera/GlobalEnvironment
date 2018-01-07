/*
  Autor: Ramón Junquera
  Tema: Sensor de movimiento PIR
  Objetivo: Aprender a utilizar el sensor de movimiento PIR
  Material: sensor PIR, cables, ESP32

  Descripción:
  El ejemplo sólo copia el estado de la señal del sensor al pin del led.
  Utilizamos el led integrado en placa para poder visualizar el estado del pin del PIR.

  Resultado:
  El led se enciende cada vez que detecta movimiento en su campo de acción.
*/

#include <Arduino.h>

//Definión de pines
const byte pinLed=LED_BUILTIN;
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
  //Recordemos que el led integrado tiene los estados invertidos
  digitalWrite(pinLed,!digitalRead(pinPIR));
}
