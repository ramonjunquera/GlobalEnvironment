/*
  Autor: Ramón Junquera
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

//Declaración de variables globales
const byte pinLed=LED_BUILTIN;
const byte pinAnalog=A5;

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
  //Esperamos el valor del potenciómetro en milisegundos
  delay(analogRead(pinAnalog));
}
