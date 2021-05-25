/*
  Tema: Servidor socket tcp con matriz de leds NeoPixel 5x5
  Autor: Ramón Junquera
  Fecha: 20210517

  Se crea un servidor socket para la recepción de colores que se
  aplicarán a la matriz de leds NeoPixel.
  Se recibirán los colores de todos los pixels por filas, por columnas y
  por canales (RGB).
  El valor 255 se considera el comando para resetear los contadores
  El cliente (Android) es el encargado de abrir y cerrar la comunicación.
  Mientras la comunicación se mantiene abierta se pueden recibir comandos.
  Se crea un punto de acceso WiFi.
  Por defecto la ip del servidor es 192.168.4.1
*/

#include <Arduino.h>
#include <WiFi.h>
#include <RoJoNeoPixel.h>

//Definición de constantes globales
const char* wifiSSID = "wifiServer";
const char* wifiPassword = "wifiServer";
const uint16_t port=12345;

//Variables globales
WiFiServer server(port); //Creamos un servidor wifi
RoJoNeoPixel leds;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //Modo: punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor
  Serial.print("\nServer running on ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(port);
  leds.begin(5,5,27); //Init para M5Stack Atom Matrix: 5x5 en pin 27
}

void loop() {
  WiFiClient client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    Serial.println(String(millis())+" Cliente conectado");
    byte rowIndex=0; //Índice de la fila del byte recibido
    byte colIndex=0; //Índice de la columna del byte recibido
    byte channelIndex=0; //Índice del canal de color del byte recibido
    byte color[3]; //Canales de color del color recibido
    while(client.connected()) { //Mientras tengamos el cliente conectado...
      if(client.available()) { //Si tenemos información pendiente...
        byte b=client.read(); //Leemos el siguiente byte
        if(b==255) { //Si es un comando reset...
          rowIndex=colIndex=channelIndex=0; //Reset de índices
          Serial.print("R "); //Indicamos que hemos recibido un reset
        } else { //No es un comando reset
          //Mostramos un punto por cada byte recibido
          //Una imágen completa tiene 5x5x3=75 bytes
          Serial.print(".");
          //Guardamos el byte leido en el canal que corresponde
          //Aumentamos el índice de canal para el siguiente ciclo
          color[channelIndex++]=b; 
          if(channelIndex==3) { //Si hemos completado el color...
            //Dibujamos el pixel en la memoria de vídeo
            leds.v->drawPixel(colIndex,rowIndex,leds.v->getColor(color[0],color[1],color[2]));
            channelIndex=0; //Reseteamos el canal de color
            //Tenemos que pasar a la siguiente columna
            if(++colIndex>4) { //Si hemos terminado todas las columnas...
              colIndex=0; //Reseteamos el índice de columna
              //Tenemos que pasar a la siguiente fila
              if(++rowIndex>4) { //Si hemos terminado todas las filas...
                rowIndex=0; //Reseteamos el índice de fila
                //Hemos recibido la imágen completamente
                leds.draw(); //La dibujamos
                Serial.println(" ok"); //Informamos que hemos completado la imágen
              }
            }
          }
        }
      }
    }
    Serial.println("Cliente desconectado");
  }
}
