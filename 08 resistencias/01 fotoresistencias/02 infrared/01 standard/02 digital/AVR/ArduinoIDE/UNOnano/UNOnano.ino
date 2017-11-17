/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja
  Objetivo: Funcionamiento de una fotorresistencia infrarroja
  Material: breadboard, fotorresistencia infrarroja, led infrarrojo, resistencia de 10Kohmios
    resistencia de 220 ohmios, pulsador, cables, Arduino UNO y Arduino Nano

  Descripción:
  Teniendo en cuenta las lecturas analógicas tomadas en el receptor en el ejercicio anterior, no debería
  ser un problema utilizar pines digitales. Esto es lo que probaremos en este proyecto.
  El circuito se mantiene, pero esta vez utilizaremos el pin A0 como pin digital. Concretamente será el 14.
  Para no tener que utilizar el puerto serie, replicaremos el estado del pin 14 en el 13 y así podremos
  ver el resultado en el led integrado.

  Resultado:
  A través del led integrado en placa podemos ver que se recibe perfectamente el estado del emisor
*/

#include <Arduino.h>

void setup()
{
  //El pin 13 del led integrado será de salida para poder escribir en él
  pinMode(13, OUTPUT);
}

void loop()
{
  //El estado del led integrado (13) será el mismo que el del pin digital 14 (A0)
  digitalWrite(13,digitalRead(14));
}
