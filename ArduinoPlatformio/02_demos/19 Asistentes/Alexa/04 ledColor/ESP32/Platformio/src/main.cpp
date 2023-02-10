/*
  Tema: librería Espalexa
  Fecha: 20230108
  Autor: Ramón Junquera
  Objetivo: Ejemplo de control de color con ESP32

  Descripción:
    Se utiliza un M5Stack Fire.
    Se definirá un dispositivo llamado bombilla.
    Se controlan los leds de NeoPixel.
    Se pueden apagar, encender o cambiar de color

  Colores:
    Los colores están tipificados. No se puede elegir un color aleatoriamente.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <WiFi.h>
#include <Espalexa.h>

const char* ssid="IronMan";
const char* password="1234yasitolrato";

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
Espalexa alexa; //Objeto de gestión de Alexa

//Fija color para todos los leds NeoPixel. Por defecto negro
void setColor(uint32_t color=0) {
  for(byte i=0;i<10;i++) leds.videoMem[i].setColor(color);
  leds.show();
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
//En esta ocasión se le pasa el parámetro de brillo/intensidad
//Y en un uint32_t los canales RGB como XRGB
void alexaLuz(byte i,uint32_t rgb) {
  //Extraemos los canales de color del parámetro rgb y calculamos
  //los brillos por canal
  byte r=(((rgb>>16)&0xFF)*i)/255;
  byte g=(((rgb>>8)&0xFF)*i)/255;
  byte b=((rgb&0xFF)*i)/255;
  setColor((r<<16)+(g<<8)+b); //Recomponemos color con canales
}

void setup() {
  //Serial.begin(115200); //DEBUG

  //Configuración NeoPixel para M5Stack Fire en pin 15
  //Serán 10 leds
  leds.begin(10,15);
  setColor(0x330000); //Leds en rojo mientras inicializa
  delay(500);
  setColor(0x333300); //Leds en amarillo mientras conecta wifi
  wifiSetup(); //Conectamos a wifi
  setColor(0x003300); //Leds en verde. Todo Ok
  delay(500);
  setColor(); //Apagados

  //Creamos el dispositivo. Le asignamos un nombre y una función de control
  alexa.addDevice("bombilla",alexaLuz); 
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  delay(1);
}
