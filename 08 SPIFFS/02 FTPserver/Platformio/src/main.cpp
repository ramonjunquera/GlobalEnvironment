/*
  Autor: Ramón Junquera
  Tema:Demo librería RoJoEspFtpServer
  Fecha: 20190716
  Objetivo: Servidor FTP en red local
  Material adicional: placa ESP
  Descripción:
    Utilizando las librerías RoJoEspFtpServer, crearemos un servidor FTP
    dentro de nuestra red local.
    Se conectará a un punto de acceso local.
    Definimos las credenciales para la conexión al punto de acceso en las constantes
    del principio; ssid & password

  Resultado:
    Podemos conectarnos al ESP con un cliente FTP y gestionar remotamente el contenido de SPIFFS
*/

#include <Arduino.h>
#include <RoJoEspFtpServer.h> //Servidor FTP

//Definición de credenciales para conectar a punto de acceso wifi
const char* ssid = "...";
const char* password = "...";

RoJoEspFtpServer ftpSrv; //Ojeto de gestión del servidor FTP

void setup(void)
{
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
  //Esperamos a que termine de conectar
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("ok");
  //Hemos conectado
  //Podemos asignar una dirección IP estática
  //WiFi.config(IPAddress(192,168,1,190),WiFi.gatewayIP(),WiFi.subnetMask());

  //Inicializamos el servidor FTP con las credenciales:
  //  usuario: esp
  //  contraseña: esp
  //Si se consigue...
  if(ftpSrv.begin("esp","esp"))
  {
    //Mostramos la IP del servidor
    Serial.print("FTP Setver at ");
    Serial.println(WiFi.localIP());
  }
  else Serial.println("Error al inicializar el servidor FTP");
}

void loop(void)
{
  //Simplemente comprobamos contínuamente si hay nuevas solicitudes en el servidor FTP
  ftpSrv.handleFTP();
}
