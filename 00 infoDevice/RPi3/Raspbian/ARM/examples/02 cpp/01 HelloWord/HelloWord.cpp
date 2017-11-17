/*
 * Autor: Ramón Junquera
 * Tema: Programación en C++
 * Objetivo: Hacer un programa que escriba en pantalla
 * Material: Raspberry Pi
 * Descripción:
 * Con Geany también  se puede desarrollar en C++.
 * Este lenguaje tiene alguna ventajas sobre C en cuanto a clases.
 * 
 * En este primer ejemplo mostraremos varias líneas de texto en pantalla
 * de distintas maneras y explicando su estructura.
 * 
 * El programa comienza con las librerías utilizadas.
 * Por defecto incluiremos iostream para gestionar el envío de
 * información a la consola.
 * La diferencia con C es que las librerías no necesitan el .h al final.
 * 
 * 
 * Dentro de una librería pueden existir distintas clases que
 * jerarquizan los comandos (métodos).
 * Podríamos escribir siempre el path entero del comando y se admitiría.
 * Pero para facilitar la escritura, podemos añadir un "using".
 * Esto permite que el comando se busque en los "usings" declarados y
 * hacerlos más cortos, porque no se necesitará el path entero.
 * 
 * Por ejemplo. La librería iostream tiene un comando llamado cout, que
 * se en cuentra en el path std::cout.
 * Si incluimos la línea:
 *   using namespace std;
 * No tendremos la necesidad de escribir std::cout siempre. Con cout
 * será suficiente.
 * 
 * Lo mismo ocurre con el comando endl. tiene el mismo path.
 * 
 * Añadir un using no impide que podamos seguir escribiendo el path
 * completo.
 * 
 * Puesto que nosotros utilizaremos los comandos cout y endl en nuestro
 * programa añadimos el using.
 * 
 * Como en C, siempre necesitamos la función main. Es por la que
 * comienza siempre el programa.
 * 
 * La función main siempre debe devolver un int, como en C, que
 * representa el código de error.
 * El código 0 significa que no hubo errores.
 * 
 * Aunque por defecto aparezca definida así:
 *   int main(int argc, char **argv)
 * No es necesaria la definici'ón de parámetros.
 * Podríamos escribirla así:
 *   int main()
 * 
 * El programa escribe 4 líneas en pantalla.
 * Todas tienen un caracter de retorno de carro al final.
 * 
 * El comando cout << envía el texto que le sigue a la consola de salida
 * El comando endl representa el retorno de carro, que es equivalente 
 * a \n.
 * Se muestra cómo se puede utilizar el path completo o aprovechar el
 * using para escribir sólo el comando.
 * 
 * Finalizamos con un return 0 para indicar que hemos terminado sin
 * errores.
 * 
 * No es obligatorio que una función tenga siempre un return. Podríamos 
 * omitirlo.
 */


#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	std::cout << "Hello World 1" << std::endl;
	std::cout << "Hello World 2\n";
	cout << "Hello World 3" << endl;
	cout << "Hello World 4\n";
	
	//Todo correcto
	return 0;
}

