/*
  Autor: Ramón Junquera
  Fecha: 20200221
  Tema: Librería para M5Bala
  Objetivo: Demo de librería RoJoM5Bala
  Material: Módulo M5Bala, M5Stack Fire

  Descripción:
	  Comprobamos cuánto tiempo tarda en que la velocidad se vuelva
		constante al aplicar una potencia fija.
		Aplicamos potencias distintas a las dos ruebas.
		Vemos que la de menor potencia tarda aproximadamente un segundo,
		mientras que la de mayor tarde 0.6 segundos.
		Conclusión: para conseguir una velocidad constante es conveniente
		variar la potencia.
*/

#include <Arduino.h>
#include <RoJoM5Bala.h> //Gestión de módulo M5Bala

RoJoM5Bala bala;

void setup() {
	Serial.begin(115200);
	if(!bala.begin()) Serial.println("Error inicializando módulo");
	else Serial.println("Inicialización Ok");
}

void loop() {
	int16_t v0,v1;
	bala.get(&v0,&v1); //Reseteamos los valores acumulados de los encoders
	Serial.println("on");
	bala.set(-80,150); //Velocidad constante
	for(byte i=1;i<10;i++) {
		delay(200);
		bala.get(&v0,&v1); //Leemos encoders
	  Serial.println(String(i)+":"+String(v0)+","+String(v1));
	}
	Serial.println("off");
	bala.set(0,0); //Paramos
	delay(4000);
}
