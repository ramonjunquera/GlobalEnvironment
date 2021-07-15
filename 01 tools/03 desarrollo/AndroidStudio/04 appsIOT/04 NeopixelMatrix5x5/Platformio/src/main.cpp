/*
  Tema: Servidor socket tcp con matriz de leds NeoPixel 5x5
  Autor: Ramón Junquera
  Fecha: 20210710
  Objetivo:
    Se crea un servidor socket para la recepción de colores que se
    aplicarán a la matriz de leds NeoPixel.
  Funcionamiento:
    La conexión permanece abierta aunque no se genera tráfico.
    La información se recibe en paquetes.
    Un paquete consta de 4 bytes.
    El primero contiene las coordenadas del pixel a mopificar em formato ..xxxyyy
    Los 3 siguientes bytes corresponden a los canales de color (RGB).
    El cliente (Android) es el encargado de abrir y cerrar la comunicación.
    Se crea un punto de acceso WiFi.
    Por defecto la ip del servidor es 192.168.4.1
  Notas:
  - A veces, tras un tiempo sin tráfico, se recibe como primer byte del paquete 255.
    No sé porqué.
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
    byte buffer[4]; //Buffer de recepción. 4 bytes=1 para coordenadas y 3 para canales de color
    byte bufferIndex=0; //Posición a llenar en el buffer de recepción
    while(client.connected()) { //Mientras tengamos el cliente conectado...
      if(client.available()) { //Si tenemos información pendiente...
        byte b=client.read(); //Leemos el siguiente byte
        if(bufferIndex==0 && b==255) { //Si el primer byte es 255...
          //no lo tenemos en cuenta
          //No se porqué, a veces, aparece un 255 despues de un tiempo sin enviar
          Serial.println("Info. 255 eliminado como primer byte");
        } else { //El primer byte no es 255
          //Mostramos un punto por cada byte recibido
          Serial.print(".");
          //Guardamos el byte leido en buffer de entrada
          //Aumentamos el índice de la posición de entrada del buffer
          buffer[bufferIndex++]=b;
          if(bufferIndex==4) { //Si ya hemos recibido 4 bytes (paquete completo)...
            //Decodificamos coordenadas
            //Formato del byte de coordenadas: ..xxxyyy
            byte y=buffer[0] & 0b00000111;
            byte x=(buffer[0] >> 3) & 0b00000111;
            if(x<5 && y<5) { //Si las coordenadas parecen correctas...
              //Dibujamos el pixel en la memoria de vídeo
              leds.v->drawPixel(x,y,leds.v->getColor(buffer[1],buffer[2],buffer[3]));
              leds.draw(); //La dibujamos
              Serial.print(" "+String(x)+","+String(y)); //Informamos de las coordenadas
              for(byte i=1;i<4;i++) Serial.print(" "+String(buffer[i])); //Informamos de los canales de color
              Serial.println(" ok");
            } else {
              Serial.print(" "+String(x)+","+String(y)); //Informamos de las coordenadas
              for(byte i=1;i<4;i++) Serial.print(" "+String(buffer[i])); //Informamos de los canales de color
              Serial.println(" wrong pack");
            }
            bufferIndex=0; //Reseteamos el índice del buffer
          }
        }
      }
    }
    Serial.println("Cliente desconectado");
  }
}
