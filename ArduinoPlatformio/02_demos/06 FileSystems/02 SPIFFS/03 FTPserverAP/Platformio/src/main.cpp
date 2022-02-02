/*
  Autor: Ramón Junquera
  Tema:Demo librería RoJoEspFtpServer
  Fecha: 20190716
  Objetivo: Servidor FTP en red propia
  Material adicional: placa ESP
  Descripción:
    Utilizando las librerías RoJoEspFtpServer, crearemos un servidor FTP
    dentro de una nueva red, puesto que también creará su propio punto de acceso wifi.
    Definimos las credenciales para la conexión al punto de acceso en las constantes
    del principio; ssid & password

  Resultado:
    Podemos conectarnos al ESP con un cliente FTP y gestionar remotamente el contenido de SPIFFS
*/

#include <Arduino.h>
#include <RoJoEspFtpServer.h> //Servidor FTP

//Definición de credenciales para conectar a punto de acceso wifi
const char* ssid = "wifiesp";
const char* password = "wifiesp";

RoJoEspFtpServer ftpSrv; //Ojeto de gestión del servidor FTP

void setup(void)
{
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(ssid,password); //Creamos punto de acceso con credenciales

  //Inicializamos el servidor FTP con las credenciales:
  //  usuario: esp
  //  contraseña: esp
  //Si se consigue...
  if(ftpSrv.begin("esp","esp")) Serial.println("Servidor FTP inicializado");
  else Serial.println("Error al inicializar el servidor FTP");
}

void loop(void)
{
  //Simplemente comprobamos contínuamente si hay nuevas solicitudes en el servidor FTP
  ftpSrv.handleFTP();
}