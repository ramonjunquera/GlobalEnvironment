/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20180621
  Objetivo: Demostración de clase de gestión del timer 3
  Material: placa Arduino Mega

  Descripción:
  Con ayuda de la librería pediremos que se ejecute una función que hemos definido cada cierto
  tiempo.
  La función sólo cambia el estado del led integrado.
  Llamaremos varias veces a la función de inicialización y cambiaremos los periodos.

  Nota:
  La librería RoJoTimer3 sólo es compatible con Arduino Mega, puesto que tiene 5 timers disponibles.
  Arduino UNO/Nano sólo tienen los timers 1 y 2.
  
  Resultado:
  El led integrado en placa parpadea a distintas frecuencias.
*/

#include <Arduino.h>
#include "RoJoTimer3.h" //Gestión de timer 3. Se crea objeto timer3

const byte pinLed=13; //Pin del led integrado

void IntFunc()
{
  //Función a la que se llama cuando se produce una interrupción

  //Cambiamos el estado del led integrado
  digitalWrite(pinLed,!digitalRead(pinLed));
}

void setup()
{
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT); 
  //Indicamos que queremos que parpadee cada segundo
  timer3.begin(IntFunc,1000000);
  //Esperamos 5 segundos
  delay(5000);
  //Deja de parpadear
  timer3.end();
  //Esperamos 5 segundos
  delay(5000);
  //Parpadea cada medio segundo
  timer3.begin(IntFunc,500000);
  //Esperamos 5 segundos
  delay(5000);
  //Parpadea cada décima de segundo
  timer3.begin(IntFunc,100000);
}

void loop()
{
  //No tenemos nada que ejecutar en el bucle principal
}






