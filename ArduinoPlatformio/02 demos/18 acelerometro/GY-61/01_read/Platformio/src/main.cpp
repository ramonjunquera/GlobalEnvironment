/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro GY-61
  Objetivo: Lectura de los valores del acelerómetro
  Material: breadboard, acelerómetro GY-61, placa ESP32

  Descripción:
  Conectaremos los pines analógicos del aceleŕometro a los tres primeros
  pines de entrada analógica del ESP32.
      
  Resultado:
  Vemos el los valores de los 3 ejes del acelerómetro en tiempo real
*/

#include <Arduino.h>

//Definición de constantes globales
const byte pins[]={36,37,38}; //Corresponden a los pines A0, A1 y A2

void setup() {
  Serial.begin(115200);
}

void loop() {
  //Recorremos los 3 ejes y mostramos su valor
  for(byte i=0;i<3;i++) Serial.print(String(analogRead(pins[i]))+" ");
  Serial.println();
  //Esperamos un momento hasta repetir una nueva lectura
  delay(100);
}