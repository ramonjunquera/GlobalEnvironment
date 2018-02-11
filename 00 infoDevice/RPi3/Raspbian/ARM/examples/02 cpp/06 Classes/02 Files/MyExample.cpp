/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Compilar proyecto con clases externas
 * Material: Raspberry Pi
 * Descripción:
 * La ventaja de crear clases es que pueden ser reutilizadas en
 * distintos desarrollos.
 * Para facilitar esta tarea es habitual que las clases sean definidas
 * en archivos independientes.
 * Estos archivo se añaden al proyecto y son llamados desde el programa
 * principal.
 * 
 * La manera tradicional de guardar una clase es en dos archivos
 * independientes: un .h y un .cpp
 * El .h (header) sólo contendrá la declaración de la clase (variables y
 * métodos).
 * El .cpp contendrá la definición de los métodos.
 * 
 * El ejemplo actual tiene la misma funcionalidad que el anterior.
 * La única diferencia es que la clase se ha definido en archivos
 * externos.
 * 
 * Comenzamos con el programa principal.
 * Tras incluir las librerías necesarias para ejecutar el programa,
 * añadiremos la referencia al archivo de cabecera (.h) de nuestra clase
 * externa.
 * Esto es suficiente como para poder utilizarla como si estuviese
 * definida dentro del programa principal.
 * 
 * El archivo de cabecera, sólo contiene la declaración de la clase.
 * Siempre comienza con una instrucción destinada al compilador.
 * Siempre deberíamos utilizar esta nomenclatura.
 * Lo que hace es comprobar si se ha definido una variable de
 * compilación con un nombre determinado. Si no se ha definido, lo
 * primero que hacemos es definirla.
 * A continuación hacemos la declaración de la clase.
 * Para finalizar cerramos el if de compilación abierto al principio.
 * 
 * Gracias a estas líneas, si se da el caso que utilizamos la misma
 * clase (mismo archivo de cabecera) en varios puntos del desarrollo,
 * sólo se cargará una vez. Evitando el error de duplicidad de clases
 * con el mismo nombre.
 * 
 * El nombre de la variable de compilación que se utiliza lo elegimos
 * nosotros mismos. Aunque por convención es el nombre de la clase
 * seguido del prefijo _h
 * 
 * Con lo que tenemos ahora el programa no terminaría de funcionar
 * porque sólo hemos declarado la clase, pero no tenemos hemos definido
 * sus métodos en ningún sitio.
 * 
 * El archivo .cpp de la clase es el que contiene estas definiciones.
 * Debe incluir las librerías necesarias para su correcto
 * funcionamiento, además del archivo de cabecera con la declaración de
 * clase.
 * 
 * Los métodos se definirán indicando siempre a qué clase pertenecen.
 * Por ejemplo:
 *   void MyClass::Add(int addValue)
 * No tenemos que preocuparnos ahora si son públicos o privados. Eso ya
 * está indicado en el arhivo de cabecera.
 * 
 * Sólo nos queda que cuando compilemos y construyamos el programa
 * principal, también se tengan en cuenta el resto de archivo .cpp que
 * haya en la misma carpeta.
 * 
 * Esto lo conseguimos modificando los parámetros de llamada al
 * compilador.
 * Seleccionamos Build/Set Build Commands. En la sección "C++ Commands".
 * Para Build los parámetros por defecto son:
 *   g++ -Wall -o "%e" "%f"
 * El último parámetro (%f) se sustituye por el nombre del archivo a
 * compilar. Pero como nosotros queremos que se tengan en cuenta todos
 * los archivos .cpp, sustituiremos los parámetros por esto:
 *   g++ -Wall -o "%e" *.cpp
 * Así nos aseguramos contruiremos el programa con todos los .cpp de la 
 * carpeta.
 * 
 * La única pega de este sistema es que el programa final se construiría
 * abriendo cualquiera de los archivo .cpp de la carpeta.
 * Y al ejecutable final se le daría el nombre del archivo .cpp abierto.
 * Por eso hay que tener un poco de cuidado y abrir siempre .cpp que 
 * contiene el programa principal para que el ejecutable mantenga el
 * nombre.
 */

#include <iostream>
#include "MyClass.h"

using namespace std;

int main(int argc, char **argv)
{
	//Creamos el objeto que contiene la clase
	//Y le asignamos un valor inicial a la variable interna
	MyClass c = MyClass(3);
	//Mostramos el valor de la variable interna
	cout << c.GetValue() << endl;
	//Añadimos un valor a la variable interna
	c.Add(2);
	//Mostramos el valor de la variable interna
	cout << c.GetValue() << endl;
	
	//Todo ok
	return 0;
}

