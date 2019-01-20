/*
  Autor: Ramón Junquera
  Fecha: 20180603
  Tema: ESP32 Touch pins
  Objetivo: Lectura de valores de pines touch
  Material adicional: placa ESP32
  Descripción:
    Leeremos continuamente el valor del pin Touch0 (pin 4) y 
    enviaremos su valor por el puerto serie.
*/

#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Started!");
}

void loop()
{
  Serial.println(touchRead(T0));
  delay(1000);
}
