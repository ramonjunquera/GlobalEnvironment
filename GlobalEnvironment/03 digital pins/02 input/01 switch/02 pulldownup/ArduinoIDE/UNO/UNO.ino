/*
  Autor: Ramón Junquera
  Tema: Interruptores
  Objetivo: Señal de interruptor con resistencias pull down & pull up
  Material adicional: breadboard, 2 interruptores, 2 resistencias de 10 Kohmios, cables, Arduino UNO

  Descripción:
  Montaremos dos interruptores. El primero de ellos con resistencias pulldown y el segundo con 
  pullup y mostraremos la lectura de sus correspondientes pines a través del monitor serie.

  Lo que se pretende es que se entienda que las señales leidas no son iguales. Dependen del tipo
  de resistencia utilizada.
    
  Resultado:
  Se puede comprobar la señal que generan ambos interruptores en tiempo real
*/

#include <Arduino.h>

const byte pinLed=LED_BUILTIN; //led integrado = pin 13
const byte pinSwitch1=2; //pin del interruptor 1
const byte pinSwitch2=3; //pin del interruptor 2

void setup()
{
  Serial.begin(115200);
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
  //Configuramos los pines de los interruptores como entradas
  pinMode(pinSwitch1, INPUT);
  pinMode(pinSwitch2, INPUT);
}

void loop()
{
  Serial.print(digitalRead(pinSwitch1));
  Serial.print(F(" "));
  Serial.println(digitalRead(pinSwitch2));
  delay(300);
}
