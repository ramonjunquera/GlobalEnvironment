/*
  Autor: Ramón Junquera
  Tema: Teclado de membrana de 4x4
  Objetivo: Demo de librería
  Material adicional: breadboard, cables, teclado de 4x4, RPi
  Descripción:
  Demostración de la librería de gestión del teclado de membrada de 4x4.
  La librería es muy sencilla. Gestiona el efecto rebote.
  Sólo tiene un método que devuelve las nuevas pulsaciones detectadas.
  la propia librería se encarga de la configuración de los pines.

  Muestra en pantalla el carácter correspondiente a la nueva pulsación
  detectada.
     
  Resultado:
  Vemos en pantalla los caracteres de las pulsaciones
*/

#include <Arduino.h>
#include "RoJoKeypad4x4.cpp"

int main(int argc, char **argv)
{
	//Creamos objeto de gestión del keypad 4x4
	//Le pasamos los pines de las filas y las columnas
	RoJoKeypad4x4 pad(18,23,24,25,12,16,20,21);
	cout << "Preparado!" << endl;
    
    //Bucle infinito
    while(1)
    {
		//Creamos una variable para guardar el carácter que se ha pulsado
		//Si es distinto de NULL, lo mostramos en pantalla
		if(char c=pad.get()) cout << c << endl;
	}
}
