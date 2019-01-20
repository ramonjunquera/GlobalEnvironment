/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Utilizar las resistencias internas de pullup
  Material adicional: breadboard, 2 interruptores, cables, Arduino UNO

  Descripción:
  Este ejercicio es idéntico al anterior.
  También utilizamos resistencias de pullup para los dos interruptores, pero en
  este caso activamos las resistencias internas. Esto hace que el circuito se
  simplifique. Pero el funcionamiento es el mismo.
  
  Queremos que se encienda el led de cuando ambos interruptores estén pulsados.
    
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
  //Configuramos los pines de los interruptores como entradas y con las
  //resistencias de pullup activas
  pinMode(pinSwitch1, INPUT_PULLUP);
  pinMode(pinSwitch2, INPUT_PULLUP);
}

void loop()
{
  //Como ambos interruptores tienen resistencias pullup, por defecto tendrán señal HIGH.
  //Y cuando se pulsen, LOW.
  //Si ambos interruptores están pulsados se encenderá el led
  digitalWrite(pinLed,!(digitalRead(pinSwitch1) || digitalRead(pinSwitch2)));
}
