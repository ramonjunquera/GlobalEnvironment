/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: breadboard, led, resistencia de 220 ohmios, placa Arduino Mega

  Descripción:
  Haremos variar la intensidad del led integrado progresivamente desde apagado
  hasta el máximo y después a la inversa.

  El led integrado de Arduino Mega también está asociado al pin 13, como en
  Arduino UNO. La diferencia está en que ahora este pin tiene capacidades
  PWM. No necesitamos uno externo para hacer el ejercicio.
  
  Resultado:
  El led integrado se enciende y apaga progresivamente
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = LED_BUILTIN; //pin 13

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
}

byte level=0; //Nivel PWM
int8_t delta=1; //Dirección

void loop()
{
  //Escribimos el nivel PWM en el pin del led
  analogWrite(pinLed,level);
  //Calculamos el valor del nivel del próximo ciclo
  level+=delta;
  //Si hemos llegado a alguno de los extremos...cambiamos de dirección
  if(level==0 || level==0xFF) delta=-delta;
  //Esperamos un momento
  delay(5);
}
