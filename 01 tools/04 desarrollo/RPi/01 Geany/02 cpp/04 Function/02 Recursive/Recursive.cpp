/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Funciones recursivas
 * Material: Raspberry Pi
 * Descripción:
 * Desarrollaremos una función que se llama a sí misma.
 * la función sirve para el cálculo del factorial.
 * 
 * En esta ocasión definiremos la función antes de ser utilizada y así
 * no tendremos que declararla.
 */


#include <iostream>

using namespace std;

//Declaración de la función de que definirá más adelante
int Factorial(int n)
{
	//Calcula el factorial
	
	//Si el número es menor que 2...su factorial es 1
	if(n<2) return 1;
	//El número es 2 o mayor
	
	//Su factorial será el número por el factorial de un valor una
	//unidad inferior.
	return n*Factorial(n-1);
}

int main(int argc, char **argv)
{
	//Utilizamos los valores del 1 al 10
	for(int a=1;a<=10;a++)
	{
		//Mostramos el resultado
		cout << "Factorial de " << a << " es " << Factorial(a) << endl;
	}

	//Todo ok
	return 0;
}

