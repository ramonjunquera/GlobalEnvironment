/*
  Tema: Servidor socket tcp con led RGB
  Autor: Ramón Junquera
  Fecha: 20210513

  Se crea un servidor socket para la recepción de colores que se
  aplicarán a un led RGB.
  Un color estará definido por 3 bytes con la información de cada uno
  de los canales de color.
  El valor 255 se considera el comando para resetear la cuenta de canales de color.
  El cliente (Android) es el encargado de abrir y cerrar la comunicación.
  Mientras la comunicación se mantiene abierta se pueden recibir comandos.
  Se crea un punto de acceso WiFi.
  Por defecto la ip del servidor es 192.168.4.1

  Los bytes se reciben de uno en uno.
  Llevamos la cuenta de cuántos bytes hemos recibido para el color actual.
  Cuando se completa un color, se muestran sus canales y se aplica.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>

//Definición de constantes globales
const char* wifiSSID = "wifiServer";
const char* wifiPassword = "wifiServer";
const uint16_t port=12345;
//Pinout RGB
const byte pins[3]={D0,D1,D2};

WiFiServer server(port); //Creamos un servidor wifi

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //Modo: punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor
  Serial.print("\nServer running on ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(port);
}

void loop() {
  WiFiClient client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    Serial.println(String(millis())+" Cliente conectado");
    byte channelIndex=0; //Indice del canal de color a recibir
    uint16_t color[3]; //Canales de color
    while(client.connected()) { //Mientras tengamos el cliente conectado...
      if(client.available()) { //Si tenemos información pendiente...
        byte b=client.read(); //Leemos el siguiente byte
        if(b==255) { //Si es un comando reset...
          channelIndex=0; //Reseteamos el índice de canales (nuevo color)
          Serial.print("R "); //Indicamos que hemos recibido un reset
        } else { //No es un comando reset
          //Guardamos el byte leido en el canal que corresponde
          //Aumentamos el índice de canal para el siguiente ciclo
          color[channelIndex++]=b; 
          if(channelIndex==3) { //Si hemos completado el color...
            channelIndex=0; //Reseteamos el canal
            //Mostramos el color recibido
            Serial.printf("%i,%i,%i\n",color[0],color[1],color[2]);
            //Aplicamos los canales a cada led
            //Convertimos de 8bits a 10bits
            //Tomamos su negativo, porque el led RGB tiene ánodo común
            for(byte i=0;i<3;i++) analogWrite(pins[i],1023-(color[i])*4);
          }
        }
      }
      yield(); //Permitimos que se ejecuten las tareas de fondo
    }
    Serial.println("Cliente desconectado");
  }
}