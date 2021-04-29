/*
 * Autor: Ramón Junquera
 * Tema: Extensor de pines digitales 74HC595N
 * Objetivo: Demostración de las capacidades de la clase RoJo74HC595N
 * Material: placa RPi, breadboard, display 1 digit, chip 74HC595N,
 *   cables
 * 
 * Descripción:
 * El ejemplo muestra en el display todos los caracteres permitidos en
 * secuencia.
 * Se va alternando el punto decimal para demostar que no está ligado al
 * carácter,
 * 
 * Recordemos la nomenclatura de los segmentos
 * 
 *       a
 *     ██████
 *    █      █
 *  f █      █ b
 *    █  g   █
 *     ██████
 *    █      █
 *  e █      █ c
 *    █      █
 *     ██████   █ h
 *       d
 * 
 * Resultado:
 * Se muestran todos los caracteres posibles en el display
 */

#include <Arduino.h>
#include "RoJo74HC595N.cpp"

using namespace std;

int main(int argc, char **argv)
{
    //Definición de constantes globales
	const byte pinLatch = 20;
	const byte pinClock = 16;
	const byte pinData = 21;

    //Declaramos el objeto que gestionará nuestro display
	//Como parámetros le pasamos los pines de conexión del 74HC595N
	//Y el número de chips conectados en cascada (=1)
	RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,1);

	bool dot=false; //Se muestra el punto?
	    
    //Bucle infinito
    while (1)
    {
		//Recorremos todos los caracteres del diccionario...
		for(byte i=0;i<19;i++)
		{
			//Escribimos en la primera posición de la memoria de vídeo
			//el carácter actual con el punto si corresponde.
			display.set(0,i,dot);
			//Mostramos el contenido de la memoria de vídeo
			display.show();
			//Cambiamos el estado del punto para el próximo ciclo
			dot=!dot;
			//Esperamos un momento
			delay(600);
		}
    }
}

