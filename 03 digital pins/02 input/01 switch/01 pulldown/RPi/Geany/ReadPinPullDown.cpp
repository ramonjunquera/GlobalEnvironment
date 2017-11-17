/*
 * Autor: Ramón Junquera
 * Tema: Interruptores
 * Objetivo: Programación de interruptor con resistencias de pulldown.
 * Material: Raspberry Pi, breadboard, pulsador, led, resistencia de 10
 *   Kohmios
 * Descripción:
 * En este ejemplo el led se encenderá al pulsar el interruptor.
 * Puesto que el estado por defecto del pin de entrada es LOW debido a
 * que está conectado a GND a través de la resistencia de pulldown, y
 * coincide con el estado de apagado del led, podemos copiar
 * directamente el estado del pin del interruptor al pin del led.
 * 
 * Nota:
 * Distinguimos una resistencia de 10 Kohmios porque tiene las
 * siguientes bandas:
 * - Banda 1 = marrón = 1
 * - Banda 2 = negra = 0
 * - Banda 3 = negra = 0
 * - Banda 4 = rojo = multiplicador *100
 * - Banda 5 = marrón = tolerancia +-1%
 * 
 * Resultado:
 * Al pulsar el botón del interruptor, el led se enciende
 */


#include <iostream>
#include <Arduino.h>

int main(int argc, char **argv)
{
    const byte pinLed=21;
	const byte pinSwitch=20;
	
	//Configuramos el pin del led como salida para poder escribir en él
	pinMode(pinLed, OUTPUT);
	//Configuramos el pin del interruptor como entrada para poder leerlo
	pinMode(pinSwitch, INPUT);
      
    //Bucle infinito
    while(1)
    {
		//Copiamos el estado del pin del interruptor en el pin del led
		digitalWrite(pinLed,digitalRead(pinSwitch));
	}
}

