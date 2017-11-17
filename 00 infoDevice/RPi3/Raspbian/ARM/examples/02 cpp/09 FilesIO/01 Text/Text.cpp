/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Archivos de texto
 * Material: Raspberry Pi
 * Descripción:
 * En C++ tenemos una librería especial para la gestión de archivos de
 * texto: fstream
 * 
 * El tratamiento que se hace para entradas y salidas es muy similar al
 * que utilizamos con la consola estándar.
 * 
 * El ejemplo actual escribirá en un archivo de texto varias líneas que 
 * a continuación serán leidas y mostradas en pantalla.
 * 
 * Tenemos tres objetos distintos para la gestión de archivos de texto:
 * - ifstream : input file stream (archivos de lectura)
 * - ofstream : output file stream (archivos de escritura)
 * - fstream : file stream (archivos de lectura/escritura)
 * 
 * Podríamos utilizar la siguiente secuencia para abrir/abrir un archivo
 *   1 - Crear el objeto stream
 *   2 - Abrirlo definiendo el nombre de archivo y tipo de acceso
 * Por ejemplo:
 *   ofstream fileWrite;
 *   fileWrite.open("numeros.txt",ofstream::out | ofstream::trunc);
 * Pero ya que su constructor nos permite pasarle estos parámetros,
 * haremos la creación del objeto y su configuración en el mismo paso:
 *   ofstream fileWrite("numeros.txt",ofstream::out | ofstream::trunc);
 * 
 * Ya sea en la función open o en el constructor, el segundo parámetro
 * son unos flags que definirán el tipo de acceso:
 * - app : append. Siempre se añade al final. Aunque cambiemos la
 *   posición de escritura con seekp()
 * - ate : at end. Se añade al final. Pero si cambiamos la posición de
 *   escritura con seekp() se respetará.
 * - binary : binary. Se trata el archivo como binario. Si no se indica
 *   se tratará como texto.
 * - in : input. Archivo de lectura
 * - out : output. Archivo de escritura.
 * - trunc : truncate. Si el archivo existe, se sobreescribirá.
 * 
 * Podemos utilizar más de un flag al abrir un archivo, siempre que
 * hagamos un OR (|) entre ellos, como se muestra en el programa.
 * 
 * Cuando estamos leyendo un archivo de texto, debemos saber si se ha
 * activado el flag de EOF (End Of File).
 * Este flag se activa DESPUÉS de intentar leer más allá del final de un
 * archivo.
 * Si abrimos un archivo de tamaño cero, NO se activará el flag
 * automáticamente. Necesitamos hacer al menos una lectura.
 * Esta es la razón por la que primero leemos una línea y después
 * comprobamos si hemos llegado al final.
 * 
 * Cuando se lee una línea de texto, se lee hasta el siguiente retorno
 * de carro. Pero éste no se incluye en la línea leída.
 */

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv)
{
	//Creamos el objeto que gestiona el archivo de salida
	//Indicamos el nombre del archivo
	//Indicamos que es de escritura
	//Indicamos que se sobreescribirá
	ofstream fileWrite("numeros.txt",ofstream::out | ofstream::trunc);
	
	//Recorremos los números desde el 112 al 123...
	for(int n=112;n<=123;n++)
	{
		//Escribimos un prefijo
		//Escribimos el número
		//Escribimos un sufijo
		//Le añadimos un retorno de carro
		//Lo escribimos en el archivo
		fileWrite << "prefix-" << n << "-suffix\n";
	}
	//Cerramos el archivo
	fileWrite.close();
	
	//Variable que contará el número de líneas leidas
	//Comenzamos por la 1
	int lineNumber=1; 
	//Creamos el objeto que gestiona el archivo de entrada
	//Al mismo tiempo indicamos el nombre de archivo
	//Y que será de lectura
	ifstream fileRead("numeros.txt",ifstream::in);
	//Si no hemos podido abrir el archivo...
	if(!fileRead)
	{
		cout << "Error al abrir el archivo\n";
		return 1;
	}
	//Hemos podido abrir el archivo
	
	//Declaramos la variable en la que guardaremos la línea leida
	string line;
	
	//Hacemos la primera lectura
	fileRead >> line;
	//Mientras no se detecte el final del archivo...
	while(!fileRead.eof())
	{
		//Mostramos el número de línea actual
		//Lo aumentamos para la próxima vez
		//Mostramos la línea leida
		cout << lineNumber++ << ":" + line << endl;
		//Leemos la siguiente línea
		fileRead >> line;
	}
	//Hemos terminado de leer el archivo
	
	//Cerramos el archivo
	fileWrite.close();
	
	//Finalmente eliminamos el archivo creado
	if(remove("numeros.txt")) cout << "error al eliminar el archivo" << endl;
	else cout << "archivo eliminado correctamente" << endl;
	
	//Todo ok
	return 0;
}

