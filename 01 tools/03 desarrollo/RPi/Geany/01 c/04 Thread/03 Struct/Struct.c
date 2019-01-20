/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Pasar una estructura a una función de thread
 * Material: Raspberry Pi
 * Descripción:
 * Tomando como base el ejercicio anterior, esta vez en vez de pasar un
 * simple valor como parámetro a la función de thread, pasaremos una
 * estructura completa.
 * 
 * La estructura contendrá el límite del contador y un entero que
 * identificará el thread.
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
	//Definimos una variable para el thread
	pthread_t thread1;
	//Definimos la variable que se pasará como parámetro 
	struct params p;
	//Llenamos sus valores
	p.maxCount = 100000000; //Límite del contador
	p.threadId = 142; //Identificador del thread
	
	//Lanzamos la función Contador en el thread
    int errorCode = pthread_create(&thread1,NULL,&Contador,(void *)&p);
    //Si hubo algún error...informamos
    if (errorCode) printf("Error %i al crear el thread",errorCode);
    //El thread ha sido lanzado
    
    //Informamos que estamos esperando
    printf("waiting\n");
    //Esperamos el tiempo suficiente como para que el thread finalice
    sleep(3);
    //Informamos que terminamos el programa principal (proceso)
    printf("end\n");
    
    //Hemos terminado. Todo ok
    return 0;
}
