/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Ejecutar una función con varios parámetros en un thread
 * Material: Raspberry Pi
 * Descripción:
 * La función a la que llama el thread tiene más de un parámetro.
 * 
 * Las diferencias con C son:
 * - La función a la que se llama con el thread pude tener varios 
 *   parámetros normales: no tienen porqué ser punteros y pueden ser de
 *   tipos distintos a void.
 * - En la creación y llamada a la función del thread se le pasan todos
 *   los parámetros necesarios.
 */

#include <iostream>
#include <thread> //Para gestionar threads

using namespace std;

void Contador(string threadName,int maxCount)
{
	//Función que cuenta hasta un número indicado como parámetro
	//También se le indica el nombre que recibirá
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	cout << "Launched thread " << threadName << endl;
    
    //Contamos hasta un valor determinado
    for(int i=0;i<maxCount;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    cout << "Counted until " << maxCount << endl;
}

int main(void)
{
	//Definimos el objeto thread1 para que llame a la función Contador
	//en un thread.
	//Le pasamos los valores de sus parámetros
	thread thread1(Contador,"myThread",100000000);
    
    //Informamos
    printf("Thread launched\n");
    //Esperamos hasta que thread1 termine su ejecución
    thread1.join();
    
    //Hemos terminado. Todo ok
    return 0;
}
