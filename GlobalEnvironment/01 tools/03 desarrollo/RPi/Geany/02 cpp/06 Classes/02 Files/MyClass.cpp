/*
 * Autor: Ramón Junquera
 */

#include "MyClass.h"

using namespace std;

MyClass::MyClass(int initialValue)
{
	//Constructor
	//Permite asignar un valor inicial a la variable interna

	internalValue=initialValue;
}
	
int MyClass::GetValue()
{
	//Devuelve el valor de la variable interna
	return internalValue;
}
	
void MyClass::Add(int addValue)
{
	//Añade un valor a la variable interna
	internalValue+=addValue;
}
