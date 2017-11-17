/*
 * Autor: Ramón Junquera
 * Tema: Librería RoJoArduino
 * Objetivo: Gestión de varios pines digitales
 * Material: Raspberry Pi, breadboard, led, cables
 * Descripción:
 * En este ejercicio controlaremos varios pines digitales.
 * Concretamente 8.
 * Haremos el efecto KITT.
 * 
 * Tomaremos los 8 pines digitales que se encuentran en la hilera de
 * pines exterior del conector.
 * 
 * Lo primero que hacemos es crear un array con los números de pin para
 * poder controlar el orden.
 * 
 * Se adjunta circuito en Fritzing.
 */

#include <Arduino.h>

using namespace std;

int main(int argc, char **argv)
{
	//Definimos las constantes de la función
	const byte leds[8]={18,23,24,25,12,16,20,21};
    const uint32_t waitTime = 80;
    
	//Variables
	byte currentLed=0; //Led procesado actualmente
	int8_t delta=-1; //Dirección
    
    //Definimos los pines como salida
    for(byte i=0;i<8;i++) pinMode(leds[i],OUTPUT);
    
    //Bucle infinito
    while (1)
    {
		//Encendemos el led
		digitalWrite(leds[currentLed],HIGH);
		//Esperamos un momento
		delay(waitTime);
		//Apagamos el led
		digitalWrite(leds[currentLed],LOW);
		
		//Si el led procesado era un extremo...cambiamos de dirección
		if(currentLed==0 || currentLed==7) delta=-delta;
		//Calculamos el siguiente led
		currentLed+=delta;	
    }
}

