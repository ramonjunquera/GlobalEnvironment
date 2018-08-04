/*
 * Autor: Ramón Junquera
 * Tema: Programación en C
 * Objetivo: Aprender a usar el IDE para C
 * Material: Raspberry Pi
 * Descripción:
 * Con el sistema operativo Raspbian se incluye un IDE llamado Geany.
 * A través de él podemos generar código para distintos lenguajes de
 * programación.
 * 
 * Escribiremos un programa que escriba en pantalla "Hola mundo"
 * utilizando el lenguaje C.
 * 
 * Abrimos Geany desde application menu/Programming/Geany
 * Seleccionamos File/New (with Template)/main.c
 * Se abrirá la plantilla de la función principal de una aplicación C.
 * Sólo contiene la definición de la función main con sus parámetros de
 * llamada y la finalización con el comando return.
 * 
 * La función main siempre devuelve un número entero que representa el
 * código de error que se ha producido en la ejecución.
 * 0 significa que no hubo errores. Cualquier otro valor indicará que
 * se ha producido un error y de qué tipo.
 * 
 * La única modificación es la inserción de la línea:
 * printf("Hola mundo\n");
 * 
 * Podemos ver que cuando comenzamos a escribir los comandos aparece una
 * ayuda tanto para la selección como para los parámetros.
 * 
 * Una vez finalizado, debemos pulsar el botón "Build the current file".
 * Esto llamará al compilador.
 * Se solicita una ubicación par aguardar tanto los fuentes como el
 * ejecutable.
 * Podemos crear una estructura bajo la carpeta "Desktop", o guardarlo
 * en una unidad remota si la tenemos montada.
 * 
 * Cuando compila aparece en la sección inferior de IDE y en color azul,
 * las instrucciones de llamada al compilador y sus mensajes.
 * 
 * Si todo es correcto debe finalizar con la frase:
 * "Compilation finished suscessfully"
 * 
 * Podemos ejecutarlo desde el mismo IDE pulsando el botón del avión
 * "Run or view the current file"
 * 
 * Si abrimos el explorador de archivos y localizamos la carpeta,
 * veremos que contiene el fuente y el ejecutable.
 * 
 * Si abrimos una sesión de terminal, podemos navegar hasta la carpeta
 * (cambiando de carpeta con "cd") y allí lanzar el ejecutable con
 * ./ como prefijo. Por ejemplo ./HolaMundo
 */

//Librerías necesarias para los comandos utilizados
#include <stdio.h>

//Función principal
int main(int argc, char **argv)
{
	//Mostramos en pantalla un texto seguido de un retorno de carro
	printf("Hola mundo\n");
	//Hemos finalizado la función  sin errores
	return 0;
}

