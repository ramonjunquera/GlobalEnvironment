/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Archivos binarios
 * Material: Raspberry Pi
 * Descripción:
 * El ejecrcicio es idéntico al anterior, excepto que en vez de utilizar
 * un array de números enteros usamos un array de una estructura.
 */


#include <iostream>

using namespace std;

//Definición de estructura de un punto
struct point
{
	//Sólo contiene las coordenadas x e y
	int x,y;
};

int main(int argc, char **argv)
{
	//Variable que contiene el número de elementos del array a escribir
	int itemsCountOut = 3;
	//Creamos un array dinámico del tamaño indicado
	//No le añadimos () al final porque no necesitamos que lo inicialice
	//con ceros. Lo llenaremos nosotros
	struct point *itemsOut = new struct point[itemsCountOut];
	//Llenamos las coordenadas de los puntos del array
	for(int i=0;i<itemsCountOut;i++)
	{
		itemsOut[i].x=i;
		itemsOut[i].y=2*i;
	}
	//Creamos el archivo de salida
	FILE *fileWrite = fopen("struct.bin","wb");
	//Escribimos el número de elementos del array
	fwrite(&itemsCountOut,sizeof(int),1,fileWrite);
	//Escribimos el array
	//Es de estructuras point
	//Indicamos el número de elementos
	fwrite(itemsOut,sizeof(struct point),itemsCountOut,fileWrite);
	//Cerramos el archivo
	fclose(fileWrite);
	//Ya no necesitamos el array. Liberamos su memoria
	delete[] itemsOut;
	
	//Variable que contiene el número de elementos del array leido
	int itemsCountIn;
	//Abrimos el archivo como lectura
	FILE *fileRead = fopen("struct.bin","rb");
	//Si hubo algún problema...
	if(!fileRead)
	{
		cout << "Problema al leer el archivo\n";
		//Salimos con error
		return 1;
	}
	//Leemos el número de elementos del array
	fread(&itemsCountIn,sizeof(int),1,fileRead);
	//Creamos un array dinámico de estructuras point
	struct point *itemsIn = new struct point[itemsCountIn];
	//Leemos el contenido del archivo al array
	fread(itemsIn,sizeof(struct point),itemsCountIn,fileRead);
	//Cerramos el archivo
	fclose(fileRead);
	
	//Mostramos el contenido del array
	for(int i=0;i<itemsCountIn;i++)
	{
		 cout << "(" << itemsIn[i].x << "," << itemsIn[i].y << ") ";
	}
	cout << endl;
	
	//Ya no necesitamos el array. Liberamos su memoria
	delete[] itemsIn;	
	
	//Todo ok
	return 0;
}

