#include "Arduino.h"
#include <WiFi.h>

//Definimos los detalles del punto de acceso
const char* wifiSSID = "displayHost";
const char* wifiPassword = "displayHost";
//Definimos el nombre (o ip) de servidor con el que conectaremos
const char* host = "192.168.4.1";

//Objeto de gestión de la conexión wifi
WiFiClient client;

void setup()
{
  Serial.begin(115200);

  //Conexión wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiSSID,wifiPassword);
  Serial.print(F("Conectando a punto de acceso"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));
  

  Serial.println("setup end");
}

void loop()
{
  //Conexión con el servidor
  if(client.connect(host,8266))
  {
    while(client.connected())
    {
      if(Serial.available())
      {
        char c=Serial.read();
        Serial.print(c);
        client.write(c);
      }
    }
  }
}