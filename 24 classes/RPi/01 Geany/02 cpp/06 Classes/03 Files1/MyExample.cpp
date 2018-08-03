/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Definir clases externas
 * Material: Raspberry Pi
 * Descripción:
 * En el ejercicio anterior hemos aprendido a crear archivos externos
 * que contienen clases de la manera tradicional: con un archivo de
 * cabecera (.h) y otro de definiciones (.cpp).
 * 
 * En éste intentaremos simplificarlo.
 * Sólo usaremos un archivo .cpp para contener la declaración y 
 * definición de la clase.
 * 
 * Este archivo contiene la misma definición de la clase que hacíamos en
 * el primer ejercicio.
 * Lo único que le añadimos son las intrucciones condicionales para el
 * compilador. Así evitaremos duplicidad de declaración si es que
 * llamamos a esta clase desde varios puntos del desarrollo.
 * 
 * También sigue siuendo necesario la modificación de los parámetros al
 * comando Build para que tenga en cuenta todos los archivos .cpp de la 
 * carpeta.
 * 
 * El resultado final es más sencillo de mantener, porqueya no hay
 * archivos de cabecera y la clase se declara y define en el mismo
 * lugar.
 */

#include <iostream>
#include "MyClass.cpp"

using namespace std;

int main(int argc, char **argv)
{
	//Creamos el objeto que contiene la clase
	//Y le asignamos un valor inicial a la variable interna
	MyClass c = MyClass(3);
	//Mostramos el valor de la variable interna
	cout << c.GetValue() << endl;
	//Añadimos un valor a la variable interna
	c.Add(2);
	//Mostramos el valor de la variable interna
	cout << c.GetValue() << endl;
	
	//Todo ok
	return 0;
}

