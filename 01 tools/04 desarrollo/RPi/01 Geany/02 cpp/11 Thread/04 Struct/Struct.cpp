/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Ejecutar una función que tiene como parámetro una
 *   estructura, en un thread.
 * Material: Raspberry Pi
 * Descripción:
 * La función a la que llama el thread tiene un parámetro que es una
 * estrutura.
 * 
 * Las diferencias con C son:
 * - No es necesario decodificar el tipo del parámetro en la función.
 */

#include <iostream>
#include <thread> //Para gestionar threads

using namespace std;

struct params
{
	//Estructura que guarda los parámetros de ejecución de la función de
	//thread.
	
	int maxCount; //Valor hasta el que contar
	string threadName; //Identificador del thread
};

void Contador(struct params p)
{
	//Función que cuenta hasta un número indicado en la estructura que
	//se le pasa como parámetro.
	//También se le indica el nombre que recibirá (en la estructura).
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	cout << "Launched thread " << p.threadName << endl;
    
    //Contamos hasta un valor determinado
    for(int i=0;i<p.maxCount;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    cout << "Counted until " << p.maxCount << endl;
}

int main(void)
{
	//Creamos una estructura para pasar como parámetro
	struct params myParams;
	//Le asignamos los valores
	myParams.threadName = "myThread";
	myParams.maxCount = 100000000;
	
	//Definimos el objeto thread1 para que llame a la función Contador
	//en un thread.
	//Le pasamos los valores de sus parámetros
	thread thread1(Contador,myParams);
    
    //Informamos
    cout << "Thread launched\n";
    //Esperamos hasta que thread1 termine su ejecución
    thread1.join();
    
    //Hemos terminado. Todo ok
    return 0;
}
