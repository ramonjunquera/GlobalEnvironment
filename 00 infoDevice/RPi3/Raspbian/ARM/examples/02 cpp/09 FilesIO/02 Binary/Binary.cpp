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
 * siguiente elemento de la lista. Sólo se puden recorrer en una 
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
 * Para finalizar, llamaos a una función que se encarga de mostrar la
 * lista en pantalla.
 * 
 * La función PrintList tiene como parámetro el puntero de la lista.
 * Lo que hacemos es utilizar un iterator para recorrer todos los
 * elementos de la lista.
 * Un iterator se comporta como un puntero. Apunta a un nodo de la
 * lista. La gran ventaja es que admite su incremento (++).
 * Esto no pasa a la siguiente posición de memoria, sino al siguiente
 * nodo. Esto facilita las cosas a la hroa de moverse por los nodos de
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

using namespace std;

int main(int argc, char **argv)
{
	//Definiremos las variables que escribiremos en el archivo
	//Son de distintos tipos
	int varOutInt = 42;
	double varOutDouble = 321.77;
	float varOutFloat = 234.56;
	
	//Creamos el archivo de salida
	FILE *fileWrite = fopen("numeros.bin","wb");
	//Escribimos cada una de las variables definidas
	//Indicamos el puntero donde guarda el valor
	//El tamaño
	//El número de veces que se debe leer
	//El archivo
	fwrite(&varOutInt,sizeof(int),1,fileWrite);
	fwrite(&varOutDouble,sizeof(double),1,fileWrite);
	fwrite(&varOutFloat,sizeof(float),1,fileWrite);
	//Cerramos el archivo
	fclose(fileWrite);
	
	//Definimos las variables en las que guardaremos los valores leidos
	int varInInt;
	double varInDouble;
	float varInFloat;
	//Abrimos el archivo como lectura
	FILE *fileRead = fopen("numeros.bin","rb");
	//Leemos las variables guardando su valor en memoria
	//Indicando el tamaño
	//Cuántas veces se lee
	//Y de qué archivo
	fread(&varInInt,sizeof(int),1,fileRead);
	fread(&varInDouble,sizeof(double),1,fileRead);
	fread(&varInFloat,sizeof(float),1,fileRead);
	//Cerramos el archivo
	fclose(fileRead);
	
	//Mostramos los valores originales
	cout << varOutInt << " : " << varOutDouble << " : " << varOutFloat << endl;
	//Y los leidos. Deberían ser iguales
	cout << varInInt << " : " << varInDouble << " : " << varInFloat << endl;	
	
	//Todo ok
	return 0;
}

