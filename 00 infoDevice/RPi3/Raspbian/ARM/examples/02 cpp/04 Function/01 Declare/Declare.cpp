/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Funciones
 * Material: Raspberry Pi
 * Descripción:
 * Las funciones en C++ son iguales que en C.
 * 
 * Se pueden declaran de dos maneras:
 * 1 - Declaración y definición de función
 * 2 - Definición previa a su uso
 * 
 * El ejemplo actual muestra el primero de los métodos.
 * 
 * Personalmente me gusta más el segundo.
 * La función se define antes de ser utilizada.
 * No necesita una declaración.
 * Evitaremos errores declarando algo distinto a lo definido.
 * Es más corto.
 */


#include <iostream>

using namespace std;

//Declaración de la función de que definirá más adelante
int sum(int a,int b);

int main(int argc, char **argv)
{
	//Variables globales
	int a = 2;
	int b = 3;
	
	//Mostramos el resultado de la función
	cout << "Resultado=" << sum(a,b) << endl;
	
	//Todo ok
	return 0;
}

int sum(int a,int b)
{
	//Función que devuelve la suma loa valores de los parámetros
	return a+b;
}
