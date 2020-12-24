/*
  Autor: Ramón Junquera
  Fecha: 20201223
  Tema: librería Espalexa
  Objetivo: Ejemplo de control de color con ESP8266

  Descripción:
    Se utiliza una placa WeMos D1 mini cun un led RGB
    Se definirá un dispositivo llamado luz.
    Se pueden apagar, encender o cambiar de brillo.
*/

#include <Arduino.h>
#include <Espalexa.h>

//Constantes globales
const byte pinR=D7;
const byte pinG=D6;
const byte pinB=D5;
const char* ssid="xxx";
const char* password="xxx";

//Variables globales
Espalexa alexa; //Objeto de gestión de Alexa

//Fija un color en el led
void setColor(byte r=0,byte g=0,byte b=0) {
  //Los parámetros de entrada son del tipo byte [0,255]
  //El rango de valores de analogWrite en ESP8266 es [0,1023]
  //Puesto que el led tiene ánodo (conectado a voltaje, no a tierra),
  //debemos indicar el inverso del valor deseado.
  //Debemos convertirlos
  analogWrite(pinR,map(r,0,255,1023,0));
  analogWrite(pinG,map(g,0,255,1023,0));
  analogWrite(pinB,map(b,0,255,1023,0));
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
  setColor(r,g,b);
}

void setup() {
  //Los pines de conexión del led serán de salida
  pinMode(pinR,OUTPUT); pinMode(pinG,OUTPUT); pinMode(pinB,OUTPUT);
  setColor(255,0,0); //Comenzamos en rojo
  delay(500);
  setColor(255,255,0); //Led en amarillo mientras conectamos a wifi
  wifiSetup(); //Conectamos a wifi
  setColor(0,255,0); //Led en verde. Todo Ok
  delay(500);
  setColor(); //led apagado

  alexa.addDevice("luz",alexaLuz); //Creamos el dispositivo
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  delay(1);
}
