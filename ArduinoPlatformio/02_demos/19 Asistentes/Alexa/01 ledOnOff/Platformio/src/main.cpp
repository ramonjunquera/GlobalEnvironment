/*
  Tema: Librería Espalexa
  Fecha: 20230116
  Autor: Ramón Junquera
  Objetivo: Ejemplo básico encendido/apagado
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Espalexa.h>

const char* ssid="xxx";
const char* password="xxx";

const byte pinLed=LED_BUILTIN;

Espalexa alexa; //Objeto de gestión de Alexa

//Conecta a wifi
void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  //Hace parpadear el led mientras lo intenta
  while (WiFi.status()!=WL_CONNECTED) {
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  } 
}

//Función de gestión de eventos de luz
//Sólo tiene el brillo como parámetro
void alexaLuz(byte b) {
  //b = 0 -> apagado
  //b > 0 -> encendido
  //El estado del led está invertido respecto de su voltaje
  digitalWrite(pinLed,!b);
}

void setup() {
  pinMode(pinLed,OUTPUT);
  wifiSetup(); //Conectamos a wifi
  digitalWrite(pinLed,HIGH); //Led apagado

  //Creamos el dispositivo. Le asignamos un nombre y una función de control
  alexa.addDevice("bombilla",alexaLuz); 
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  delay(1);
}
