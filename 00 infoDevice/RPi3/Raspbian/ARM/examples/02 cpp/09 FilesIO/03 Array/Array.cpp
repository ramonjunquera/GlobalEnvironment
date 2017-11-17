/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Archivos binarios
 * Material: Raspberry Pi
 * Descripción:
 * Para gestionar archivos binarios tenemos suficiente con las funciones
 * proporcionadas por la librería iostream.
 * 
 * En este ejercicio crearemos un array dinámico de números enteros con
 * tantos elementos como se nos indique en la variable itemsCountOut.
 * Lo llenaremos con números consecutivos comenzando con el 0.
 * Crearemos un archivo binario.
 * Guardaremos el número de elementos del array.
 * Guardaremos a continuación el array completo y cerraremos el archivo.
 * 
 * Abriremos al archivo, leeremos el número de elementos del array.
 * Crearemos un nuevo array dinámico. Leeremos su contenido del archivo.
 * Finalmente mostraremos su contenido en pantalla.
 */


#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	//Variable que contiene el número de elementos del array a escribir
	int itemsCountOut = 14;
	//Creamos un array dinámico del tamaño indicado
	//No le añadimos () al final porque no necesitamos que lo inicialice
	//con ceros. Lo llenaremos nosotros
	int *itemsOut = new int[itemsCountOut];
	//Llenamos el array con números consecutivos desde el 0
	for(int i=0;i<itemsCountOut;i++) itemsOut[i]=i;
	//Creamos el archivo de salida
	FILE *fileWrite = fopen("array.bin","wb");
	//Escribimos el número de elementos del array
	fwrite(&itemsCountOut,sizeof(int),1,fileWrite);
	//Escribimos el array
	//Es de numeros enteros
	//Indicamos el número de elementos
	fwrite(itemsOut,sizeof(int),itemsCountOut,fileWrite);
	//Cerramos el archivo
	fclose(fileWrite);
	//Ya no necesitamos el array. Liberamos su memoria
	delete[] itemsOut;
	
	//Variable que contiene el número de elementos del array leido
	int itemsCountIn;
	//Abrimos el archivo como lectura
	FILE *fileRead = fopen("array.bin","rb");
	//Si hubo alg'ún problema...
	if(!fileRead)
	{
		cout << "Problema al leer el archivo\n";
		//Salimos con error
		return 1;
	}
	//Leemos el número de elementos del array
	fread(&itemsCountIn,sizeof(int),1,fileRead);
	//Creamos un array dinámico
	int *itemsIn = new int[itemsCountIn];
	//Leemos el contenido del archivo al array
	fread(itemsIn,sizeof(int),itemsCountIn,fileRead);
	//Cerramos el archivo
	fclose(fileRead);
	
	//Mostramos el contenido del array
	for(int i=0;i<itemsCountIn;i++) cout << itemsIn[i] << " ";
	cout << endl;
	
	//Ya no necesitamos el array. Liberamos su memoria
	delete[] itemsIn;	
	
	//Todo ok
	return 0;
}

