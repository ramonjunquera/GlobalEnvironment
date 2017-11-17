/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: breadboard, led, placa ESP8266

  Descripción:
  Haremos variar la intensidad de un led integrado conectado a un pin con
  capacidades PWM progresivamente desde apagado hasta el máximo y después
  a la inversa.

  Todas las placas ESP8266 tienen al menos un led integrado en placa
  identificado por la constante LED_BUILTIN asociado a un pin con
  capacidades PWM.

  Resultado:
  El led integrado se enciende y apaga progresivamente
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = LED_BUILTIN;

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
}

uint16_t level=0; //Nivel PWM
int8_t delta=1; //Dirección

void loop()
{
  //Escribimos el nivel PWM en el pin del led
  analogWrite(pinLed,level);
  //Calculamos el valor del nivel del próximo ciclo
  level+=delta;
  //Si hemos llegado a alguno de los extremos...cambiamos de dirección
  if(level==0 || level==1023) delta=-delta;
  //Esperamos un momento
  delay(1);
}
