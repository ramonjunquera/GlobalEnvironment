/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Actualización de valores compartidos
 * Material: Raspberry Pi
 * Descripción:
 * Los ejemplos que hemos desarrollado hasta el momento siempre 
 * utilizaban una función para el thread que no actualizaba ningún
 * valor.
 * En todo caso, podrían modificar datos en la estructura de parámetros.
 * Pero siempre serán datos propios del thread.
 * 
 * En este la función modificaremos una variable global que es común a
 * todos los threads. Se trata de un contador. Aumentará su valor por
 * cada ciclo de la función Contador.
 * 
 * El problema es que podríamos tener conflictos a la hora de escribir
 * o leer desde distintos threads las mismas posiciones de memoria.
 * Se podría corromper su contenido si se intentan hacer varias 
 * modificaciones simultáneas.
 * 
 * Para evitar esto se crea el objeto "mutex".
 * Este objeto se puede activar y desactivar.
 * Cuando se activa, se impide la ejecución de se esa sección a
 * cualquier otro thread.
 * Otros threads que lleguen a este punto con el mutex activado,
 * quedarán a la espera de que se desactive sin hacer nada más.
 * Una sección protegida por un mutex sólo puede ser ejecutada por un
 * único thread.
 * 
 * Por esta razón es muy importante que las zonas protegidas por mutex
 * sean lo más pequeñas y rápidas posibles.
 * 
 * Incluir un mutex significa reducir el rendimiento global debido a 
 * pérdida de tiempo esperando a que se desbloquee.
 * 
 * Un objeto mutex puede ser activado y desactivado en distintos puntos
 * de un thread, pero cuando se bloquee, todas las zonas afectadas serán
 * bloqueadas.
 * 
 * Se pueden crear distintos mutex para distintas zonas o funciones.
 * 
 * Por defecto un mutex debería inicializarse con el valor:
 *   PTHREAD_MUTEX_INITIALIZER
 */

#include<stdio.h>
#include<pthread.h> //Para gestionar threads
#include<unistd.h> //Para sleep()

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
	const int maxThreads = 4;
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
    //Informamos que estamos esperando
    printf("Waiting...\n");
    //Esperamos el tiempo suficiente como para que el thread finalice
    sleep(3);
    //Informamos que terminamos el programa principal (proceso)
    //y mostramos el valor del contador global
    printf("End. globalCounter=%i\n",globalCounter);
    
    //Hemos terminado. Todo ok
    return 0;
}
