/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Generación de números aleatorios
 * Material: Raspberry Pi
 * Descripción:
 * Crearemos un programa que muestre 10 números aleatorios
 * 
 * Para ello utilizaremos la función rand() de la librería cstdlib.
 * Esta función genera números aleatorios entre 0 y RAND_MAX
 * RAND_MAX es una constante definida en la librería cstdlib
 * 
 * La rutina de generación de números aleatorios se basa en una semilla.
 * Si no alteramos la semilla por defecto, los números generados siempre
 * serán los mismos.
 * 
 * Para evitarlo, tomaremos la hora actual y la utilizaremos como
 * semilla.
 */


#include <iostream>
#include <ctime> //Para obtener la hora
#include <cstdlib> //Para función rand()

using namespace std;

int main(int argc, char **argv)
{
	//Generamos una nueva semilla en función de la hora actual
	srand((unsigned int)time(NULL));
	
	//Repetimos 10 veces...
	for(int i=0;i<10;i++)
	{
		//Generamos un nuevo número entero aleatorio
		int num = rand();
		//Lo mostramos
		cout << num <<endl;
	}
	
	//Todo ok
	return 0;
}

