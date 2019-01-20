/*
 * Autor: Ramón Junquera
 * Tema: Módulo de reloj Tiny RTC I2C Real Time
 * Objetivo: Demo de librería de gestión de la memoria EEPROM del módulo
 *   de reloj Tiny RTC I2C Real Time
 * Material: breadboard, placa RPi, cables, módulo "Tiny RTC I2C"
 * 
 * Descripción:
 * RTC incluye una memoria de 32Kb = 32*1024= 8192 bytes
 * Crearemos un programa que nos permita interactuar con la memoria
 * EEPROM que contiene el Tiny RTC.
 * Nos comunicaremos a traves de la consola.
 * Utilizaremos una colección de comandos básicos para ello: poke y peek
 * Ejemplos:
 *   Escribe el valor 23 en la posición de memoria 700
 *     poke 700,23
 *   Lee el contenido de la posición de memoria 932
 *     peek 932
 * 
 * Con estos simples comandos podemos leer y escrbir en la memoria
 * EEPROM.
 * 
 * Resultado:
 * Con la colección de comando podemos escribir y leer en la memoria
 * EEPROM
 */

#include <iostream>
#include <Arduino.h>
#include "RoJoRTCeeprom.cpp"

//Declaración de variables globales
const byte pinLed = 21; //Pin del led

int main(int argc, char **argv)
{
	//Objeto de gestión de la memoria EEPROM del RTC
	RoJoRTCeeprom mem;
		
	if(!mem.check())
	{
		cout << "EEPROM no encontrada!" << endl;
		return 1;
	}
	cout << "EEPROM activa" << endl;
	
	string command; //Comando: peek o poke
	uint16_t address; //Dirección de peek o poke
	//Valor de poke. Es un uint16 porque si ponemos byte, cin sólo leerá
	//el siguiente carácter y no el siguiente valor numérico
	uint16_t value; 
	//Utilizado para almacenar el separador de dirección y valor en poke
	char comma; 
	
  
	//Bucle infinito
	while(1)
	{
		//Leemos el texto de la primera palabra (comando)
		cin >> command;
		//Si es el comando peek...
		if(command=="peek")
		{
			//...obtenemos la dirección
			cin >> address;
			//Si la dirección es mayor que la memoria real...error
			if(address>=8192) cout << "Error. Dirección demasiado grande" << endl;
			//La dirección está dentro del rango permitido
			//Mostramos el contenido de la dirección
			else cout << (int)mem.peek(address) << endl;
		}
		else if(command=="poke")
		{
			//...obtenemos la dirección, el separador y el valor
			cin >> address >> comma >> value;
			//Si la dirección es mayor que la memoria real...error
			if(address>=8192) cout << "Error. Dirección demasiado grande" << endl;
			else //La dirección está dentro del rango permitido
			{
				//Escribimos el valor en la dirección
				//Nos aseguramos que el valor entra en un byte
				mem.poke(address,value & 0xFF);
				cout << "ok" << endl;
			}
		}
		else cout << "Comando desconocido: " + command << endl;
	}
}

