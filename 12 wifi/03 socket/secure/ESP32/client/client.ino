/*
  Autor: Ramón Junquera
  Fecha: 20190429
  Descripción:
    Ejemplo de gestión de socket seguro. Cliente
*/

#include <Arduino.h>
#include <WiFiClientSecure.h> //Librería para gestión de conexiones https

//Definimos los detalles del punto de acceso
const char* wifiClientSSID = "SecureServer";
const char* wifiClientPassword = "SecureServer";
//Definimos el nombre (o ip) de servidor con el que conectaremos
const char* host = "192.168.4.1";
 
//Objeto de conexión de cliente seguro
WiFiClientSecure client; 

void setup()
{
  Serial.begin(115200);

  //Conexión wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiClientSSID,wifiClientPassword);
  Serial.print(F("Conectando a punto de acceso."));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));

  Serial.println("setup end");
}

void loop()
{
  //Conexión con el servidor
  if(client.connect(host,443))
  {
    Serial.println("Conectado!");
    while(client.connected())
    {
      if(Serial.available())
      {
        char c=Serial.read();
        Serial.println("Enviado: "+String(c));
        client.write(c);
      }
    }
    Serial.println("Desconectado!");
  }
}
