/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Sincronización NTP
  Material: breadboard, placa RPi, cables, módulo "Tiny RTC I2C"

  Descripción:
  Raspberry Pi al tener un sistema operativo propio, por defecto
  sincroniza la hora de su reloj interno con Internet.
  Podemos encontrar la configuración de la sincronización NTP en el
  archivo /etc/ntp.conf
  
  En el ejemplo mostraremos cómo obtenemos la hora del reloj interno
  y sincronizamos el RTC con ella.
  La única particularidad es que en Unix la hora se cuenta en segundos
  desde el primer día de 1970, no como los servidores NTP que lo cuentan
  desde 1900.
  
  Resultado:
  Sincroniza el RTC con el reloj interno.
 */

#include <Arduino.h>
#include "RoJoRTCclock.cpp" //Gestión del reloj del módulo Tiny RTC
#include <time.h> //Para time_t

int main(int argc, char **argv)
{
	//Zona horaria = GMT+1
	const int8_t timeZone = 1;
	//Horario de verano?
	const bool summerTime=true; //Sí
	
	//Creamos objeto de gestión del reloj
	RoJoRTCclock clock;
	
	if(!clock.check())
	{
		cout << "Reloj no detectado" << endl;
		return 1;
	}
	cout << "Reloj detectado" << endl;
	
	//Obtenemos la hora del reloj interno que ya está sincronizado 
	//con Internet. Nos devuelve el número de segundos transcurridos
	//desde 1970 hasta el momento actual.
	time_t realTime=time(nullptr);
	//Ajustamos el horario local
	realTime+=3600*(timeZone+summerTime);
	//Ponemos el reloj en hora
	clock.set1970(realTime);
	
	while(1)
	{
		//Obtenemos la hora actual
		datetime currentTime=clock.get();
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
