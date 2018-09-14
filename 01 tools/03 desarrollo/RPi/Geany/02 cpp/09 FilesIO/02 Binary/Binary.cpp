/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Archivos binarios
 * Material: Raspberry Pi
 * Descripción:
 * Ejemplo de uso de archivos binarios.
 * Declaramos varios tipos de variables simples con valor inicial y las
 * guardamos en un archivo binario.
 * Después lo abrimos y recuperamos sus valores.
 */


#include <iostream>
#include <fstream>

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

