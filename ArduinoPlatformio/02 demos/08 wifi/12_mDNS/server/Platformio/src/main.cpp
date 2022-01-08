/*
  Autor: Ramón Junquera
  Fecha: 20201230
  Tema: Protocolo mDNS
  Objetivo: Demo de uso de protocolo mDNS (servidor)
  Material: M5Stack Atom Lite o Echo
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h> //Protocolo mDNS
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en Atom Lite
#include <RoJoSwitch.h> //Gestión de pulsadores

//Contantes globales
const char* wifiSSID = "ESPwifi"; //Nombre del punto de acceso generado
const char* wifiPassword = "12345678"; //Contraseña del punto de acceso generado
const char* hostnameLocal="AtomServer"; //Nombre del dispositivo local
const char* hostnameRemote="AtomClient"; //Nombre del dispositivo remoto
const byte pinButton=39;

//Variables globales
WiFiServer server(8266); //Servidor en puerto 8266
RoJoNeoPixelAtomLite led; //Objeto de gestión de led NeoPixel
RoJoSwitch button(pinButton);
bool ledIsBlue=true;
WiFiClient client;

//Dibuja el color del led en función de la variable ledIsBlue
void drawLed() {
  if(ledIsBlue) led.draw({0,0,200}); //Azul
  else led.draw({0,200,0}); //Verde
}

void setup() {
  led.begin(); //Inicialización del led
  led.draw({200,200,200}); //Led blanco mientras inicialización
  WiFi.mode(WIFI_AP); //Se crea un punto de acceso wifi
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña del punto de acceso
  if(!MDNS.begin(hostnameLocal)) { //Si no podemos asignar el nombre...
    led.draw({200,0,0}); //Led en rojo
    vTaskDelete(NULL); //Fin de programa
  }
  server.begin(); //Arrancamos el servidor
  drawLed(); drawLed(); //Mostramos estado actual en el led
}

void loop() {
  if(button.pressed()) { //Si se ha pulsado el botón...
    IPAddress ip=MDNS.queryHost(hostnameRemote); //Pedimos conversión de hostname a ip
    //Si ha podido convertir el hostname a ip consiguiendo una ip distinta de 0.0.0.0 y
    //consigue conexión con el servidor en el puerto indicado...
    if(ip!=0 && client.connect(ip,8266)) {
      client.write(100); //Enviamos algo (no importa qué)
      client.stop(); //Finalizamos la comunicación
    } else led.draw({200,0,0}); //Si hubo algún problema mostramos el led en rojo
  }
  client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si detectamos algún cliente...
    client.read(); //Leemos un byte (no importa el contenido)
    client.stop(); //Finalizamos la comunicación
    ledIsBlue=!ledIsBlue; //Cambiamos el estado al led
    drawLed(); //Aplicamos el nuevo estado al led
  }
}
