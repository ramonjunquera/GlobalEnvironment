/*
 * Autor: Ramón Junquera
 * Fecha: 20190602
 * Descripción:
 *   Compatibilidad con las funciones setup() y loop() de Arduino.
 *   El programa de Arduino se debe llamar main.cpp y debe estar en
 *   la misma carpeta que el actual.
 *   La Aplicación se lanza desde aquí.
 */

#include "main.cpp"

int main(int argc, char **argv)
{
	setup();
	while(true)
	{
		loop();
	}
}
