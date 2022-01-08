/*
  Autor: Ramón Junquera
  Fecha: 20191029
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las lecturas analógicas
  Material: breadboard, potenciómetro de 10 Kohmios, cables, placa Arduino
  
  Descripción:
  Haremos una nueva versión del proyecto blink.
  Esta vez utilizaremos un potenciómetro que regulará la velocidad de parpadeo de un led.
    
  Resultado:
  Al girar el potenciómetro controlaremos la velocidad de parpadeo del led integrado
*/

#include <Arduino.h>

//Declaración de constantes globales
#if defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_DIGISPARK) //ATtiny85
  const byte pinLed=1;
#else //Resto de placas Arduino
  const byte pinLed=13;
#endif
const byte pinAnalog=A1;

void setup() {
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
}

void loop() {
  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
  //Esperamos el valor del potenciómetro en milisegundos
  delay(analogRead(pinAnalog));
}
