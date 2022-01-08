/*
 * Autor: Ramón Junquera
 * Tema: Programación en C
 * Objetivo: Definir la función Micros()
 * Material: Raspberry Pi
 * Descripción:
 * La función Micros() devuelve el tiempo en microsegundos transcurrido
 * desde que se arrancó.
 * 
 * Para poder probar la función, utilizaremos las funciones de sleep
 * definidas en la librería unistd.h
 * Podemos usar sleep(sec) donde sec es el número de segundos a esperar
 * O usleep(usec), donde usec son microsegundos.
 */

#include <stdio.h>
#include <sys/time.h> //Para cálculo de tiempos
#include <unistd.h> //Para sleep

long long Micros()
{
	//Devuelve el tiempo transcurrido desde que se ha arrancado
	//en microsegundos
	
	//Estructura para recoger la hora actual
	struct timeval tv;
	//Leemos la hora actual
	gettimeofday(&tv,NULL);
	//En tv.tv_sec tenemos la hora actual en segundos
	//En tv.tv_usec tenemos los microsegundos del segundo actual
	//Multiplicamos los segundos por un millón y le sumamos los
	//microsegundos
	return 1e6*tv.tv_sec + tv.tv_usec;
}

int main(int argc, char **argv)
{
	while(1)
	{
		//Mostramos los microsegundos transcurridos desde el inicio
		printf("now = %lld\n",Micros());
		//Esperamos un segundo
		sleep(1);
		//También podríamos haber utilizado usleep(usec)
		//Donde usec es el tiempo a esperar en microsegundos
		//En nuestro caso sería usleep(1e6);
	}
	//Aunque nunca lleguemos aquí, salimos sin errores
	return 0;
}

