/*
 * Autor: Ramón Junquera
 * Tema: punteros de funciones
 * Objetivo: ejemplo de uso de punteros de funciones
 * Descripción:
 * C++ también permite declarar punteros de funciones.
 * Es importante que cuando se declare el puntero, se especifiquen las 
 * características de la función: si devuelve algo y los parámetros que
 * tiene con sus tipos.
 * En nuestro ejemplo tenemos dos funciones. Una de ellas no devuelve
 * nada y no tiene parámetros. La segunda devuelve un entero y tiene un
 * par de parámetros enteros.
 * En el programa principal declararemos una variable de tipo puntero
 * de función, le asignaremos el puntero de la función que le
 * corresponde y a continuación llamaremos a la función a través del 
 * nuevo puntero.
 */
 
#include <iostream>
using namespace std;

void myFunction()
{
	//Función que no devuelve nada y sin parámetros
	cout << "Hola" << endl;
}

int add(int a,int b)
{
	//Función que devuelve un entero y con 2 parámetros enteros
	return a+b;
}

int callAdd(int a,int b,int (*f)(int,int))
{
	//Función que llama a una función
	//Se llama a una función que devuelve un entero y tiene dos
	//parámetros que también son enteros
	
	return f(a,b);
}

int main(int argc, char **argv)
{
	//Declaramos una variable de puntero a una función que no devuelve
	//nada y que no tiene parámetros
	void (*myNewFunction)();
	//Asignamos el puntero de una función existente a nuestra variable
	myNewFunction = myFunction;
	//Llamamos a la función como si fuese la original
	myNewFunction();
	//Repetimos los mismos pasos para una función que devuelte un valor
	//y tiene parámetros
	int (*newAdd)(int,int);
	newAdd = add;
	cout << newAdd(2,3) << endl;
	//Llamamos a una función que llama a otra pasándole los dos
	//parámetros primeros
	cout << callAdd(3,4,add);

    //Todo ok
	return 0;
}

