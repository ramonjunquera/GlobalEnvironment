/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Ejecutar una función en un thread
 * Material: Raspberry Pi
 * Descripción:
 * La Raspberry Pi 2 y 3 tienen 4 cores.
 * Por defecto un proceso (programa) se lanza en un thread (hilo), que
 * es gestionado por un procesador (core).
 * Eso significa que si tenemos varios procesadores, un programa se
 * ejecutará en uno de ellos. Posiblemente consumiendo el 100% de CPU.
 * Pero el resto de procesadores no serán afectados.
 * Si creamos un programa que simplemente cuente hasta un número muy
 * alto y lo lanzamos en una Raspberry Pi 2 o 3, veremos que se consume
 * aproximadamente el 25% del total de CPU, porque sólo está funcionando
 * una.
 * 
 * En el ejercicio actual aprenderemos a crear programas multi-thread.
 * Son aquellos capaces de lanzar distintas tareas independentes para
 * que sean ejecutadas por cualquier procesador disponible,
 * 
 * Necesitaremos incluir la librería pthread.h
 * Esta librería es un poco particular. Necesita hace un pequeño cambio
 * en la sección Build. Hay que añadir manualmente:
 * -l pthread
 * Con esto cargará la librería al crear el ejecutable.
 * La línea Build podría quedar así:
 * gcc -Wall -o "%e" "%f" -l bcm2835 -l pthread
 *  
 * Lo primero que hacemos es definir la función que será ejecutada en el
 * thread. En nuestro caso: Contador.
 * Se debe definir como puntero de función, por eso es *Contador.
 * Nunca devuelve nada, por eso comienza con void.
 * Siempre deberíamos añadir un único parámetro que no tiene tipo, pero
 * es un puntero (void *arg)
 * Esto se hace así para poder pasar parámetros a la función.
 * En esta ocasión no pasaremos parámetros, pero dejamos la definición
 * como está. Ya trataremos este tema más adelante.
 * 
 * La función Contador sólo cuenta hasta un determinado valor, pero
 * informa por pantalla antes y después de hacerlo.
 * 
 * Aunque la función contador no devuelva nada, es obligatorio que
 * finalice con el comando con la secuencia:
 *   return NULL
 * 
 * En la función principal, definimos una variable que mantendrá el
 * objeto que gestiona el thread.
 * 
 * A continuación creamos el thread con el comando pthread_create
 * Tiene 4 parámetros:
 * 1- puntero de la variable que contendrá el objeto del thread
 * 2- parámetros del thread (prioridad, etc...)
 * 3- puntero de la función a ejecutar
 * 4- puntero (sin tipo) del parámetro de la función
 * 
 * pthread_create siempre devuelve un entero con el código de error.
 * Si no hubo errores creando el thread devuelve cero.
 * 
 * En el instante en el que creamos el thread comienza a ejecutarse en
 * paralelo a la ejecución del programa principal.
 * 
 * Cuando finaliza el programa (proceso) que ha generado los threads,
 * automáticamente los cierra aunque aun no hayan terminado.
 * 
 * Como por ahora no sabemos con seguridad cuándo terminará el thread
 * lanzado, esperamos un tiempo prudencial.
 * 
 * El resultado es que comprobaremos que el thread se crea porque
 * podremos ver los mensajes de inicio y finalización de la función.
 * Poco después finaliza el programa principal.
 */

#include<stdio.h>
#include<pthread.h> //Para gestionar threads
#include<unistd.h> //Para sleep()

void *Contador(void *arg)
{
	//Función que cuenta hasta un determinado número
	//Es llamada para ser ejecutada desde un thread
	
	//Informamos que comenzamos el thread
    printf("Starting thread\n");
    
    //En funciones definidas por punteros no se puede declarar la
	//variable de un bucle for en su interior. Debe hacerse fuera.
    int i;
    //Contamos hasta un valor determinado
    for(i=0;i<100000000;i++)
    {
		//No hay nada especial que hacer dentro del bucle
	}
	//Hemos terminado de contar

    //Informamos que hemos terminado el thread
    printf("Ending thread\n");
    
    //Todas las funciones a las que llama un thread devuelven void
    //y están obligadas a devolver un nulo al finalizar
    return NULL;
}

int main(void)
{
	//Definimos una variable para el thread
	pthread_t thread1;
	//Lanzamos la función Contador en el thread
    int errorCode = pthread_create(&thread1,NULL,&Contador,NULL);
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
