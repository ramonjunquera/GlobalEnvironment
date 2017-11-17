/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Uso de atomic
 * Material: Raspberry Pi
 * Descripción:
 * Según hemos comprobado en el ejercicio anterior, el uso de bloqueos
 * en threads con variables compartidas hace que el rendimiento
 * descienda mucho.
 * Para evitarlo y simplificar la gestión de estas situaciones se
 * crea la librería atomic.
 * 
 * Con esta librería se puede declarar cualquier variable como atomic,
 * que significa que ella misma se encargará de los bloqueos en sus
 * lecturas y escrituras.
 * Desaparecen los bloqueos de parte del código y se multiplica el
 * rendimiento.
 * 
 * En el momento de declarar la variable atomic, también podemos
 * asignarle valor.
 * 
 * Los bloqueos con mutex nos permitían controlar también que las
 * excrituras en la consola de salida no se pisaban unas con otras.
 * Como ahora no los tenemos, componemos el texto a mostrar antes de 
 * enviarlo a pantalla.
 */

#include <iostream>
#include <thread> //Para gestionar threads
#include <atomic>

using namespace std;

//Declaramos una variable global de tipo int como atomic (protección
//contra concurrencia) y le asignamos el valor inicial.
atomic<int> globalCounter(0);

void Contador(int threadId)
{
	//Función que cuenta hasta un número determinado.
	//Se le pasa como parámetro el identificador de thread.
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	string message = "Start thread " + to_string(threadId) + "\n";
	cout << message;
    
    //Contamos hasta un valor determinado
    for(int i=0;i<1000000;i++)
    {
		//Tenemos que aumentar el valor del contador global
		globalCounter++;
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    message = "End thread " + to_string(threadId) + ". globalCounter=";
    message += to_string(globalCounter) + "\n";
    cout << message;
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
    cout << "Threads launched\n";
	
	//Esperamos a que todos terminen
	for(int i=0;i<threadsCount;i++) myThreads[i].join();
	
	//Informamos que hemos terminado y el valor del contador global
	cout << "End program. globalCounter=" << globalCounter << endl;
	
    //Hemos terminado. Todo ok
    return 0;
}
