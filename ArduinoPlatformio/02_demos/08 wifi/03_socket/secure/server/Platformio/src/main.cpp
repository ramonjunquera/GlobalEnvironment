/*
  Autor: Ramón Junquera
  Fecha: 20211026
  Descripción:
    Ejemplo de gestión de socket seguro. Servidor
  Prerequisitos:
    - Generar los archivos de clave y certificado. Ya sea desde el PC, o
      en el mismo dispositivo.
      En para este ejemplo se ha utilizado certificados de 1024bits
*/

#include <Arduino.h>
#include <RoJoWiFiServerSecure32.h>
#include <RoJoNeopixel2.h>

//Declaración de constantes globales
//Definimos los detalles del punto de acceso
const char* wifiSSID="SecureServer";
const char* wifiPassword="SecureServer";
const char* secureServerCertificateFile="/autocert.crt"; //Archivo de certificado
const char* secureServerPrivateKeyFile="/private.key"; //Archivo de clave privada
const uint16_t port=443; //Puerto de escucha del servidor
const byte pinLed=27; //Pin de led NeoPixel

//Declaración de variables globales
//Creamos un socket seguro en el puerto indicado.
//Por defecto sólo admitirá 1 conexión simultánea y tendrá un timeout de 1ms
RoJoWiFiServerSecure32 server(port);
RoJoNeoPixel2 led;
byte c; //Carácter recibido

void setup() { 
  Serial.begin(115200);
  led.begin(1,pinLed); //Sólo tenemos un led
  led.videoMem[0].setColor(0x880000); led.show(); //Rojo
  WiFi.mode(WIFI_AP); //Modo punto de acceso independiente
  WiFi.softAP(wifiSSID,wifiPassword); //Solicitamos conexión

  //Arrancamos el servidor
  byte errorCode=server.begin(secureServerCertificateFile,secureServerPrivateKeyFile);
  if(!errorCode) { //Si no hubo errores arrancando el server...
    Serial.println("\nServidor iniciado correctamente!");
    led.videoMem[0].setColor(0x888800); led.show(); //Amarillo
  } else Serial.println("\nError iniciando servidor ["+String(errorCode)+"]");
}

void loop(void) {
  //Anotamos si hay algún cliente
  RoJoWiFiClientSecure32 *client=server.available();
  if(client) { //Si hay alguna conexión...
    led.videoMem[0].setColor(0x008800); led.show(); //Verde
    Serial.print("Cliente conectado...");
    while(client->connected()) { //Mientras el cliente se mantenga conectado...
      //Si hemos conseguido recibir un nuevo byte...lo enviamos por el puerto serie
      if(client->read(&c)) Serial.print((char)c);
    }
    //Han cerrado la conexión
    delete client; //Liberamos el objeto de conexión
    led.videoMem[0].setColor(0x888800); led.show(); //Amarillo
    Serial.println("...Cliente desconectado");
  }
}
