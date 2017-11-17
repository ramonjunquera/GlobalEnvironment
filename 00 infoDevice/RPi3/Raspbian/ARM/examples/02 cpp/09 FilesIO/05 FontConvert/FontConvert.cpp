/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Archivos binarios
 * Material: Raspberry Pi
 * Descripción:
 * Este ejemplo servirá para convertir un archivo de definición de 
 * fuentes (.h) en un archivo físico con la misma información, que será
 * mucho más manejable en entornos como Raspberry Pi, por tener una 
 * estructura de archivos.
 * 
 * Se deben modificar las primeras líneas marcadas como editables para
 * cada fuente.
 * 
 * El archivo .h debe ser ligeramente modificado:
 *   1. Eliminar la palabra PROGMEN
 *   2. Sustituir " B" por " 0b" case sensitive
 *   3. Sustituir ",B" por ",0b" case sensitive
 * 
 * La secuencia de guardado/lectura en el archivo es la siguiente:
 *   charMin : byte
 *   charMax : byte
 *   pages : byte
 *   charIndex[charMax-charMin+1] : unsigned short (size 2)
 *   width[charMax-charMin+1] : byte
 *   bitmapCount : unsigned short (size 2)
 *   bitmap[bitmapCount] : byte
 */

#include <iostream>
using namespace std;
#include "RoJoABCtypedef.h" //Tipos necesarios para interpretar el .h

//Líneas a personalizar para cada archivo .h
#include "RoJoABCFreeMono24pt7b.h"
const abcFont my_abc_font = RoJoABCFreeMono24pt7b;
const abcProperties *my_abc_props = RoJoABCFreeMono24pt7bProps;
const byte *my_abc_bitmap = RoJoABCFreeMono24pt7bBitmap;
const unsigned short bitmapCount = sizeof(RoJoABCFreeMono24pt7bBitmap);
const char fileName[] = "RoJoABCFreeMono24pt7b.fon";
//Fin de líneas a personalizar

int main(int argc, char **argv)
{
	//Comenzamos creando el archivo de salida
	FILE *file = fopen(fileName,"wb");
	
	//Escribimos los valores globales de la fuente contenidos en 
	//my_abc_font
	fwrite(&my_abc_font.charMin,1,1,file);
	fwrite(&my_abc_font.charMax,1,1,file);
	fwrite(&my_abc_font.pages,1,1,file);
	
	//Calculamos el número de caracteres que tiene la fuente
	byte charCount=my_abc_font.charMax-my_abc_font.charMin+1;

	//Creamos un array de bytes para mantener las posiciones de inicio
	//de cada carácter en el array gráfico
	//Aunque en las propiedades guardemos este detalle, se desglosa y se
	//guarda por separado para optimizar tamaño de archivo
	unsigned short *charIndex = new unsigned short[charCount];
	//Lo llenamos con sus valores
	for(byte c=0;c<charCount;c++) charIndex[c]=my_abc_props[c].charIndex;
	//Escribimos el array en el archivo
	fwrite(charIndex,2,charCount,file);
	//Ya no necesitamos el array dinámico
	delete[] charIndex;
	
	//Hacemos algo parecido con la anchura de los caracteres
	byte *width = new byte[charCount];
	//Lo llenamos con sus valores
	for(byte c=0;c<charCount;c++) width[c]=my_abc_props[c].width;
	//Escribimos el array en el archivo
	fwrite(width,1,charCount,file);
	//Ya no necesitamos el array dinámico
	delete[] width;

	//Anotamos el número de elementos del array de bitmap
	fwrite(&bitmapCount,2,1,file);
	//Guardamos el array de bitmap completo
	fwrite(my_abc_bitmap,1,bitmapCount,file);
	
	//Cerramos el archivo
	fclose(file);
	
	//Todo ok
	return 0;
}

