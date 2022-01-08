/*
  Tema: Servidor socket tcp con matriz de leds NeoPixel 16x16
  Autor: Ramón Junquera
  Fecha: 20210827

  Objetivo:
    Se crea un servidor socket para la recepción de colores que se
    aplicarán a la matriz de leds NeoPixel.
  Funcionamiento:
    La conexión permanece abierta aunque no se genera tráfico.
    La información se recibe en paquetes.
    Un paquete consta de 4 bytes.
    El primero contiene las coordenadas del pixel a mopificar em formato xxxxyyyy
    Los 3 siguientes bytes corresponden a los canales de color (RGB).
    El cliente (Android) es el encargado de abrir y cerrar la comunicación.
    Se crea un punto de acceso WiFi.
    Por defecto la ip del servidor es 192.168.4.1
*/

#include <Arduino.h>
#include <RoJoNeoPixel2M16x16.h>
#ifdef ESP32
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//Definición de constantes globales
const char* wifiSSID = "wifiServer";
const char* wifiPassword = "wifiServer";
const uint16_t port=12345;

#ifdef ESP32
  const byte pin=26;
#elif defined(ESP8266)
  const byte pin=D8;
#endif

//Variables globales
WiFiServer server(port); //Creamos un servidor wifi
RoJoNeoPixel2M16x16 leds;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //Modo: punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor
  Serial.print("\nServer running on ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(port);
  leds.begin(pin);
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
        Serial.print("."); //Mostramos un punto por cada byte recibido
        //Guardamos el byte leido en buffer de entrada
        //Aumentamos el índice de la posición de entrada del buffer
        buffer[bufferIndex++]=b;
        if(bufferIndex==4) { //Si ya hemos recibido 4 bytes (paquete completo)...
          //Decodificamos coordenadas
          //Formato del byte de coordenadas: xxxxyyyy
          byte y=buffer[0] & 0b00001111;
          byte x=(buffer[0] >> 4);
          //Dibujamos el pixel en la memoria de vídeo
          leds.drawPixel(x,y,((uint32_t)buffer[1]<<16)|(((uint32_t)buffer[2])<<8)|buffer[3]);
          //leds.v->drawPixel(x,y,leds.v->getColor(buffer[1],buffer[2],buffer[3]));
          leds.show(); //La dibujamos
          Serial.print(" "+String(x)+","+String(y)); //Informamos de las coordenadas
          for(byte i=1;i<4;i++) Serial.print(" "+String(buffer[i])); //Informamos de los canales de color
          Serial.println(" ok");
          bufferIndex=0; //Reseteamos el índice del buffer
        }
      }
    }
    Serial.println("Cliente desconectado");
  }
}
