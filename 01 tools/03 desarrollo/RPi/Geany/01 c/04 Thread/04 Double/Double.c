/*
 * Autor: Ramón Junquera
 * Tema: Programación en C. Threads
 * Objetivo: Lanzar dos threads simultáneos
 * Material: Raspberry Pi
 * Descripción:
 * Tomando como base el ejercicio anterior, esta vez lanzaremos dos
 * threads simultáneamente.
 * El primero de ellos tendrá que contar hasta un límite inferior al
 * segundo. Por eso terminará siempre antes.
 * 
 * Es imprescindible crear variables distintas para guardar las
 * estructuras que se pasan como parámetro. La razón es porque no
 * pasamos la estructura completa, sino su puntero.
 * En la función se recoge el puntero y se le aplica el tipo correcto.
 * Pero se leen los valores originales.
 * La estructura se pasa por referencia, no por valor.
 * 
 * Si no creamos variables distintas, al modificar en el programa
 * principal los distintos atributos de la estructura para lanzar el
 * segundo thread, el primero los tendrá en cuenta!
 * 
 * Esto tiene una ventaja. Si la función debe devolver algo, se puede
 * almacenar en la estructura de parámetros. Mientras el thread se 
 * ejecuta e incluso cuando ha terminado, la estructura estará 
 * disponible desde el programa principal.
 * 
 * Es importante comprobar que aunque ahora lanzamos dos threads
 * aproximadamente del mismo peso que en el ejercicio anterior, el
 * tiempo de proceso es el mismo, porque se utilizan 2 procesadores en
 * vez de uno.
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
	//Definimos las variables para los thread
	pthread_t thread1,thread2;
	//Definimos las variables que se pasarán como parámetro 
	struct params p1,p2;
	//Variable que contendrá el posible código de error
	int errorCode;
	
	//Llenamos los parámetros para el primer thread
	p1.maxCount = 90000000; //Límite del contador
	p1.threadId = 111; //Identificador del thread
	//Llenamos los parámetros para el segundo thread
	p2.maxCount = 100000000; //Límite del contador
	p2.threadId = 222; //Identificador del thread
	
	//Lanzamos la función Contador en el primer thread
    errorCode = pthread_create(&thread1,NULL,&Contador,(void *)&p1);
    //Si hubo algún error...informamos
    if (errorCode) printf("Error %i al crear el thread %i\n",errorCode,p1.threadId);
    //El primer thread ha sido lanzado

	//Lanzamos la función Contador en el segundo thread
    errorCode = pthread_create(&thread2,NULL,&Contador,(void *)&p2);
    //Si hubo algún error...informamos
    if (errorCode) printf("Error %i al crear el thread %i\n",errorCode,p2.threadId);
    //El segundo thread ha sido lanzado
    
    //Informamos que estamos esperando
    printf("waiting\n");
    //Esperamos el tiempo suficiente como para que el thread finalice
    sleep(3);
    //Informamos que terminamos el programa principal (proceso)
    printf("end\n");
    
    //Hemos terminado. Todo ok
    return 0;
}
