/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Lanzar varios threads simultáneos
 * Material: Raspberry Pi
 * Descripción:
 * Lanzaremos un número de threads simultáneos determinado por el valor
 * de una constante.
 * Guardaremos todos los objetos de thread en un array.
 * Podremos ver el inicio y fin de ejecución de cada thread gracias a
 * que pasamos como parámetro el identificador del thread lanzado.
 */

#include <iostream>
#include <thread> //Para gestionar threads

using namespace std;

void Contador(int threadId)
{
	//Función que cuenta hasta un número determinado.
	//Se le pasa como parámetro el identificador de thread.
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	cout << "Start thread " << threadId << endl;
    
    //Contamos hasta un valor determinado
    for(int i=0;i<100000000;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    cout << "End thread  " << threadId << endl;
}

int main(void)
{
	//Indicamos el número de threads a lanzar
	const int threadsCount = 5;
	
	//Creamos un array de threads
	thread myThreads[threadsCount];
	
	//Los lanzamos
	for(int i=0;i<threadsCount;i++) myThreads[i]=thread(Contador,i);
	
	//Informamos
    cout << "Threads launched\n";
	
	//Esperamos a que todos terminen
	for(int i=0;i<threadsCount;i++) myThreads[i].join();
	
    //Hemos terminado. Todo ok
    return 0;
}
