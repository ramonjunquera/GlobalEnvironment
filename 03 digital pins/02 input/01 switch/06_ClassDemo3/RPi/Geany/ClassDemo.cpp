/*
 * Autor: Ramón Junquera
 * Tema: Interruptores
 * Objetivo: Demostración de las capacidades de la clase RoJoSwitch
 * Material adicional: breadboard, 3 interruptores, 3 leds, cables, 
 *   placa RPi
 * 
 * Descripción:
 * La clase RoJoSwitch se ha creado para simplificar el uso de
 * interruptores.
 * Se utiliza la técnica de polling (comprobación de estado continua).
 * Ya no tenemos que ocuparnos ni de la configuración de los pines ni
 * de la gestión del efecto rebote.
 * 
 * Tenemos 3 interruptores. Cada uno de ellos asociado a un led.
 * El primer led cambia de estado cuando se pulsa su interruptor.
 * El segundo cuando se suelta el botón.
 * Y el tercero se enciende cuando se mantiene pulsado.
 * 
 * Resultado:
 * Los leds se encienden con distintas condiciones
 */

#include <Arduino.h>
#include "RoJoSwitch.cpp"

int main(int argc, char **argv)
{
    const byte pinSwitch1=8;
	const byte pinLed1=16;
	const byte pinSwitch2=7;
	const byte pinLed2=20;
	const byte pinSwitch3=12;
	const byte pinLed3=21;
	
	//Declaramos los objetos de gestión de los interruptores
	RoJoSwitch mySwitch1(pinSwitch1);
	RoJoSwitch mySwitch2(pinSwitch2);
	RoJoSwitch mySwitch3(pinSwitch3);

	//Configuramos los pines de los leds como salida
	pinMode(pinLed1,OUTPUT);
	pinMode(pinLed2,OUTPUT);
	pinMode(pinLed3,OUTPUT);
      
    //Bucle infinito
    while(1)
    {
		if(mySwitch1.pressed()) digitalWrite(pinLed1,!digitalRead(pinLed1));
		if(mySwitch2.released()) digitalWrite(pinLed2,!digitalRead(pinLed2));
		digitalWrite(pinLed3,mySwitch3.pressing());
	}
}

