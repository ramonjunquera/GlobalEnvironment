/*
 * Autor: Ramón Junquera
 * Tema: Gestión de interrupciones
 * Objetivo: Utilizar interrupciones con la librería de gestión de
 *   interruptores
 * Material adicional: breadboard, cables, interruptor de pulsador,
 *   placa RPi
 * 
 * Descripción:
 * Ahora que ya sabemos gestionar interrupciones, utilizaremos la
 * librería de gestión de interruptores (RoJoSwitch) para llamarla sólo
 * cuando se produzca un cambio de estado en el pin del interruptor, en
 * vez de contínuamente.
 * 
 * El led cambiará de estado cuando se detecte una pulsación en el
 * interruptor.
 * Recordemos que la librería ya gestiona el efecto rebote.
 * 
 * Resultado:
 * El led cambia de estado cuando se pulsa el interruptor
 */

#include <Arduino.h>
#include "RoJoSwitch.cpp"

//Declaración de variables globales
const byte pinLed = 21; //Pin del led integrado
const byte pinSwitch = 20; //pin al que está conectado el interruptor

//Declaramos el objeto de gestión del interruptor
RoJoSwitch mySwitch(pinSwitch);

void IntSwitch(byte pin,byte status)
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Si el interruptor ha sido pulsado...cambiamos el estado del led
  if(mySwitch.pressed()) digitalWrite(pinLed,!digitalRead(pinLed));
}

int main(int argc, char **argv)
{
	//Configuramos el pin del led para que sea de salida
	pinMode(pinLed,OUTPUT);
	
	//Activamos las resistencias de pull-up para el pin de interruptor
	//Por defecto el pin del interruptor estará en HIGH y cuando se
	//pulse en LOW
	pinMode(pinSwitch,INPUT_PULLUP);
	
	//Definimos la interrupción
	//Será la correspondiente al pin del interruptor
	//Se llamará a la función IntSwitch
	//Ocurrirá cuando el estado del pin cambie
	attachInterrupt(pinSwitch,IntSwitch,CHANGE);
  
    //Bucle infinito
    while(1)
    {
		//Nada especial que hacer aquí
	}
}

