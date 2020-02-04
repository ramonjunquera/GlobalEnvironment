/*
  Autor: Ramón Junquera
  Fecha: 20200201
  Tema: M5Stick C. ToF HAT (sensor de distancia láser)
  Objetivo: Ejemplo de obtención de datos
  Material: M5Strick C, M5Stick ToF HAT

  Descripción:
    Leeremos los datos del sensor y los mostraremos por el puerto serie.
*/

#include <Arduino.h>
#include <Wire.h>
#include <RoJoM5ToFHAT.h> //Gestión de sensor ToF

//Definición de objetos globales
RoJoM5ToFHAT tof;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW); //Encendemos el led integrado
  delay(2000);
  if(tof.begin()) Serial.println("ToF HAT inicializado correctamente");
  else Serial.println("No se puede inicializar ToF HAT");
}

void loop() {
  uint16_t d;
  while((d=tof.get())==0); //Intentaremos conseguir una medida válida
  Serial.println("distance="+String(d));
  delay(100);
}
