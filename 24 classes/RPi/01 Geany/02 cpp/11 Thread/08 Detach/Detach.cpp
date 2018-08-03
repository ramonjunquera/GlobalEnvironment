/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Funcionamiento del método detach de un thread
 * Material: Raspberry Pi
 * Descripción:
 * Cuando aplicamos el método detach a un thread que está en ejecución
 * conseguimos desvincularlo totalmente del programa principal.
 * El programa principal y el thread correrán por separado.
 * El programa principal perderá el control sobre el thread y ya no
 * podrá hacer un join. Incluso el método joinable devolverá siempre
 * el valor falso.
 * 
 * Si el programa principal finaliza antes que el thread, NO dará ningún
 * error. Simplemente el thread morirá sin finalizar.
 * 
 * En el ejemplo lanzaremos como thread una función que tarda un rato.
 * Desvincularemos el thread.
 * Esperaremos un tiempo superior al que tarde el thread en ejecutarse
 * para poder ver en pantalla el mensaje de que el thread ha terminado.
 * Posteriormente finalizará el programa principal.
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
	//Lo desvinculamos del programa principal
	thread1.detach();
    
    //Contamos hasta un valor determinado (mayor que el bucle del thread)
    for(int i=0;i<200000000;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
    
    //Informamos
    cout << "Ending main program" << endl;
    
    //Hemos terminado. Todo ok
    return 0;
}
