/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Objetivo: Transferir imagen en tiempo real por wifi
  Aplicación: servidor (display).
  Material adicional: placa ESP32, display SSD1331
*/

#include "Arduino.h"
#include <WiFi.h>
#include "RoJoSSD1331.h" //Librería de gestión del display SSD1331
#include "RoJoSprite16.h" //Gestión de sprites color

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
RoJoSprite16 sprite;

//Creamos un servidor wifi en el puerto 8266
WiFiServer server(8266);

void setup()
{
  Serial.begin(115200);

  Serial.println("setup start");
  //La imagen del sprite tiene el tamaño de la pantalla
  sprite.setSize(96,64);

  //Inicializamos el display
  display.begin(pinDC,pinRES,pinCS);

  //Creamos el punto de acceso
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña

  //Arrancamos el servidor
  server.begin();

  Serial.println("setup end");
}

void loop()
{
  //Byte recibido actualmente
  byte currentByte;
  //El código anterior era de control?
  bool controlCode=false;
  //Coordenadas actuales
  byte x,y;
  //Estamos procesando el primer byte de color?
  bool firstByte=true;
  //Color del pixel
  uint16_t color;

  //Anotamos si hay algún cliente
  WiFiClient client = server.available();
  //Si hay alguno...
  if(client)
  {
    Serial.println("Cliente disponible");
    //Mientras el cliente esté conectado...
    while (client.connected())
    {
      //Si el tenemos información pendiente de recibir del cliente...
      if (client.available())
      {
        //Leemos el byte recibido
        currentByte=client.read();
        //Si el byte anterior era un código de control y el actual es el código
        //0x01=inicio de página...
        if(controlCode && currentByte==0x01)
        {
          //Hemos recibido la marca que indica el inicio de una nueva imagen
          //Mostraremos en el display la imagen que tenemos ahora
          display.drawSprite(0,0,&sprite);
          //Anotamos para el próximo ciclo que el byte anterior no era de control
          controlCode=false;
          //El próximo pixel se dibujará en las coordenadas 0,0
          x=y=0;
          //El próximo byte recibido será el primer byte de color
          firstByte=true;
        }
        //Si el byte anterior no era un código de control pero el actual sí...
        else if(!controlCode && currentByte==0xFF)
        {
          //Anotamos para el próximo ciclo que el byte anterior era de control
          controlCode=true;
        }
        else //Hemos recibido un byte que debemos preocesar
        {
          //Si el byte anterior era de control...
          if(controlCode)
          {
            //...tiene que ser porque el código actual es 0x00=valor 0xFF
            currentByte=0xFF;
            //Anotamos para el próximo ciclo que el byte anterior no era de control
            controlCode=false;
          }

          //Procesamos el byte recibido?
          if(firstByte)
          {
            //Anotamos el byte actual como byte bajo del color
            color=currentByte;
            //El próximo byte será el alto
            firstByte=false;
          }
          else //Estamos procesando el segundo byte del color
          {
            //Completamos el valor del color
            color|=((uint16_t)currentByte)<<8;
            //El próximo ciclo procesaremos el primer byte de color
            firstByte=true;
            //Tenemos el color. Lo guardamos en las coordenadas correctas
            sprite.setPixel(x,y,color);
            //Calculamos las coordenadas del próximo pixel
            //Si hemos terminado con la fila actual...
            if(++x == 96)
            {
              x=0;
              //Si no estamos en la última fila...pasamos a la siguiente fila
              if(y<63) y++;
            }
          }
        }
      }
    }
  }
}

