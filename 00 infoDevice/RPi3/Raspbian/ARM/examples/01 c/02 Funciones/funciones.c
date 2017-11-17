/*
 * Autor: Ramón Junquera
 * Tema: Programación en C
 * Objetivo: Aprender a definir funciones
 * Material: Raspberry Pi
 * Descripción:
 * Podemos definir funciones de la manera clásica pero con ciertas
 * restricciones.
 * 
 * Una línea de código siempre se debe basar en funciones declaradas
 * anteriormente.
 * Por lo tanto, una función que se utiliza en main() podemos definirla
 * antes de main().
 * 
 * También podemos definirla después, pero a condición de que declaremos
 * la cabecera antes de main().
 * 
 * En nuestro ejemplo utilizamos este segundo caso.
 * Personalmente pienso que es más complicado, porque conlleva una
 * doble declaración de la cabecera que puede inducir a errores y más
 * código del necesario.
 */


#include <stdio.h>

//Declaramos la función que definiremos después
int sum(int a,int b);

int main(int argc, char **argv)
{
	int a = 2;
	int b = 3;
	int c = sum(a,b);
	printf("%i + %i = %i\n",a,b,c);
	return 0;
}

//Código completo de la función
int sum(int a,int b)
{
	return a+b;
}


	

