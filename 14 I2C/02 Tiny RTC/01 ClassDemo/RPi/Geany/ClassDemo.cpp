/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Demo de librería
  Material: breadboard, placa RPi, cables, módulo "Tiny RTC I2C"

  Descripción:
  Mostraremos las capacidades de la librería de gestión del RTC.
  Inicialmente comprobamos si el reloj está presente.
  Lo ponemos en hora en dada una hora fija.
  Mostramos en pantalla la hora completa cada segundo.
  
  Resultado:
  Vemos la hora del reloj en pantalla
*/

#include <Arduino.h>
#include "RoJoRTCclock.cpp" //Gestión del reloj del módulo Tiny RTC

int main(int argc, char **argv)
{
	//Creamos objeto de gestión del reloj
	RoJoRTCclock clock;
	
	if(!clock.check())
	{
		cout << "Reloj no detectado" << endl;
		return 1;
	}
	cout << "Reloj detectado" << endl;
	
	//Creamos una estructura de fecha
	datetime currentTime;
	//Completamos los detalles de la hora actual
	currentTime.second=50;
	currentTime.minute=59;
	currentTime.hour=23;
	currentTime.weekDay=7;
	currentTime.day=31;
	currentTime.month=12;
	currentTime.year=2017;
	//Ponemos la hora definida en el reloj
	clock.set(currentTime,0);
	
	while(1)
	{
		//Obtenemos la hora actual
		currentTime=clock.get();
		//Si hemos podido leer la hora del módulo conectado a I2C...
		if (currentTime.second < 128)
		{
			//Ya tenemos el tiempo actual guardado en la variable
			//currentTime. Lo mostramos
			cout << (int)currentTime.day << "/";
			cout << (int)currentTime.month << "/";
			cout << currentTime.year << " ";
			cout << (int)currentTime.hour << ":";
			cout << (int)currentTime.minute << ":";
			cout << (int)currentTime.second;
			cout << " weekDay=" << (int)currentTime.weekDay << endl;
		}
		else //No hemos podido leer la hora!
		{
			//Mostramos 4 guiones en la pantalla para indicar que algo no funciona
			cout << "----" << endl;
		}
		//Esperamos un segundo
		delay(1000);
	}
}

