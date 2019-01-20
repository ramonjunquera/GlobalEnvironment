/*
  Autor: Ramón Junquera
  Tema: Sistema de archivos de ESP
  Objetivo: Conversión de librería ESP8266FtpServer a ESP. FTP server
  Material adicional: placa ESP (ESP32 o ESP8266)
  Descripción:
  Basado en el ejemplo anterior.
  Misma funcionalidad, pero crearemos un servidor web integrado en nuestra red.
  Se conectará como cliente a un punto de acceso. No creará uno propio.
  Sólo cambia este program, no la librería EspFtpServer.

  Definimos las credenciales para la conexión al punto de acceso en las constantes
  del principio; ssid & password

  En la función setup también se le asigna una dirección ip estática para facilitarnos
  su localización.

  Resultado:
  Podemos conectarnos al ESP con un cliente FTP y gestionar remotamente el contenido de SPIFFS
*/

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h> //Para crear el punto de acceso
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClient.h>
#include "EspFtpServer.h" //Para crear el servidor FTP

const char* ssid = "xxx";
const char* password = "xxx";

FtpServer ftpSrv; //Creamos el ebjeto de gestión del servidor FTP

void setup(void)
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Esperamos a que termine de coenctar
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("ok");
  //Hemos conectado
  //Asignamos una dirección estática
  WiFi.config(IPAddress(192,168,1,190),WiFi.gatewayIP(),WiFi.subnetMask());

  //Si se ha podido inicializar correctamente el sistema de archivos...
  if (SPIFFS.begin())
  {
    //...arrancamos el servidor FTP pasándole como parámetros el usuario y la contraseña de acceso
    ftpSrv.begin("esp","esp");
  }    
}

void loop(void)
{
  //Simplemente comprobamos contínuamente si hay nuevas solicitudes en el servidor FTP
  ftpSrv.handleFTP();
}
