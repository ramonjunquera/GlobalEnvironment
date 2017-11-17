/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Ejecutar una función en un thread
 * Material: Raspberry Pi
 * Descripción:
 * Los threads en C++ son similares a C.
 * Se simplifican algunos aspectos.
 * 
 * Incluiremos la librería thread.
 * 
 * Para que el programa compile tendremos que incluir manualmente como
 * parámetro del Build (Build/Set Build Commands/C++ Commands/Build):
 * -pthread
 * Si además indicamos que queremos hacer uso de las funciones de C++11,
 * el total de parámetros quedaría así:
 * g++ -Wall -std=c++11 -o "%e" "%f" -pthread
 * 
 * Recordemos que originalmente era:
 * g++ -Wall -o "%e" "%f"
 * 
 * Algunas de las diferencias con C son:
 * - La función a la que se llama con el thread, al ser void, no tiene
 *   porqué tener un "return NULL" obligatorio.
 * - La función a la que se llama con el thread puede ser definida como
 *   función normal. No sebe ser un puntero de función. Gracias a esto,
 *   podemos incluir en su interior bucles for que inicializan su
 *   variable contadora dentro del for.
 * - La definición del objeto thread y su lanzamiento se pueden hacer 
 *   en el mismo paso.
 * 
 * Para asegurarnos que finalizamos el programa después de que finalice
 * el thread lanzado, esperamos a que termine.
 */

#include <iostream>
#include <thread> //Para gestionar threads

using namespace std;

void Contador()
{
	//Función que cuenta hasta un determinado número
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
	cout << "Starting thread\n";
    
    //Contamos hasta un valor determinado
    for(int i=0;i<100000000;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    cout << "Ending thread\n";
}

int main(void)
{
	//Definimos el objeto thread1 para que llame a la función Contador
	//en un thread
	thread thread1(Contador);
    
    //Informamos
    printf("Thread launched\n");
    //Esperamos hasta que thread1 termine su ejecución
    thread1.join();
    
    //Notas acerca de la función joinable()
    //El método joinable() de un thread devuelve un bool
    //Si el thread no ha comenzado o ya ha finalizado y se le ha hecho
    //un join devolverá false.
    //Si el thread está en ejecucióin y aun no se le ha hecho un join
    //devolverá true.
    //No sirve para saber si un thread está en ejecución.
    
    //Hemos terminado. Todo ok
    return 0;
}
