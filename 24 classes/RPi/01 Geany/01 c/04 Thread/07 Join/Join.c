/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Detección de finalización de thread
 * Material: Raspberry Pi
 * Descripción:
 * Hasta ahora hemos tenido que hacer una estimación del tiempo que 
 * tardarían los threads lanzados en finalizar, para no cerrar el
 * programa principal antes, porque sino los cerrabamos a la fuerza.
 * Por esa razón utilizábamos el comando sleep.
 * 
 * Para evitar esto, tenemos una función que detiene la ejecución del
 * programa principal (o thread actual) hasta que un thread concreto
 * finalice. Esta función es: pthread_join
 * Si pedimos esperar por un thread que ha finalizado, no espera nada.
 * 
 * Puesto que ya no necesitamos el comando sleep, tampoco incluimos su
 * librería (unistd.h).
 * 
 * Ahora no importará el número de threads que se creen, porque el
 * programa principal esperará a que finalicen todos ellos.
 */

#include<stdio.h>
#include<pthread.h> //Para gestionar threads

//Declaración de variables globales
int globalCounter=0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

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
		//Tenemos que aumentar el valor del contador global
		//Antes de hacerlo debemos bloquear 
		pthread_mutex_lock(&mutex1);
		//Aumentamos el valor del contador global
		globalCounter++;
		//Desbloqueamos
		pthread_mutex_unlock(&mutex1);
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
	const int maxThreads = 8;
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
		p[t].maxCount = 1000000; //Límite del contador
		p[t].threadId = t+1; //Identificador del thread
		//Lanzamos el thread
		errorCode = pthread_create(&threads[t],NULL,&Contador,(void *)&p[t]);
		//Si hubo algún error...informamos
		if (errorCode) printf("Error %i al crear el thread %i\n",errorCode,p[t].threadId);
	}
	//Hemos lanzado todos los threads
	
	//Recorremos todos los threads lanzados...
	for(t=0;t<maxThreads;t++)
	{
		//...esperamos a que finaliza cada uno de ellos
		pthread_join(threads[t],NULL);
	}
	//Todos los threads han finalizado
	
    //Informamos que terminamos el programa principal (proceso)
    //y mostramos el valor del contador global
    printf("End. globalCounter=%i\n",globalCounter);
    
    //Hemos terminado. Todo ok
    return 0;
}
