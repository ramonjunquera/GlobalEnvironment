/*
  Autor: Ramón Junquera
  Fecha: 20200423
  Tema: librería Espalexa
  Objetivo: Ejemplo básico de control de brillo

  Descripción:
    Se utiliza un M5Stack Fire.
    Se definirá un dispositivo llamado luz.
    Se controlan los leds de NeoPixel.
    Se pueden apagar, encender o cambiar de brillo.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <WiFi.h>
#include <Espalexa.h>

const char* ssid="xxx";
const char* password="xxx";

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
Espalexa alexa; //Objeto de gestión de Alexa

//Fija color para todos los leds Neo
void setColor(RoJoColor color={0,0,0}) {
  for(byte x=0;x<10;x++) leds.v->drawPixel(x,0,color);
  leds.draw();
}

//Conecta a wifi
void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) {
    delay(100);
  }
}

//Función de gestión de eventos de luz
//Sólo tiene un parámetro con el brillo
void alexaLuz(byte b) {
  setColor({b,b,b});
}

void setup() {
  //Serial.begin(115200); //DEBUG

  //Configuración NeoPixel para M5Stack Fire en pin 15
  //Se simplifica a una sola tira de 10x1
  leds.begin(10,1,15);
  setColor({255,0,0}); //Leds en rojo mientras inicializa
  delay(500);
  setColor({255,255,0}); //Leds en amarillo mientras conecta wifi
  wifiSetup(); //Conectamos a wifi
  setColor({0,255,0}); //Leds en verde. Todo Ok
  delay(500);
  setColor(); //Apagados

  alexa.addDevice("luz",alexaLuz); //Creamos el dispositivo
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  delay(1);
}
