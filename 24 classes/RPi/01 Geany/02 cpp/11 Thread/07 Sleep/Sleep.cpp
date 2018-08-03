/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Ejemplo de función sleep_for
 * Material: Raspberry Pi
 * Descripción:
 * Un programa cuando se ejecuta se considera un thread simple.
 * El programa utilizará el 100% de la CPU a la que ha sido asignada por
 * el sistema operativo.
 * Una Raspberry Pi 3 tiene 4 CPUs.
 * Si el programa genera nuevos threads, se asignarán a otras CPUs para
 * que puedan ser ejecutados en paralelo.
 * Si un thread (o programa principal) debe esperar un determinado
 * tiempo, es muy conveniente utilizar la función sleep_for que se
 * encarga de crear una interrupción interna para volver a despertar.
 * Esto permite que el thread deje de consumir CPUs y la deje libre a 
 * otros threads que puedan utilizara.
 * 
 * El namespace std incluye le variable this_thread que contiene el
 * thread en el que se está ejecutando el código actual.
 * 
 * La función sleep_for necesita de un parámetro que indique el tiempo
 * en segundos que estará "durmiendo". El tiempo tendrá el formato
 * utilizado en la librería chronos (obligatoria).
 * 
 * En este ejemplo creamos un array de threads. Los identificamos con
 * su índice en el array y los lanzamos todos a la vez.
 * El thread llama a una función que muestra el identificador del thread
 * y la hace dormir tantos segundos como indique su segundo parámetro.
 * 
 * Dormiremos la función antos segundos como el valor del identificador.
 * 
 * La función muestra el inicio y el fin de la ejecución.
 * 
 * Comprobamos que se lanzan todos los threads al mismo tiempo y van
 * terminando uno por segundo, pero lo importante es que no consumen CPU
 */


#include <thread> //Para gestionar threads
#include <chrono> //Para los segundos de sleep_for
#include <iostream>

using namespace std;

void SleepSeconds(int id,int s)
{
	//Función muestra el identificador que recibe y duerme el thread en
	//el que se encuentra durante s segundos
	
	//Informamos que comenzamos el thread
	cout << "Start thread " << id << endl;
    
    this_thread::sleep_for(chrono::seconds(s));
    
    //Informamos que hemos terminado el thread
    cout << "End thread  " << id << endl;
}

int main(void)
{
	//Indicamos el número de threads a lanzar
	const int threadsCount = 10;
	
	//Creamos un array de threads
	thread myThreads[threadsCount];
	
	//Los lanzamos
	for(int i=0;i<threadsCount;i++) myThreads[i]=thread(SleepSeconds,i,i);
	
	//Informamos
    cout << "Threads launched\n";
	
	//Esperamos a que todos terminen
	for(int i=0;i<threadsCount;i++) myThreads[i].join();
	
    //Hemos terminado. Todo ok
    return 0;
}
