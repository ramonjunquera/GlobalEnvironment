/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Generación de números aleatorios
 * Material: Raspberry Pi
 * Descripción:
 * Crearemos un programa que muestre 10 números aleatorios entre ciertos
 * límites.
 * 
 * Como hemos visto en el ejercicio anterior, rand() devuelve un número
 * aleatorio bastante grande.
 * Si queremos limitar su rango, lo que hacemos es tomar el resto de
 * dividir el número a leatorio entre el rango deseado.
 * Si además tenemos un valor mínimo, tendremos que sumarlo al resultado
 * obtenido.
 * 
 * En el programa definimos como constantes los límites deseados.
 * 
 * El rango real será [minRand,maxRand[
 * El valor máximo siempre estará excluido. Nunca podrá aparecer.
 */


#include <iostream>
#include <ctime> //Para obtener la hora
#include <cstdlib> //Para función rand()

using namespace std;

int main(int argc, char **argv)
{
	//Declaración de constantes
	int minRand = 100; //Límite inferior
	int maxRand = 200; //Límite superior
	
	//Generamos una nueva semilla en función de la hora actual
	srand((unsigned int)time(NULL));
	
	//Repetimos 10 veces...
	for(int i=0;i<10;i++)
	{
		//Generamos un nuevo número entero aleatorio
		//entre los límites especificados
		int num = minRand + rand() % (maxRand-minRand);
		//Lo mostramos
		cout << num <<endl;
	}
	
	//Todo ok
	return 0;
}

