/*
  Autor: Ramón Junquera
  Fecha: 20190511
  Descripción:
    Ejemplo de gestión de socket seguro. Servidor
*/

#include <Arduino.h>
#include <RoJoWiFiServerSecure32.h>

//Definimos los detalles del punto de acceso
const char* wifiSSID="SecureServer";
const char* wifiPassword="SecureServer";
//Archivo de certificado
const char* secureServerCertificateFile="/autocert.crt";
//Archivo de clave privada
const char* secureServerPrivateKeyFile="/private.key";
//Puerto de escucha del servidor
const uint16_t port=443;
 
//Creamos un servidor wifi seguro en el puerto indicado
//Daremos un timeout de 10 segundos
RoJoWiFiServerSecure32 server(port,1,10000);
//Carácter recibido
byte c;

void setup()
{ 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);

  WiFi.mode(WIFI_AP); //Modo punto de acceso independiente
  WiFi.softAP(wifiSSID,wifiPassword); //Solicitamos conexión

  //Arrancamos el servidor
  byte errorCode=server.begin(secureServerCertificateFile,secureServerPrivateKeyFile);
  Serial.println("Server started with error: "+String(errorCode));
}

void loop(void)
{
  //Anotamos si hay algún cliente
  RoJoWiFiClientSecure32 *client=server.available();
  //Si hay alguna conexión...
  if(client)
  {
    Serial.println("----Cliente conectado");
    //Mientras tengamos datos que leer...
    while(client->read(&c))
    {
      //...mostramos el carácter recibido
      Serial.print(String((char)c));
    }
    //Ya hemos leido todo lo que teníamos pendiente
    //Cortamos la conexión
    client->stop();
    //Liberamos el objeto de conexión
    delete client;
    Serial.println("\n----Cliente desconectado");
  }
}
