/*
  Autor: Ramón Junquera
  Tema: Sistema de archivos de ESP
  Objetivo: Conversión de librería ESP8266FtpServer a ESP. FTP server AP
  Material adicional: placa ESP (ESP32 o ESP8266)
  Descripción:
  Se convierte la librería original de David Paiva.

  En la familia ESP8266 tenemos un plugin incluido en el IDE que nos permite
  subir la carpeta "data" al sistema de archivos (SPIFFS).
  Lo que no tenemos es la opción de descargar los archivos que se hayan podido generar.
  Por otra parte, el artual driver de gestión SPIFFS no oficial para ESP32 no es
  compatible con el plugin del IDE.

  El desarrollo actual viene a resolver todos estos problemas.
  Convertimos el dispositivo es un servidor FTP estándar.

  Se ha configurado el wifi para que cree un punto de acceso propio llamado ESP-FTP
  abierto (sin seguridad).
  Dentro, en la dirección 192.168.4.1 está el servidor FTP cuyas credenciales de acceso son:
    User:     esp
    Password: esp
    
  Cliente FTP.
  Se recomienda utilizar Filezilla como cliente FTP.
  Cuando se crea la entrada del servidor debemos indicar que:
  - El modo de acceso es Normal, con usuario y contraseña
  - El cifrado es "Only use plain FTP (insecure)"
  Y en la solapa de "Opciones de transferencia":
  - El modo de transferencia es pasivo
  - El límite máximo de conexiones simultáneas es 1
 
  El sistema de archivos SPIFFS no es capaz de crear carpetas.
  Todo se guarda en raíz (/).
  Puede confundir que los nombres de los archivo pueden contener el caracter /
  Por ejemplo: /folder1/file1.ext no es un archivo llamado file1.exe contenido en folder1.
  El nombre completo del archivo es así.
  Por esta razón tampoco se podrán crear carpetas a través del cliente FTP.

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

const char* ssid = "ESP-FTP"; //Nombre del punto de acceso

FtpServer ftpSrv; //Creamos el ebjeto de gestión del servidor FTP

void setup(void)
{
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(ssid); //Red abierta

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
