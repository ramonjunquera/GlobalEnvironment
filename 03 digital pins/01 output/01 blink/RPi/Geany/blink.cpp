/*
 * Autor: Ramón Junquera
 * Tema: Librería RoJoArduino
 * Objetivo: Escritura de pines digitales
 * Material: Raspberry Pi, breadboard, cables, led
 * Descripción:
 * Cambiaremos el estado del pin 21 cada segundo, haciendo que parpadee
 * el led que está conectado en él.
 * Se incluye un esquema de Fritzing en la carpeta del proyecto.
 */

#include <Arduino.h>

using namespace std;


int main(int argc, char **argv)
{
    //Definimos el pin con el que trabajaremos
    const byte pin=21;
    //Configuramos el pin 21 como salida
    pinMode(pin,OUTPUT);
    //Definimos la variable que guardará el estado del pin
    byte pinStatus=LOW;
    
    while(1)
    {
		//Aplicamos el estado al pin
		digitalWrite(pin,pinStatus);
		//Cambiamos el estado del pin para el próximo ciclo
		pinStatus = !pinStatus;
		//Esperamos un segundo
		delay(1000);
	}
}

