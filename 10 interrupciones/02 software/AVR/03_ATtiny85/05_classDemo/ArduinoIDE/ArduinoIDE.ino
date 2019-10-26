/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20191025
  Objetivo: Demostración de clase de gestión del timers de ATtiny85
  Material: Placa ATtiny85

  Descripción:
    Puesto que el timer 1 es utilizado por las funciones delay y millis,
    sólo utilizaremos el timer 0.
    Lanzamos el timer para que llame a una función que cambie el estado
    del led integrado, cada segundo.
    Lo mantenemos 5 segundos.
    Paramos el timer.
    Esperamos 3 segundos.
    Lanzamos el timer una sóla vez.
    Lanzamos el timer indefinidamente.
  
  Resultado:
    Se muestran los mensajes de la función a la que llaman el timer.
*/

#include <Arduino.h>
#include "RoJoTimerTiny0.h" //Gestión de timer 0 de ATtiny85

const byte pinLed=1;

//Función a la que se llama cuando se produce una interrupción del timer 0
void f0() {
  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
}

void setup() {
  pinMode(pinLed,OUTPUT);
  timerTiny0.attach(1,f0); //Llamará a la función f0 cada segundo
  delay(5000); //Esperamos 5 segundos
  timerTiny0.detach(); //Detenemos el timer
  delay(3000); //Esperamos 3 segundos
  timerTiny0.once(2,f0); //Arrancamos el timer para una sóla ejecución en 2 segundos
  delay(4000); //Esperamos 4 segundos
  timerTiny0.attach_ms(100,f0); //Arrancamos el timer cada medio segundo (500ms)
}

void loop() {
  //Nada especial que hacer aquí
}
