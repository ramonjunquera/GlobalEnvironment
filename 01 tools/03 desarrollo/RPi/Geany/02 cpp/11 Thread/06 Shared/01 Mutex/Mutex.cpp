/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Uso de mutex
 * Material: Raspberry Pi
 * Descripción:
 * Como en C, en C++ también existe la librería mutex para bloquear
 * secciones de código en procesos de threads concurrentes.
 * 
 * En este ejemplo lanzaremos varios threads simultáneos cuyo número 
 * está definido por una constante.
 * Los threads incrementarán una variable global.
 * En pantalla se escribe el progreso tanto desde la función principal
 * como desde la función de thread.
 * 
 * Se debe incluir la librería mutex para que reconozca este tipo de
 * objetos.
 * 
 * Definiremos un par de mutex globales.
 * Uno para el bloqueo del contador.
 * Otro para el bloqueo de la consola de salida. Para que no se pisen el
 * texto que se envía a pantalla entre los distintos threads.
 * La función principal también se debe considerar como thread.
 * 
 * Como ya explicamos en el ejemplo de mutex para C, el uso de bloqueos
 * hace que el rendimiento global del proceso descienda mucho.
 */

#include <iostream>
#include <thread> //Para gestionar threads
#include <mutex> //Para utilizar mutex

using namespace std;

//Declaración de valiables globales
int globalCounter=0;
mutex mutexCounter,mutexCout;

void Contador(int threadId)
{
	//Función que cuenta hasta un número determinado.
	//Se le pasa como parámetro el identificador de thread.
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	mutexCout.lock();
	cout << "Start thread " << threadId << endl;
	mutexCout.unlock();
    
    //Contamos hasta un valor determinado
    for(int i=0;i<1000000;i++)
    {
		//Tenemos que aumentar el valor del contador global
		mutexCounter.lock();
		globalCounter++;
		mutexCounter.unlock();
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    mutexCout.lock();
    cout << "End thread " << threadId << ". globalCounter=" << globalCounter << endl;
    mutexCout.unlock();
}

int main(void)
{
	//Indicamos el número de threads a lanzar
	const int threadsCount = 4;
	
	//Creamos un array de threads
	thread myThreads[threadsCount];
	
	//Los lanzamos
	for(int i=0;i<threadsCount;i++) myThreads[i]=thread(Contador,i);
	
	//Informamos
	mutexCout.lock();
    cout << "Threads launched\n";
    mutexCout.unlock();
	
	//Esperamos a que todos terminen
	for(int i=0;i<threadsCount;i++) myThreads[i].join();
	
	//Informamos que hemos terminado y el valor del contador global
	cout << "End program. globalCounter=" << globalCounter << endl;
	
    //Hemos terminado. Todo ok
    return 0;
}
