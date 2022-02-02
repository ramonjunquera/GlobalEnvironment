/*
  Autor: Ramón Junquera
  Tema: Servomotores
  Fecha: 20180829
  Objetivo: Demostración uso de la clase RoJoServo para RPi
  Material: servomotor SG90 9g, placa RPi

  Descripción:
  Ejemplo simple de control de un servomotor.
  Fijamos los valores límite del servo y utilizamos el método
  setAngle para fijar el ángulo.
  Las constantes de los límites pueden variar dependiendo del servomotor.
  Incluso servomotores del mismo modelo podrían tener parámetros
  distintos. Se debe ajustar para cada caso.
  
  Resultado:
  El servomotor gira distintos ángulos
*/

#include <Arduino.h>
#include "RoJoServo.cpp" //Librería de gestión de servomotores

int main(int argc, char **argv)
{
	//Declaración de variables globales
	RoJoServo servo; //Objeto de gestión del servomotor
	
	//Inicializamos el servo con el pin y los valores máximo y mínino
	servo.begin(18,4,18);
	//Puesto que hemos utilizado los valores por defecto, también
	//podríamos haberlo definido sólo con el pin
	//  servo.begin(18);

	while(1)
	 {
		servo.setAngle(0);
		delay(1500);
		servo.setAngle(45);
		delay(500);
		servo.setAngle(90);
		delay(500);
		servo.setAngle(180);
		delay(1000);
	}
}

