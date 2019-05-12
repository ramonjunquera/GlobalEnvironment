/*
  Autor: Ramón Junquera
  Fecha: 20190511
  Descripción:
    Ejemplo de gestión de socket seguro. Cliente
*/

#include <Arduino.h>
#include <WiFiClientSecure.h> //Librería para gestión de conexiones https

//Definimos los detalles del punto de acceso
const char* wifiClientSSID = "SecureServer";
const char* wifiClientPassword = "SecureServer";
const char* serverHost = "192.168.4.1"; //Dirección del servidor
const uint16_t port=443; //Puerto
const uint32_t timeout=10000; //Tiempo de espera de nuevos caracteres para enviar
 
WiFiClientSecure client; //Objeto de conexión de cliente seguro
char c; //Carácter leido
uint32_t timeOutMax;

void setup()
{
  Serial.begin(115200);

  //Conexión wifi
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  WiFi.begin(wifiClientSSID,wifiClientPassword);
  Serial.print("Conectando a punto de acceso.");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("ok");
}

void loop()
{
  //Si hay caracteres pendientes de enviar...
  if(Serial.available())
  {
    //Si conectamos con el servidor...
    if(client.connect(serverHost,port))
    {
      Serial.println("Conexión establecida");
      //Calculamos el tiempo máximo de espera de nuevos caracteres
      timeOutMax=millis()+timeout;
      //Mientras no hayamos alcanzado el tiempo máximo de timeout...
      while(timeOutMax>millis())
      {
        //Si hay caracteres pendientes de ser enviados...
        if(Serial.available())
        {
          //Leemos el carácter del puerto serie
          c=Serial.read();
          //Lo enviamos al servidor
          client.write(c);
          //Y lo mostramos
          Serial.print(c);
          //Refrescamos el timeout
          timeOutMax=millis()+timeout;
        }
      }
      //Hemos llegado al tiempo máximo de timeout sin recibir nuevos caracteres
      //Cortamos conexión
      client.stop();
      Serial.println("\nConexión detenida");
    }
    else Serial.println("Error al conectar con el servidor");
  }
}
