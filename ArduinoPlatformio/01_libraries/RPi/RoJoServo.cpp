/*
 * Autor: Ramón Junquera
 * Placa: Raspberry Pi 3
 * Descripción: Gestión de servomotores
 * Versión: 20180830
 * 
 * Métodos:
 *   void begin(byte pin,byte minValue,byte maxValue)
 *   void setAngle(byte angle)
 * 
 * Nota:
 *   Teniendo en cuenta el poco rango de funcionamiento que tenemos
 *   (entre 4 y 18), el ángulo de giro es muy poco preciso.
 *   No se debería utilizar para gestión precisa de servomotores.
 */

#ifndef RoJoServo_cpp
#define RoJoServo_cpp

#include <Arduino.h>

class RoJoServo
{
	//Utilizar las funciones de gestión PWM incluidas en la librería
	//Arduino.h

	//Declaración de variables privadas
	private:
	byte _pin=255;
	byte _minValue=17;
	byte _maxValue=255;
	
	//Declaración de métodos públicos:
	public:
	
	void begin(byte pin,byte minValue=4,byte maxValue=18)
	{
		//Inicialización de la clase
		//Se indica el pin de control, el valor mínimo (0 grados) y el
		//valor máximo (180 grados)
		
		//Anotamos los parámetros en la variables internas
		_pin=pin;
		_minValue=minValue;
		_maxValue=maxValue;
		//Fijamos la frecuencia de PWM a 60Hz
		//Es la frecuencia a la que trabajan los servomotores
		//Atención!. Esta frecuencia afecta a todos los pines con PWM
		freqPWM(60);
	}
	
	void setAngle(byte angle)
	{
		//Fija el ángulo
		analogWrite(_pin,(float)(_maxValue-_minValue)*(float)angle/180.0+(float)_minValue);
	}
	

};

#endif

