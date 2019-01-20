/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Gestionar dos interruptores simultáneamente
  Material adicional: breadboard, 2 interruptores, 2 resistencias de 10 Kohmios, cables, Arduino UNO

  Descripción:
  Este ejemplo servirá para aprender a controlar más de un interruptor al mismo tiempo.
  Utilizaremos dos interruptores y el led integrado en placa.
  Para controlar los interruptores utilizaremos los pines 2 y 3.
  Queremos que se encienda el led de cuando ambos interruptores estén pulsados.
  Los interruptores tienen resistencias de pullup.
    
  Resultado:
  Al pulsar ambos interruptores, el led se enciende
*/

#include <Arduino.h>

const byte pinLed=LED_BUILTIN; //led integrado = pin 13
const byte pinSwitch1=2; //pin del interruptor 1
const byte pinSwitch2=3; //pin del interruptor 2

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
  //Configuramos los pines de los interruptores como entradas
  pinMode(pinSwitch1, INPUT);
  pinMode(pinSwitch2, INPUT);
}

void loop()
{
  //Como ambos interruptores tienen resistencias pullup, por defecto tendrán señal HIGH.
  //Y cuando se pulsen, LOW.
  //Si ambos interruptores están pulsados se encenderá el led
  digitalWrite(pinLed,!(digitalRead(pinSwitch1) || digitalRead(pinSwitch2)));
}
