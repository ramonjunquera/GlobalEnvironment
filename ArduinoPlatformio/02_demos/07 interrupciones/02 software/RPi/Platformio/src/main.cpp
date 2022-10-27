/*
  Autor: Ramón Junquera
  Fecha: 20221027
  Tema: Interrupciones en RPi
  Objetivo: Demo de librería RoJoTimerRPi
*/

#include <Arduino.h>
#include <RoJoTimerRPi.h>


//Declaración de variables globales
RoJoTimerRPi timer;
const byte pinLed = 21; //Pin del led

//Función a la que se llama cuando se produce una interrupción
void iFunction() {
  digitalWrite(pinLed,!digitalRead(pinLed)); //Cambiamos el estado del led
}

void setup() {
	pinMode(pinLed,OUTPUT);
	timer.attach(1,iFunction); //Indicamos que queremos que parpadee cada segundo
  delay(5000); //Esperamos 5 segundos
	timer.detach(); //Deja de parpadear
  delay(5000); //Esperamos 5 segundos
	timer.attach_ms(500,iFunction); //Parpadea cada medio segundo
  delay(5000); //Esperamos 5 segundos
	timer.attach(0.1,iFunction); //Parpadea cada décima de segundo
  delay(5000); //Esperamos 5 segundos
	timer.detach(); //Deshabilitamos el timer
  timer.once(5,iFunction); //En 5 segundos cambiamos el estado
}

void loop() {

}

