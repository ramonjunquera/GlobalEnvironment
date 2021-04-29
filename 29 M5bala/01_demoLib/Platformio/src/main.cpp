/*
  Autor: Ramón Junquera
  Fecha: 20200221
  Tema: Librería para M5Bala
  Objetivo: Demo de librería RoJoM5Bala
  Material: Módulo M5Bala, M5Stack Fire

  Descripción:
	  Variamos la velocidad de las ruedas en ambos sentidos,
		progresivamente.
		La rueda izquierda a la mitad de velocidad de la derecha.
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
	for(int16_t i=0;i<100;i++) {
		bala.set(i,i*2);
		Serial.println(String(i)+","+String(i*2));
		delay(50);
	}
	for(int16_t i=100;i>=0;i--) {
		bala.set(i,i*2);
		Serial.println(String(i)+","+String(i*2));
		delay(50);
	}
	delay(3000);
	for(int16_t i=0;i<100;i++) {
		bala.set(-i,-i*2);
		Serial.println(String(i)+","+String(i*2));
		delay(50);
	}
	for(int16_t i=100;i>=0;i--) {
		bala.set(-i,-i*2);
		Serial.println(String(i)+","+String(i*2));
		delay(50);
	}
	delay(3000);
}
