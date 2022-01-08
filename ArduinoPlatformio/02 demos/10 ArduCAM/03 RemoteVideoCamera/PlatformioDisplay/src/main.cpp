/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Versión: 20220108
  Objetivo: Transferir imagen en tiempo real por wifi
  Aplicación: servidor (display).
*/

#include <Arduino.h>
#include <WiFi.h>
#include <RoJoSSD1331.h> //Librería de gestión del display SSD1331
#include <RoJoSprite3.h> //Gestión de sprites

//Definimos pines
const byte pinDC=2;
const byte pinRES=15;
const byte pinCS=5;
//Definimos los detalles del punto de acceso
const char* wifiSSID = "displayHost";
const char* wifiPassword = "displayHost";

//Creamos los objetos de gestión
RoJoSSD1331 display;

//Creamos un sprite para guardar la imagen capturada
RoJoSprite3 sprite(2); //Sprite de 16 bits

//Creamos un servidor wifi en el puerto 8266
WiFiServer server(8266);

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  Serial.println("\nsetup start");
  sprite.setSize(96,64); //La imagen del sprite tiene el tamaño de la pantalla
  display.begin(pinRES,pinDC,pinCS); //Inicializamos el display

  //Creamos el punto de acceso
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña

  server.begin(); //Arrancamos el servidor
  Serial.println("setup end");
}

void loop() {
  byte currentByte; //Byte recibido actualmente
  bool controlCode=false; //El código anterior era de control?
  byte x=0,y=0; //Coordenadas actuales
  bool firstByte=true; //Estamos procesando el primer byte de color?
  uint16_t color=0; //Color del pixel

  WiFiClient client = server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    Serial.println("Cliente disponible");
    while (client.connected()) { //Mientras el cliente esté conectado...
      if (client.available()) { //Si el tenemos información pendiente de recibir del cliente...
        currentByte=client.read(); //Leemos el byte recibido
        //Si el byte anterior era un código de control y el actual es el código 0x01=inicio de página...
        if(controlCode && currentByte==0x01) {
          //Hemos recibido la marca que indica el inicio de una nueva imagen
          //Mostraremos en el display la imagen que tenemos ahora
          display.drawSprite(&sprite);
          controlCode=false; //Anotamos para el próximo ciclo que el byte anterior no era de control
          x=y=0; //El próximo pixel se dibujará en las coordenadas 0,0
          firstByte=true; //El próximo byte recibido será el primer byte de color
        }
        //Si el byte anterior no era un código de control pero el actual sí...
        else if(!controlCode && currentByte==0xFF) {
          controlCode=true; //Anotamos para el próximo ciclo que el byte anterior era de control
        } else { //Hemos recibido un byte que debemos preocesar
          if(controlCode) { //Si el byte anterior era de control...
            currentByte=0xFF; //...tiene que ser porque el código actual es 0x00=valor 0xFF
            controlCode=false; //Anotamos para el próximo ciclo que el byte anterior no era de control
          }
          if(firstByte) { //Procesamos el byte recibido?
            color=currentByte; //Anotamos el byte actual como byte bajo del color
            firstByte=false; //El próximo byte será el alto
          } else { //Estamos procesando el segundo byte del color
            color|=((uint16_t)currentByte)<<8; //Completamos el valor del color
            firstByte=true; //El próximo ciclo procesaremos el primer byte de color
            sprite.drawPixel(x,y,color); //Tenemos el color. Lo guardamos en las coordenadas correctas
            //Calculamos las coordenadas del próximo pixel
            //Si hemos terminado con la fila actual...
            if(++x == 96) {
              x=0;
              if(y<63) y++; //Si no estamos en la última fila...pasamos a la siguiente
            }
          }
        }
      }
    }
  }
}

