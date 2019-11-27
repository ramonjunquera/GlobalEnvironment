/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Sistemas de debug en ATtiny85. Librería DigiCDC.h
  Objetivo: Envío de información
  Material adicional: ATtiny85, breadboard

  Descripción:
  Enviaremos por el puerto serie el valor de una variable que contiene un contador.
  El contador se incrementa cada segundo de manera infinita.

  Resultado:
  Vemos en el valor del contador en el monitor seria
*/

#include <Arduino.h>
#include <DigiCDC.h> //Gestión de puerto serie

//Definición de variables globales
unsigned int counter=0;

void setup() {                
  //Inicializamos el puerto serie USB. No se indica velocidad!
  SerialUSB.begin(); 
}

void loop() {
  //Enviamos el valor actual del contador por el puerto serie USB
  //y aprovechamos para incrementar después su valor
  SerialUSB.println(counter++);
  //Esperamos un poco con el método delay del nuevo puerto serie USB
  //Esto impide que se pierda la comunicación con el ordenador
  SerialUSB.delay(1000);
}
