/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Pasar un parámetro a una función de thread
 * Material: Raspberry Pi
 * Descripción:
 * Basándonos en el ejercicio anterior, en este caso la función a la que
 * llamamos en el thread aceptará un parámetro, que será un número 
 * entero que indicará hasta qué número se debe contar.
 * 
 * En la función decodificamos el argumento.
 * Sabemos que es un entero y hacemos un cast.
 * 
 * Desde la llamada, cambiamos el tipo del entero a un puntero void.
 * 
 * Hemos modificado la función para que muestre el número hasta el que 
 * contará. Así también podemos verificar que lo hemos decodificado
 * correctamente.
 */

#include<stdio.h>
#include<pthread.h> //Para gestionar threads
#include<unistd.h> //Para sleep()

void *Contador(void *arg)
{
	//Función que cuenta hasta un número indicado como parámetro
	//Es llamada para ser ejecutada desde un thread
	
	//Decodificamos el parámetro
    //Realmente es un entero
    int maxCount = (int)arg;
	
	//Informamos que comenzamos el thread
    printf("Counting until %i\n",maxCount);
    
    //En funciones definidas por punteros no se puede declarar la
	//variable de un bucle for en su interior. Debe hacerse fuera.
    int i;
    //Contamos hasta un valor determinado
    for(i=0;i<maxCount;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    printf("Counted until %i\n",maxCount);
    
    //Todas las funciones a las que llama un thread devuelven void
    //y están obligadas a devolver un nulo al finalizar
    return NULL;
}

int main(void)
{
	//Definimos una variable para el thread
	pthread_t thread1;
	//Definimos la variable que se pasará como parámetro 
	//Es el número hasta el que tiene que contar
	int maxCount = 100000000;
	//Lanzamos la función Contador en el thread
    int errorCode = pthread_create(&thread1,NULL,&Contador,(void *)maxCount);
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
