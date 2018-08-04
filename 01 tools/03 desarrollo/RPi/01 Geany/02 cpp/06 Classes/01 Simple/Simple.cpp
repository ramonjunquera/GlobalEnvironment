/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Classes
 * Material: Raspberry Pi
 * Descripción:
 * Una clase es un estructura que además de datos puede contener
 * funciones. En un excelente método para encapsular código para que
 * pueda ser reutilizado.
 * 
 * Tanto la declaración de variables como de funciones puede ser pública
 * o privada.
 * La sección privada no es accesible desde el exterior de la clase.
 * 
 * Al igual que las funciones, debemos definirlas antes de ser
 * utilizadas. O en su defecto, declararlas para definirlas más tarde.
 * 
 * En este ejemplo crearemos una clase llamada MyClass.
 * Contiene una variable tipo int declarada en la sección privada, por
 * lo tanto no es accesible desde el exterior.
 * 
 * En la sección pública hemos declarado varios métodos.
 * 
 * El primero de ello es el constructor, que permite asignar un valor
 * inicial a la variable privada.
 * 
 * El método GetValue devuelve el valor de la variable interna.
 * 
 * El método Add añade un valor a la variable interna.
 * 
 * Dentro de la definición de la clase se deben incluir las etiquetas
 * private: y public: para distinguir a partir de qué punto comienza
 * cada una de las secciones.
 * Tras la etiqueta public: todas las declareaciones de variables o
 * métodos se considerarán públicos.
 * 
 * Si tenemos muchas declaraciones es posible que nos resulte complicado
 * saber en qué sección estamos.
 * Para evitarlo, no hay problema en añadir la etiqueta justo antes de
 * la declaración de cada item.
 * Así está escrito en el ejemplo.
 * No hay problema en intercalar o repetir las etiquetas.
 * 
 * 
 * 
 */

#include <iostream>

using namespace std;

class MyClass
{
	//Definición de la clase MyClass
	
	//Variable interna
	private: int internalValue;
	
	public: MyClass(int initialValue)
	{
		//Constructor
		//Permite asignar un valor inicial a la variable interna
		
		internalValue=initialValue;
	}
	
	public: int GetValue()
	{
		//Devuelve el valor de la variable interna
		return internalValue;
	}
	
	public: void Add(int addValue)
	{
		//Añade un valor a la variable interna
		internalValue+=addValue;
	}
};

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

