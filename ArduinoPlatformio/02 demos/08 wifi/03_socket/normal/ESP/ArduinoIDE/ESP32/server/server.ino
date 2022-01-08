#include <Arduino.h>
#include <WiFi.h>

//Definimos los detalles del punto de acceso
const char* wifiSSID = "displayHost";
const char* wifiPassword = "displayHost";
//Creamos un servidor wifi en el puerto 8266
WiFiServer server(8266);

void setup()
{
  Serial.begin(115200);

  //Creamos el punto de acceso
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña

  //Arrancamos el servidor
  server.begin();

  Serial.println("setup end");
}

void loop()
{
  //Anotamos si hay algún cliente
  WiFiClient client = server.available();
  //Si hay alguno...
  if(client)
  {
    Serial.println("Hay algún cliente conectado");
    //Mientras el cliente esté conectado...
    while (client.connected())
    {
      //Si el tenemos información pendiente de recibir del cliente...
      if (client.available())
      {
        //Leemos el byte recibido
        char c=client.read();
        //Lo enviamos al puerto serie
        Serial.print(c);
      }
    }
    Serial.println("Cliente desconectado");
  }
}
