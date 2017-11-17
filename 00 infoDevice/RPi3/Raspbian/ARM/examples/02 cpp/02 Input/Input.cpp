/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Solicitar datos
 * Material: Raspberry Pi
 * Descripción:
 * A veces el programa debe solicitar datos al usuario.
 * Haremos un programa que pida el nombre y edad al usuario y después
 * los muestre.
 * 
 * El comando para hacer input es std::cin.
 * Al igual que cout, también pertenece a la librería iostream.
 * 
 * Añadiremos un using namespace std para evitar escribir std como
 * prefijo de los comandos.
 * 
 * cin recogerá de la línea de comandos el siguiente grupo de caracteres
 * hasta encontrar un espacio o un retorno de carro, y lo guardará en
 * la variable indicada.
 * 
 * Intentará convertir el texto introducido al tipo de la variable
 * indicada para guardarlo.
 * Si la variable es de tipo int pero introducimos un texto sin valor
 * numérico, lo interpretará como valor cero.
 * 
 * Si introducimos más de una palabra (texto separado por espacios) en 
 * una línea, sólo se tendrá en cuenta la primera palabra. Pero las
 * siguientes quedarán en buffer de entrada y serán procesadas por las
 * futuras llamadas a cin.
 * 
 * En este ejemplo se introduce por primera vez el uso del tipo string.
 * Para la concatenación de string se utiliza el signo +
 * 
 * Tras pedir los datos al usurio, finalmente se muestran en pantalla.
 * 
 * Si utilizamos la nomenclatura << no tendremos que convertir todas las
 * variables a string. La conversión se hará automáticamente.
 * Sólo en el primero de los casos podríamos utilizar + porque
 * concatenamos dos strings.
 */


#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	//Definición de variables de método
	string nombre;
	int edad;

	cout << "Cómo te llamas? ";
	cin >> nombre;	
	cout << "Cuántos años tienes? ";
	cin >> edad;
	cout << nombre + " tiene " << edad << " años\n";
	
	//Todo ok
	return 0;
}

