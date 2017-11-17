/*
 * Autor: Ramón Junquera
 * Tema: Gestión de interrupciones
 * Objetivo: Demostración de clase de gestión del timer 1
 * Material: breadboard, cables, led, placa RPi
 * 
 * Descripción:
 * Con ayuda de la librería pediremos que se ejecute una función que
 * hemos definido cada cierto tiempo.
 * La función sólo cambia el estado del led integrado.
 * Llamaremos varias veces a la función de inicialización y cambiaremos
 * los periodos.
 * 
 * Resultado:
 * El led integrado en placa parpadea a distintas frecuencias.
 */

#include <Arduino.h>
#include "RoJoTimer1.cpp" //Gestión de timer 1. Se crea objeto timer1

//Declaración de variables globales
const byte pinLed = 21; //Pin del led

void IntTimer(int signalType)
{
  //Función a la que se llama cuando se produce una interrupción

  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
}

int main(int argc, char **argv)
{
	//Configuramos el pin del led para que sea de salida
	pinMode(pinLed,OUTPUT);
	
	//Indicamos que queremos que parpadee cada segundo
	timer1.begin(IntTimer,1000000);
	//Esperamos 5 segundos
	delay(5000);
	//Deja de parpadear
	timer1.end();
	//Esperamos 5 segundos
	delay(5000);
	//Parpadea cada medio segundo
	timer1.begin(IntTimer,500000);
	//Esperamos 5 segundos
	delay(5000);
	//Parpadea cada décima de segundo
	timer1.begin(IntTimer,100000);
	//Esperamos 5 segundos
	delay(5000);
	//Deshabilitamos el timer
	timer1.end();
	
	//Terminamos de utrabajar con los pines
	ArduinoEnd();
	//Hemos terminado si errores
	return 0;
}

