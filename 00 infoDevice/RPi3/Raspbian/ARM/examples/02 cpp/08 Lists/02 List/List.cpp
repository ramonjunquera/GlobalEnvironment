/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Lista doblemente enlazada
 * Material: Raspberry Pi
 * Descripción:
 * En el ejercicio anterior aprendíamos a utilizar listas simples.
 * En este aprenderemos a utilizar listas doblemente enlazadas.
 * 
 * Cada nodo de la lista contiene un valor y dos punteros. Uno al nodo
 * siguiente y otro al nodo anterior.
 * 
 * Su gestión es idéntica a las listas simples.
 * La ventaja es que se pueden añadir nodos tanto por delante como por
 * detrás.
 * 
 * Se muestra un ejemplo de cómo eliminar los nodos que contienen un
 * valor determinado.
 * También se muestra cómo vaciar una lista (borrar todos sus nodos).
 */

#include <iostream>
#include <list>

using namespace std;

void PrintList(list<int> list)
{
	//Muestra la lista en pantalla
	
	//Recorre todos los elementos de la lista, comenzando por el primero
	//de uno en uno, mientras no se llegue al último
	for(auto itr=list.begin();itr!=list.end();itr++)
	{
		//Muestra el valor del elemento de la lista seguido de un espacio
		cout << *itr << " ";
	}
	//Hemos terminado de escribir todos los elementos de la lista
	//Escribimos un retorno de carro
	cout << endl;
}

int main(int argc, char **argv)
{
	//Creamos una lista sencilla de números enteros
	list<int> list;
	
	//Añadimos varios valores a la lista por el inicio
	list.push_front(3);
	list.push_front(7);
	list.push_front(8);
	//Y alguno por el final
	list.push_back(2);
	//Mostramos el contenido de la lista
	PrintList(list);
	//Eliminamos el nodo con el valor 7
	list.remove(7);
	//Mostramos el contenido de la lista
	PrintList(list);
	//Vaciamos la lista
	list.clear();

	//Todo ok
	return 0;
}

