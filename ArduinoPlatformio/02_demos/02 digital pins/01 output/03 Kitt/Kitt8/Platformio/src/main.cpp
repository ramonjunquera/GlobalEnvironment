/*
  Autor: Ramón Junquera
  Fecha: 20210504
  Tema: Gestión de pines digitales de salida
  Objetivo: Efecto Kitt con 8 leds
*/

#include <Arduino.h>

//Definición de constantes globales
const uint32_t waitTime=80;

//Definición de array de pines
#ifdef __arm__ //Si se trata de una RPi
  const byte leds[8]={18,23,24,25,12,16,20,21};
#elif defined(ARDUINO_AVR_UNO) //Si es un Arduino UNO
  const byte leds[8]={2,3,4,5,6,7,8,9};
#elif defined(ARDUINO_ESP8266_WEMOS_D1MINI) //Si es un ESP8266 Wemos D1 mini
  const byte leds[8]={TX,RX,D1,D2,D3,D4,D8,D7};
#endif

//Definición de variables globales
int8_t currentLed=0; //Led procesado actualmente
int8_t delta=1; //Dirección

void setup() {
  for(int8_t i=7;i>=0;i--) pinMode(leds[i],OUTPUT); //Definimos los pines como salida
}

void loop() {
	digitalWrite(leds[currentLed],HIGH); //Encendemos el led
  delay(waitTime); //Esperamos un momento
	digitalWrite(leds[currentLed],LOW); //Apagamos el led
  currentLed+=delta; //Calculamos el siguiente led
	//Si el led procesado era un extremo...cambiamos de dirección
	if(currentLed==0 || currentLed==7) delta=-delta;
}
