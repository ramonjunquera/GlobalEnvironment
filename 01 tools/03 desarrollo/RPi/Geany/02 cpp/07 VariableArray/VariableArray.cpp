/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Arrays dinámicos
 * Material: Raspberry Pi
 * Descripción:
 * Hay veces que desconocemos el tamaño que tendrá un array antes de
 * comenzar el programa.
 * 
 * El el siguiente ejemplo crearemos un array dinámico (tamaño definido
 * en tiempo de ejecución).
 * 
 * Comenzamos la función main declarando la variable que definirá el 
 * numero de elementos del array dinámico.
 * 
 * El array será de números enteros.
 * 
 * Creamos un puntero a un array de números enteros (int *valArray)
 * Con la función new reservaremos la memoria necesaria para contenerlo.
 * 
 * Cuando se reserva cierta parte de la memoria no se borra su
 * contenido. Eso quiere decir que contendrá "basura".
 * Esto es lo que hace la siguiente línea:
 *   int *valArray = new int[valCount];
 * Si queremos que la memoria se borre tras su reserva, debemos añadir 
 * al final unos paréntesis:
 *   int *valArray = new int[valCount]():
 * 
 * Hemos creado una función que se encarga de mostrar el valor de los
 * elementos del array, puesto que la llamaremos en dos ocasiones.
 * 
 * Mostramos el array para comprobar que se ha creado vacío.
 * 
 * Damos un valor aleatorio a cada elemento y volvemos a mostrarlo.
 * 
 * Es muy importante que una vez que hemos terminado de trabajar con 
 * el array, nos encarguemos de liberar la memoria reservada con el
 * comando new. Esto no se se hace automáticamente con este tipo de
 * reservas. Si no lo hacemos, nuestro programa generará pérdidas de
 * memoria (memory leaks). Cada vez que se llame a esa función se 
 * desperdiciará un poco más de memoria.
 * Utilizamos el comando:
 *   delete[] valArray;
 */


#include <iostream>
#include <ctime> //Para obtener la hora
#include <cstdlib> //Para función rand()

using namespace std;

void showArray(int *valArray,int valCount)
{
	//Mostramos el contenido del array
	for(int i=0;i<valCount;i++) cout << valArray[i] << " ";
	cout << endl;
}

int main(int argc, char **argv)
{
	//Declaramos una variable que definirá el tamaño del array
	int valCount = 10;
	
	//Creamos un array dinámico del tamaño deseado inicializado a ceros
	int *valArray = new int[valCount]();
	//Mostramos el array vacío
	showArray(valArray,valCount);
	
	//Generamos una nueva semilla en función de la hora actual
	srand((unsigned int)time(NULL));
	//Llenamos el array de valores aleatorios entre 0 y 9
	for(int i=0;i<valCount;i++) valArray[i]=rand() % 10;
	//Mostramos de nuevo el array
	showArray(valArray,valCount);
	
	//Ya no necesitamos el array. Liberamos su la memoria consumida
	delete[] valArray;
	
	//Todo ok
	return 0;
}

