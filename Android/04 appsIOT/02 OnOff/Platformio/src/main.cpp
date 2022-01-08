/*
  Tema: Servidor socket de mensajes
  Autor: Ramón Junquera
  Fecha: 20210512

  Se crea un servidor socket simple para la recepción de comandos
  procedentes de la aplicación cliente de Android.
  Los comandos recibidos serán de un sólo byte e interactuan con el led
  integrado:
  - 0 para apagar
  - 1 para enceder
  El cliente (Android) es el encargado de abrir y cerrar la comunicación.
  Mientras la comunicación se mantiene abierta se pueden recibir comandos.
  Se crea un punto de acceso WiFi.
  Por defecto la ip del servidor es 192.168.4.1
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>

//Definimos los detalles del punto de acceso
const char* wifiSSID = "wifiServer";
const char* wifiPassword = "wifiServer";
const uint16_t port=12345; 

WiFiServer server(port); //Creamos un servidor wifi

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //Modo: punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor
  Serial.print("\nServer running on ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(port);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH); //Led apagado
}

void loop() {
  WiFiClient client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    Serial.println(String(millis())+" Cliente conectado");
    while(client.connected()) { //Mientras tengamos el cliente conectado...
      while(client.available()) { //Mientras quede algo pendiente por recibir...
        byte c=client.read(); //Leemos el byte recibido
        digitalWrite(LED_BUILTIN,!c);
        Serial.println(c?"off":"on");
      }
    }
    Serial.println("Cliente desconectado");
  }
}