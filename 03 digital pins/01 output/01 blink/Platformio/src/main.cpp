/*
  Autor: Ramón Junquera
  Fecha: 20210119
  Tema: Gestión de pines digitales de salida
  Descripción:
    Hacemos parpadear el led integrado
    Los tiempos de pin HIGH y LOW son distintos para que podamos distinguir
    el estado del led en cada caso.
  Resultado:
    El led integrado parpadea
*/

#include <Arduino.h>

#ifdef __arm__ //Si es una Raspberri Pi
  const byte pinLed=21;
#elif defined(ARDUINO_AVR_ATTINYX5) //Si es una ATtiny85
  const byte pinLed=1;
#else //Para cualquier otro dispositivo
  const byte pinLed=LED_BUILTIN;
#endif

void setup() {
  pinMode(pinLed,OUTPUT); //Definimos el pin del led integrado como salida
}

void loop() {
  digitalWrite(pinLed,LOW); //Cambio a estado LOW
  delay(1000); //Esperamos un segundo
  digitalWrite(pinLed,HIGH); //Cambio a estado HIGH
  delay(200); //Esperamos 2 décimas de segundo
}
