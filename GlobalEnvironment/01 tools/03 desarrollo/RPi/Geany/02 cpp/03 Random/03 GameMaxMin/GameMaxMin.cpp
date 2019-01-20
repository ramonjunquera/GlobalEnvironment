/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Generación de números aleatorios
 * Material: Raspberry Pi
 * Descripción:
 * Basándonos en los ejercicios anteriores, crearemos el juego de
 * mayor/menor.
 * El programa selecciona un número aleatorio entre 1 y 100 y el usuario
 * debe adivinarlo con las pistas de mayor o menor.
 * 
 * 
 * 
 */


#include <iostream>
#include <ctime> //Para obtener la hora
#include <cstdlib> //Para función rand()

using namespace std;

int main(int argc, char **argv)
{
	//Generamos una nueva semilla en función de la hora actual
	srand((unsigned int)time(NULL));
	
	//Generamos un número aleatorio entre 1 y 100
	int rightValue = 1 + rand()%100;
	
	//Creamos una variable para guardar el número del usuario
	//La inicializamos con un valor que nunca puede darse
	int userValue=0;
	
	//Mostramos mensaje de inicio de juego
	cout << "Adivina un número del 1 al 100.\n";

	//Mientras no se haya acertado el número correcto...
	while(rightValue != userValue)
	{
		//Pedimos un número al usuario
		cin >> userValue;
		//Si es menor informamos...
		if(rightValue < userValue) cout << "menor\n";
		//...si no es menor comprobamos si es mayor e informamos
		else if(rightValue > userValue) cout << "mayor\n";
	}
	//El usuario ha acertado el número. Informamos
	cout << "Correcto!!!";

	//Todo ok
	return 0;
}

