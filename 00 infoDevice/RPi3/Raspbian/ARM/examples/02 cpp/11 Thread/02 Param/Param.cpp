/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Ejecutar una función con parámetro en un thread
 * Material: Raspberry Pi
 * Descripción:
 * Siguiendo con el ejercicio anterior, esta vez definimos la función
 * llamada por el thread con un parámetro.
 * 
 * Las diferencias con C son:
 * - La función a la que se llama con el thread puede tener parámetros
 *   normales: no tiene porqué ser un puntero y puede ser de un
 *   tipo distinto a void. Por lo tanto, no será necesario volver a
 *   darle el tipo correcto dentro de la función.
 * - En la creación y llamada a la función del thread se le puede pasar
 *   el parámetro del tipo correcto y sin conversiones.
 */

#include <iostream>
#include <thread> //Para gestionar threads

using namespace std;

void Contador(int maxCount)
{
	//Función que cuenta hasta un número indicado como parámetro
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	cout << "Counting until " << maxCount <<endl;
    
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
	//Le pasamos el valor de su parámetro
	thread thread1(Contador,100000000);
    
    //Informamos
    cout << "Thread launched\n";
    //Esperamos hasta que thread1 termine su ejecución
    thread1.join();
    
    //Hemos terminado. Todo ok
    return 0;
}
