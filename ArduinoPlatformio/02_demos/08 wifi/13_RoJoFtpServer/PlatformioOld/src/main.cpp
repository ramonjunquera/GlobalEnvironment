/*
  Autor: Ramón Junquera
  Tema:Demo librería RoJoEspFtpServer
  Fecha: 20230210
  Objetivo: Servidor FTP en red local
  Material adicional: placa ESP
  Descripción:
    Utilizando las librerías RoJoFtpServer, crearemos un servidor FTP
    dentro de nuestra red local.

  Resultado:
    Podemos conectarnos al ESP con un cliente FTP (recomendado el de FileZilla) y gestionar
    remotamente su contenido.
*/

//Si se define ACCESSPOINT se creará un punto de acceso propio
//Si no se define, se conectará a un punto de acceso existente
//#define ACCESSPOINT

#include <Arduino.h>
#include <RoJoFtpServer.h> //Servidor FTP

//Configuración de tipo de conexión
//- comentado: se conectará a un punto de acceso wifi existente
//- descomentado: creará su propio punto de acceso wifi
#define ACCESSPOINT

//Credenciales de acceso al servidor FTP
const String ftpUser="esp";
const String ftpPassword="esp";

//Definición de credenciales de SSID
const char* ssidName = "FTP-Server";
const char* ssidPassword = "12345679";

RoJoFtpServer ftpServer; //Objeto de gestión del servidor FTP

void setup(void) {
  Serial.begin(115200);
  delay(3000);

  #ifdef ACCESSPOINT //Si debemos crear un punto de acceso propio...
    //Parámetros de creación de un punto ce acceso propio
    WiFi.mode(WIFI_AP); //Modo punto de acceso
    WiFi.softAP(ssidName,ssidPassword); //Creamos punto de acceso con credenciales
  #else //Si debemos conectarnos a un punto de acceso existente...
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidName,ssidPassword);
    Serial.print("\nConnecting.");
    while (WiFi.status()!=WL_CONNECTED) { //Esperamos a que termine de conectar
      delay(500);
      Serial.print(".");
      if(millis()>10000) ESP.restart(); //Si no conecta en 10s...reiniciamos
    }
    Serial.println("ok");
    //Hemos conectado
    //Podríamos asignar una dirección IP estática
    //WiFi.config(IPAddress(192,168,1,190),WiFi.gatewayIP(),WiFi.subnetMask());
  #endif

  if(ftpServer.begin(ftpUser,ftpPassword)) { //Si conseguimos inicializar el servidor...
    Serial.print("\nFTP Server is running on ");
    Serial.println(ftpServer.getIP());
  } else Serial.println("Error starting FTP server");
}

void loop(void) {
  ftpServer.refresh(); //Procesa nuevas peticiones 
}
