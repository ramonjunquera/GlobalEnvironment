/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Lista simple
 * Material: Raspberry Pi
 * Descripción:
 * Las listas son una de las estructuras más utilizadas en programación.
 * 
 * En el ejemplo actual aprenderemos a utilizar una lista simple.
 * Son aquellas que un elemento guarda un valor y un puntero al
 * siguiente elemento de la lista. Sólo se pueden recorrer en una 
 * dirección: de principio a fin.
 * 
 * Ya existe una clase librería llamada forward_list que permite
 * automatizar su uso.
 * El problema es que esta librería pertenece al estándar C++11 que no
 * siempre está disponible en el compilador.
 * Por suerte en la versión actual de Raspbian el compilador (gcc) es
 * capaz de soportarla aunque sea en modo beta.
 * Para ello tendremos que hacer alguna modificación en los parámetros
 * del compilador.
 * Seleccionamos Build/Set Build Commands y en la sección "C++ commands"
 * Encontraremos los siguientes parámetros:
 *   Compile: g++ -Wall -c "%f"
 *   Build: g++ -Wall -o "%e" "%f"
 * Que los sustituiremos por:
 *   Compile: g++ -Wall -std=c++11 -c "%f"
 *   Build: g++ -Wall -std=c++11 -o "%e" "%f"
 * 
 * Comenzamos incluyendo la librería forward_list
 * 
 * En la función main creamos una lista sencilla de números enteros.
 * Realmente podemos poner cualquier tipo de variable. Clases incluidas.
 * 
 * A continuación añadimos varios elementos a la lista.
 * A este tipo de listas sólo se le pueden añadir elementos por el
 * principio.
 * Realmente se crea un nuevo nodo y se le hace apuntar al nodo que
 * antes era el primero.
 * 
 * Para finalizar, llamamos a una función que se encarga de mostrar la
 * lista en pantalla.
 * 
 * La función PrintList tiene como parámetro el puntero de la lista.
 * Lo que hacemos es utilizar un iterator para recorrer todos los
 * elementos de la lista.
 * Un iterator se comporta como un puntero. Apunta a un nodo de la
 * lista. La gran ventaja es que admite su incremento (++).
 * Esto no pasa a la siguiente posición de memoria, sino al siguiente
 * nodo. Esto facilita las cosas a la hora de moverse por los nodos de
 * la lista.
 * Comenzamos por el nodo inicial (list.begin()).
 * Recorremos los nodos hasta llegar al final (list.end()).
 * El nodo final no es el último, sino que es el puntero del último de
 * los nodos (que habitualmente es NULL).
 * En cada ciclo iremos aumentando el iterator.
 * Otra de las ventajas es que el contenido del iterator es el valor del
 * nodo. Así no tenemos que buscarlo.
 * 
 * En esta primera función se declara completamente el iterator.
 * 
 * Existe una segunda función para imprimir la lista llamada PrintList2.
 * La única diferencia es que aquí no se declara el iterator.
 * Nos aprovechamos de que una variable puede ser declarada como auto y
 * su tipo será definido cuando se le asigne valor.
 * Esta sintaxis es más cómoda, rápida y evita errores de declaración.
 */


#include <iostream>
#include <forward_list>

using namespace std;

void PrintList(forward_list<int> list)
{
	//Muestra la lista en pantalla
	
	//Recorre todos los elementos de la lista, comenzando por el primero
	//de uno en uno, mientras no se llegue al último
	for(forward_list<int>::iterator itr=list.begin();itr!=list.end();itr++)
	{
		//Muestra el valor del elemento de la lista seguido de un espacio
		cout << *itr << " ";
	}
	//Hemos terminado de escribir todos los elementos de la lista
	//Escribimos un retorno de carro
	cout << endl;
}

void PrintList2(forward_list<int> list)
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
	forward_list<int> list;
	
	//Añadimos varios valores a la lista por el inicio
	list.push_front(3);
	list.push_front(7);
	list.push_front(8);
	//Mostramos el contenido de la lista
	PrintList(list);
	//Mostramos el contenido de la lista
	PrintList2(list);
	
	//Todo ok
	return 0;
}

