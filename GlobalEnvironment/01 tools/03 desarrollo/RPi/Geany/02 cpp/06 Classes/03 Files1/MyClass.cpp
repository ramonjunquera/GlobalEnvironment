/*
 * Autor: Ramón Junquera
 */

#ifndef MyClass_h
#define MyClass_h

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

#endif

