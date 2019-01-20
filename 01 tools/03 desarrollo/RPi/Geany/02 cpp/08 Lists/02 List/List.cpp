/*
 * Autor: Ramón Junquera
 * Fecha: 20180830
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
 * valor determinado o en una posición determinada.
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
	list<int> myList;
	
	//Añadimos varios valores a la lista por el inicio
	myList.push_front(3);
	myList.push_front(7);
	myList.push_front(8);
	//Y alguno por el final
	myList.push_back(2);
	//Mostramos el contenido de la lista
	PrintList(myList);
	//Eliminamos el nodo con el valor 7
	cout << "Eliminando nodo con valor 7" << endl;
	myList.remove(7);
	//Mostramos el contenido de la lista
	PrintList(myList);
	//Obtenemos el iterator del primer nodo
	list<int>::iterator itr=myList.begin();
	cout << "Valor del primer nodo = " << *itr << endl;
	cout << "Borrando el primer nodo" << endl;
	//Lo borramos
	myList.erase(itr);
	//Mostramos el contenido de la lista
	PrintList(myList);
	//Vaciamos la lista
	myList.clear();

	//Todo ok
	return 0;
}

