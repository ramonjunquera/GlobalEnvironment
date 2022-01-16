/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Programación de interruptor con resistencias de pulldown.
  Material adicional: breadboard, interruptor, resistencia de 10 Kohmios, cables y Arduino UNO

  Descripción:
  En este ejemplo el led integrado se encenderá al pulsar el interruptor.
  Puesto que el estado por defecto del pin de entrada es LOW debido a que está conectado
  a GND a través de la resistencia de pulldown, y coincide con el estado de apagado del led
  integrado, podemos copiar directamente el estado del pin del interruptor al pin del led.
  
  Nota:
  Distinguimos una resistencia de 10 Kohmios porque tiene las siguientes bandas:
  - Banda 1 = marrón = 1
  - Banda 2 = negra = 0
  - Banda 3 = negra = 0
  - Banda 4 = rojo = multiplicador *100
  - Banda 5 = marrón = tolerancia +-1%
    
  Resultado:
  Al pulsar el botón del interruptor, el led se enciende
*/

#include <Arduino.h>

const byte pinLed=LED_BUILTIN; //led integrado = pin 13
const byte pinSwitch=5; //pin del interruptor

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
  //Configuramos el pin del interruptor como entrada para poder leerlo
  pinMode(pinSwitch, INPUT);
}

void loop()
{
  //Copiamos el estado del pin del interruptor en el pin del led
  digitalWrite(pinLed, digitalRead(pinSwitch));
}
