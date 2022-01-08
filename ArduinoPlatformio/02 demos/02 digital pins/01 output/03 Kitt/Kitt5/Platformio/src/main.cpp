/*
  Autor: Ramón Junquera
  Fecha: 20210504
  Tema: Gestión de pines digitales de salida
  Objetivo: Efecto Kitt con 5 leds
*/

#include <Arduino.h>

//Definición de constantes globales
const uint32_t waitTime=200;

//Definición de array de pines par ATtiny85
const byte leds[6]={0,1,2,3,4};

//Definición de variables globales
int8_t currentLed=0; //Led procesado actualmente
int8_t delta=1; //Dirección

void setup() {
  for(int8_t i=4;i>=0;i--) pinMode(leds[i],OUTPUT); //Definimos los pines como salida
}

void loop() {
	digitalWrite(leds[currentLed],HIGH); //Encendemos el led
  delay(waitTime); //Esperamos un momento
	digitalWrite(leds[currentLed],LOW); //Apagamos el led
  currentLed+=delta; //Calculamos el siguiente led
	//Si el led procesado era un extremo...cambiamos de dirección
	if(currentLed==0 || currentLed==4) delta=-delta;
}
