/*
  Tema: Servidor socket de mensajes
  Autor: Ramón Junquera
  Fecha: 20210511

  Se crea un servidor socket simple para la recepción de mensajes
  procedentes de la aplicación cliente de Android.
  Los mensajes recibidos se enviarán al puerto serie.
  El cliente (Android) es el encargado de abrir y cerrar la comunicación.
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
}

void loop() {
  WiFiClient client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    Serial.println(String(millis())+" Cliente conectado");
    while(client.connected()) { //Mientras tengamos el cliente conectado...
      while(client.available()) { //Mientras quede algo pendiente por recibir...
        char c=client.read(); //Leemos el byte recibido
        Serial.print(c); //Lo enviamos al puerto serie
      }
    }
    Serial.println("\nCliente desconectado");
  }
}