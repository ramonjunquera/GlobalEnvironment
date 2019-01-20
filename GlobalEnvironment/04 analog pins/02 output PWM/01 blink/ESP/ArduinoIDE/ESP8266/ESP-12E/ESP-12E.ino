/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: placa ESP-12E

  Descripción:
  La placa ESP-12E es muy particular respecto al resto porque tiene dos
  leds integradso en placa:

    GPIO pin pos    const
    ---- --- ------ -----------
     16   D0 placa  LED_BUILTIN
      2   D4 antena

  Ambos pines tienen capacidad PWM y los led integrados adjuntos pueden mostrarlo.
  En esta variante se cambiará el nivel PWM a ambos simultáneamente.
  
  Resultado:
  Los leds integrados se encienden y apagan progresivamente
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed1 = D4;
const byte pinLed2 = D0;

void setup()
{
  //Configuramos los pines de los leds como salida para poder escribir en ellos.
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
}

uint16_t level=0; //Nivel PWM
int8_t delta=1; //Dirección

void loop()
{
  //Escribimos el nivel PWM en los pines de los leds
  analogWrite(pinLed1,level);
  analogWrite(pinLed2,level);
  //Calculamos el valor del nivel del próximo ciclo
  level+=delta;
  //Si hemos llegado a alguno de los extremos...cambiamos de dirección
  if(level==0 || level==1023) delta=-delta;
  //Esperamos un momento
  delay(1);
}
