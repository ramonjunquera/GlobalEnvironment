/*
  Autor: Ramón Junquera
  Fecha: 20201210
  Tema: Librería para M5Bala
  Objetivo: Obtención de valores de dead zone
  Material: Módulo M5Bala, M5Stack Fire

  Descripción:
	  Cuando se aplica poca potencia a los motores del módulo bala, no hacen suficiente fuerza
		para que se muevan las ruedas.
		Existe un rango de potencia en el que no se obtiene movimiento. A esto llamaremos
		dead zone.
		No tiene que ser el mismo para ambos motores.
		Si determinamos estos valores y los configuramos en la librería RoJoM5Bala, conseguiremos
		que cualquier potencia de motor aplicada tenga como respuesta un movimiento (excepto el 0).
		Esto facilita mucho el control para conseguir el equilibrio.
		Este ejemplo calcula los valores mínimo y máximo para cada motor.
		Simplemente va aumentanto la potencia de un motor hasta detectar que existe movimiento
		en la rueda, gracias al encoder.
		Las ruedas no deben tocar nada mientras se hace la prueba.
		Los resultados se van mostrando en pantalla.
		Los valores en verde son fijos y los amarillos dinámicos.
		El valor de arriba a la izquierda es el número de ciclos realizados.
		La primera columna muestra el identificador del motor (0 o 1).
		las siguientes columnas muestran los valores de potencia mínimo y máximo que no mueven las
		ruedas.
		El primer valor es el obtenido en el ciclo actual y el valor entre paréntesis es el promedio
		de todos los ciclos realizados.

		Ejemplo:

    40 min          max
    0  -49(-49.85)  31(36.10)
    1  -53(-43.45)  42(40.12)

		Descripción:
    40 = número de ciclo
    -49 = valor mínimo de potencia para motor 0 en ciclo actual
    -49.85 = valor medio de potencia mínima para motor 0 de todos los ciclos
    31 = valor máximo de potencia para motor 0 en ciclo actual
    36.10 = valor medio de potencia mmáxima para motor 0 de todos los ciclos
		-53 = valor mínimo de potencia para motor 1 en ciclo actual
    -43.45 = valor medio de potencia mínima para motor 1 de todos los ciclos
    42 = valor máximo de potencia para motor 1 en ciclo actual
    40.12 = valor medio de potencia mmáxima para motor 1 de todos los ciclos
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoM5Bala.h> //Gestión de módulo M5Bala

//Objetos globales
RoJoILI9341 display; //Objeto de gestión de display
RoJoM5Bala bala; //Gestión de módulo M5Bala

//Variables gloabales
uint16_t epoch=0; //Contador de ciclos
int16_t powerRight0,powerRight1; //Última potencia que no mueve la rueda
float powerMeanMin0,powerMeanMax0,powerMeanMin1,powerMeanMax1; //Promedios

void getPower(int16_t deltaPower) {
	int16_t e0=0,e1=0; //Encoders
	int16_t power=0;
	powerRight0=powerRight1=0; //Reseteamos potencias
	while(e0==0 || e1==0) {
		power+=deltaPower;
		bala.set(power,power);
		delay(100);
		bala.get(&e0,&e1);
		if(e0!=0 || e1!=0) {
			delay(200);
			bala.get(&e0,&e1);
			if(e0==0) powerRight0=power;
			if(e1==0) powerRight1=power;
	  } else powerRight0=powerRight1=power;
	}
	bala.set(0,0); //Motores parados
}

void setup() {
  //Inicializamos el display en un M5Stack Fire
  //  pinRES_display=33;
  //  pinDC_display=27;
  //  pinCS_display=14;
  //  pinBackLight_display=32;
  display.begin(33,27,14,32);
	display.rotation(1); //Botones abajo
	bala.begin(); //Inicialización de módulo M5Bala
	display.printOver("/10x22.fon","min",{0,255,0},25);
	display.printOver("/10x22.fon","max",{0,255,0},140);
	display.printOver("/10x22.fon","0",{0,255,0},0,24);
	display.printOver("/10x22.fon","1",{0,255,0},0,48);
}

void loop() {
	epoch++;
	display.block(0,0,24,21,{0,0,0}); //Borramos valor anterior
	display.printOver("/10x22.fon",String(epoch),{255,255,0},0,0);
	getPower(1); //Obtenemos potencias ascendentes
	if(epoch==1) { //Si es la primera medida...
	  powerMeanMax0=powerRight0;
		powerMeanMax1=powerRight1;
	} else {
		powerMeanMax0=(powerMeanMax0*((float)epoch-1)+(float)powerRight0)/(float)epoch;
		powerMeanMax1=(powerMeanMax1*((float)epoch-1)+(float)powerRight1)/(float)epoch;
	}
	display.block(140,24,254,72,{0,0,0}); //Borramos valores anteriores
	display.printOver("/10x22.fon",String(powerRight0)+" ("+String(powerMeanMax0)+")",{255,255,0},140,24);
	display.printOver("/10x22.fon",String(powerRight1)+" ("+String(powerMeanMax1)+")",{255,255,0},140,48);
	delay(1000); //Esperamos a que los motores se paren

	getPower(-1); //Obtenemos potencias descendentes
	if(epoch==1) { //Si es la primera medida...
	  powerMeanMin0=powerRight0;
		powerMeanMin1=powerRight1;
	} else {
		powerMeanMin0=(powerMeanMin0*((float)epoch-1)+(float)powerRight0)/(float)epoch;
		powerMeanMin1=(powerMeanMin1*((float)epoch-1)+(float)powerRight1)/(float)epoch;
	}
	display.block(25,24,139,72,{0,0,0}); //Borramos valores anteriores
	display.printOver("/10x22.fon",String(powerRight0)+" ("+String(powerMeanMin0)+")",{255,255,0},25,24);
	display.printOver("/10x22.fon",String(powerRight1)+" ("+String(powerMeanMin1)+")",{255,255,0},25,48);
	delay(1000); //Esperamos a que los motores se paren
}
