/*
  Tema: Librería RMT
  Objetivo: Receptor de pulso simple sin RMT
  Fecha: 20220609
  Autor: Ramón Junquera

  Descripción:
    Muestra por puerto serie cualquier cambio de estado del pin al que hemos conectado
    el receptor laser.
    Recordar que el receptor entrega una señal HIGH cuando NO recibe luz (está invertido).
  Resultado:
    Podemos ver como se detectan los pulsos.
    Los estados están invertidos debido al receptor laser: 0=LUZ, 1=SIN LUZ
    Gracias a que mostramos el tiempo en ms comprobamos que el pulso dura exactamente 100ms=0.1s
*/

#include <Arduino.h>

#ifdef ARDUINO_AVR_UNO
  const byte pinLed=2;
#elif defined(ESP32)
  const byte pinLed=32;
#endif
bool lastStatus=true; //Suponemos que inicialmente no hay luz

void setup() {
  Serial.begin(115200);
  pinMode(pinLed,INPUT); //No son necesarias resistencias PULL
}

void loop() {
  if(digitalRead(pinLed)!=lastStatus) { //Si cambia de estado
    lastStatus=!lastStatus; //Cambiamos el estado anterior
    Serial.println(String(millis())+" : "+String(lastStatus));
  }
}