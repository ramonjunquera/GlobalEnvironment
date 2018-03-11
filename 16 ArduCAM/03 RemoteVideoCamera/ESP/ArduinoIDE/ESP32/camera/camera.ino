/*
  Autor: Ramón Junquera
  Tema: ArduCAM
  Versión: 20180311
  Objetivo: Transferir imagen en tiempo real por wifi
  Aplicación: cliente (cámara).
  Material adicional: placa ESP32, ArduCAM-Mini-2MP
*/

#include "Arduino.h"
#include <WiFi.h>
#include "RoJoArduCAM.h" //Librería de gestión de ArduCAM
#include "RoJoSprite16.h" //Gestión de sprites color

//Definimos pines
const byte pinCS=4;
//Definimos el punto de acceso con el que conectaremos
const char* wifiSSID = "displayHost";
const char* wifiPassword = "displayHost";
//Definimos el nombre (o ip) de servidor al que enviaremos las imágenes
const char* host = "192.168.4.1";

//Objeto de gestión de la cámara
RoJoArduCAM camera;
//Creamos el sprite que utlizaremos para guardar la imagen
RoJoSprite16 sprite;
//Objeto de gestión de la conexión wifi
WiFiClient client;

void setup()
{
  Serial.begin(115200);

  //Inicializamos la cámara
  byte errorCode = camera.begin(pinCS);
  //Si tenemos algún error en la inicialización...
  if(errorCode)
  {
    Serial.print(F("Camara inicializada con error "));
    Serial.println(errorCode);
    //Led integrado configurado como salida
    pinMode(BUILTIN_LED,OUTPUT);
    //Led integrado apagado
    digitalWrite(BUILTIN_LED,HIGH);
    //Bucle infinito
    while(1)
    {
      //Repetimos tantas veces como código de error
      for(byte i=0;i<errorCode;i++)
      {
        digitalWrite(BUILTIN_LED,LOW); //Encendido
        delay(300);
        digitalWrite(BUILTIN_LED,HIGH); //Apagado
        delay(300);
      }
      delay(1000);
    };    
  }
  //Capturaremos imágenes sin comprimir
  camera.setFormatBMP();
  //Utilizaremos la resolución más baja (160x120)
  camera.setResolution(0);
  //Conexión wifi
  WiFi.begin(wifiSSID,wifiPassword);
  Serial.print(F("Conectando a punto de acceso"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));
  //Conexión con el servidor
  if(!client.connect(host,8266))
  {
    Serial.println(F("Error al conectar con el servidor"));
    while(1)
    {
      yield();
    }
  }
  //Hemos conseguido conectar con el servidor
  Serial.println("Conectado con el servidor");
}

void sendByte(byte b)
{
  //Envía un byte al servidor

  //El carácter 0xFF se considera código de escape
  //En el caso que se tenga que enviar el valor 0xFF se tendrá
  //que enviar el código de escape seguido de otro byte con 
  //valor 0x00

  client.write(b);
  if(b==0xFF) client.write((byte)0);
}

void loop()
{
  byte errorCode;
  
  //Capturamos una imagen
  camera.takePhoto();
  do
  {
    //Pedimos que la imagen capturada se guarde en el sprite, redimensionado
    //a las medidas del display y anotamos el código de error
    errorCode=camera.savePhoto(96,64,&sprite);
  } while(errorCode==3); //Repetimos mientras no se haya terminado la captura
  //La captura ha terminado
  //Si tenemos algún otro error...
  if(errorCode)
  {
    //...lo mostramos
    Serial.print(F("errorCode="));
    Serial.println(errorCode);
    //...y terminamos
    return;
  }
  //No hay errores
  //Enviamos el contenido del sprite al servidor

  //Enviamos marca de inicio de imagen
  client.write(0xFF); //Código de escape
  client.write(0x01); //Inicio de imagen
  //Recorremos filas
  for(byte y=0;y<64;y++)
  {
    //Recorremos columnas
    for(byte x=0;x<96;x++)
    {
      //Obtenemos color de pixel
      uint16_t color=sprite.getPixel(x,y);
      //Descomponemos en byte
      byte lb=color & 0xFF;
      byte hb=color >> 8;
      //Enviamos cada byte
      sendByte(lb);
      sendByte(hb);
    }
  }
}

