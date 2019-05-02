/*
  Autor: Ramón Junquera
  Fecha: 20190502
  Descripción:
    Ejemplo de gestión de socket seguro. Servidor
*/

#include <Arduino.h>
#include "RoJoWiFiServerSecure32.h"

//Definimos los detalles del punto de acceso
const char* wifiSSID="SecureServer";
const char* wifiPassword="SecureServer";
//Archivo de certificado
const char* secureServerCertificateFile="/autocert.crt";
//Archivo de clave privada
const char* secureServerPriveteKeyFile="/private.key";
 
//Creamos un servidor wifi seguro en el puerto indicado
RoJoWiFiServerSecure32 server(443);
//Carácter recibido
byte c;

void setup()
{ 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);

  WiFi.mode(WIFI_AP); //Modo punto de acceso independiente
  WiFi.softAP(wifiSSID,wifiPassword); //Solicitamos conexión

  //Arrancamos el servidor
  byte errorCode=server.begin(secureServerCertificateFile,secureServerPriveteKeyFile);
  Serial.println("Server started with error: "+String(errorCode));
}

void loop(void)
{
  //Anotamos si hay algún cliente
  RoJoWiFiClientSecure32 *client=server.available();
  //Si hay alguna conexión...
  if(client)
  {
    Serial.println("Cliente conectado");
    //Mientras el cliente esté conectado...
    while(client->connected())
    {
      //Si podemos leer el siguiente carácter...lo enviamos al puerto serie
      if(client->read(&c))
      {
        Serial.print(String((char)c));
        //Si el carácter recibido es 'x'...finalizamos la conexión
        if((char)c=='x') client->stop();
      } 
    }
    //Liberamos el objeto de conexión
    delete client;
    Serial.println("\nCliente desconectado");
  }
}