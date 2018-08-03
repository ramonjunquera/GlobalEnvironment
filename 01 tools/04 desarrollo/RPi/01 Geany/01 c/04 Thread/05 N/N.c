/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Lanzar n threads simultáneos
 * Material: Raspberry Pi
 * Descripción:
 * Tomando como base el ejercicio anterior, esta vez lanzaremos un
 * número de threads simultáneamente definido por una constante.
 * Utilizaremos arrays para guardar tanto los objetos de thread como
 * las estructuras de parámetros.
 * 
 * Descubrimos que esperando al final del programa 3 segundos para
 * terminar podemos lanzar con seguridad un máximo de 5 threads.
 * Con 6 comenzamos a perder alguno.
 * 
 * Si lanzamos 15 threads necesitaremos esperar un mínimo de 7 segundos.
 * 
 * Qué ocurre cuando lanzamos más threads que procesadores?
 * Cuando se crea un nuevo thread, el sistema operativo se encarga de 
 * asignarlo a un procesador para que lo ejecute. Ese procesador es el
 * que menos trabajo tenga en ese momento.
 * Si ya tenemos ocupados todos los procesadores, el sistema operativo
 * no deja en cola de espera el nuevo thread hasta que algún procesador
 * queda libre. Sigue usando la misma regla.
 * Un procesador es capaz de procesar varios threads simultáneamente.
 * Puesto que tendrá que repartir su tiempo de proceso entre varios
 * threads, éstos se ejecutarán más lentamente.
 */

#include<stdio.h>
#include<pthread.h> //Para gestionar threads
#include<unistd.h> //Para sleep()

//Creamos una estructura que guardará los parámetros que pasaremos a la
//función del thread
struct params
{
	int maxCount; //Valor hasta el que contar
	int threadId; //Identificador del thread
};

void *Contador(void *arg)
{
	//Función que cuenta hasta un número indicado en la estructura que
	//se pasa como parámetro.
	//Es llamada para ser ejecutada desde un thread
	
	//Decodificamos el parámetro
    //Realmente es una estructura params
    struct params *p = (struct params *)arg;
	
	//Informamos que comenzamos el thread
    printf("Thread %i. Counting until %i\n",(*p).threadId,(*p).maxCount);
    
    //En funciones definidas por punteros no se puede declarar la
	//variable de un bucle for en su interior. Debe hacerse fuera.
    int i;
    //Contamos hasta un valor determinado
    for(i=0;i<(*p).maxCount;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    printf("Thread %i. End\n",(*p).threadId);
    
    //Todas las funciones a las que llama un thread devuelven void
    //y están obligadas a devolver un nulo al finalizar
    return NULL;
}

int main(void)
{
	//Definimos el número de threads que queremos crear
	const int maxThreads = 15;
	//Creamos un array para guardar los objetos de thread
	pthread_t threads[maxThreads];
	//Creamos un array para guardar las estructuras con los parámetros
	struct params p[maxThreads];
	//Variable que contendrá el posible código de error
	int errorCode;
	
	//Recorremos todos los threads..
	int t;
	for(t=0;t<maxThreads;t++)
	{
		//Llenamos los parámetros
		p[t].maxCount = 100000000; //Límite del contador
		p[t].threadId = t+1; //Identificador del thread
		//Lanzamos el thread
		errorCode = pthread_create(&threads[t],NULL,&Contador,(void *)&p[t]);
		//Si hubo algún error...informamos
		if (errorCode) printf("Error %i al crear el thread %i\n",errorCode,p[t].threadId);
	}
    //Informamos que estamos esperando
    printf("waiting\n");
    //Esperamos el tiempo suficiente como para que el thread finalice
    sleep(7);
    //Informamos que terminamos el programa principal (proceso)
    printf("end\n");
    
    //Hemos terminado. Todo ok
    return 0;
}
