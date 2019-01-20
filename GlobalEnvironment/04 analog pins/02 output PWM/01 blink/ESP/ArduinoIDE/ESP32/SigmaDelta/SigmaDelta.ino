/*
  Autor: Ramón Junquera
  Fecha: 20181108
  Tema: Elementos analógicos
  Objetivo: Escrituras analógicas PWM. Método SigmaDelta
  Material: placa ESP32

  Descripción:
  Haremos variar la intensidad del led integrado asociado a un pin
  progresivamente desde apagado hasta el máximo y después a la inversa.

  Las placas ESP32 no tienen definida la función analogWrite a fecha 
  actual.
  Hacemos uso de una clase definida especialmente para ello.
  El método de aplicación de PWM es SigmaDelta.
  Puesto que la funcionalidad PWM no está asociada con los pines, podemos
  aplicarla a cualquiera.

  En este ejemplo utilizamos el mismo led integrado.
  
  Resultado:
  El led integrado se enciende y apaga progresivamente
*/

#include <Arduino.h>
#include "RoJoAnalogWriteESP32S.h"

//Declaración de variables globales
const byte pinLed = LED_BUILTIN;

void setup()
{
  //Nada especial que configurar
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
