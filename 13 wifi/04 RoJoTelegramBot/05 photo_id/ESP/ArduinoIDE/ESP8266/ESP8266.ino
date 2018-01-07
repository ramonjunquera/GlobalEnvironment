/*
  Autor: Ramón Junquera
  Fecha: 20171226
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen por referencia
  Material: placa ESP8266
  Descripción:
    Al recibir el comando adecuado, enviaremos una imagen referenciada a otra enviada anteriormente

    La primera vez, enviamos la imagen. Ya sea desde local o desde Internet con su URL.
    Una vez enviada, anotamos su file_id para poder referenciarla en sucesivos mensaje.
    Esto siempre es más rápido que enviar la imagen de nuevo.

    Para poder distinguirla, siempre añadimos el comentario a la imagen referenciada.
 */
#include <Arduino.h>
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include <FS.h> //Librería para gestión de archivos
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa

//Definición de variables globales
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;
//Tiempo en milisegundos de la próxima vez que se debe comprobar si hay nuevos mensajes
uint32_t nextCheckTime=0;
//Identificadores de imágenes
String mickey_id="";
String mountain_id="";

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    //Si el comando es /start...
    if (msg.text == "/start")
    {
      //Componemos el texto a enviar en un sólo mensaje (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Envío de imágenes por id\n\n";
      message += "/Mickey : imagen de Internet\n";
      message += "/mountain : imagen local\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Analizamos los distintos comandos...
    else if(msg.text=="/Mickey")
    {
      //Si no tenemos un identificador previo para esta imagen...
      if(mickey_id.length()==0)
      {
        //Enviamos una imagen de Internet y anotamos su identificador
        mickey_id=bot.sendPhotoRemote(msg.chat_id,"https://upload.wikimedia.org/wikipedia/en/d/d4/Mickey_Mouse.png","imagen original");
      }
      //Si no tenemos un identificador previo para esta imagen...
      if(mickey_id.length()==0)
      {
        bot.sendMessage(msg.chat_id,"No se pudo enviar la imagen original");
      }
      else //Tenemos un identificador previo
      {
        bot.sendPhotoRemote(msg.chat_id,mickey_id,"imagen referenciada");
      }
    }
    else if(msg.text=="/mountain")
    {
      //Si no tenemos un identificador previo para esta imagen...
      if(mountain_id.length()==0)
      {
        //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
        mountain_id=bot.sendPhotoLocal(msg.chat_id,msg.text + ".jpg");
      }
      //Si no tenemos un identificador previo para esta imagen...
      if(mountain_id.length()==0)
      {
        bot.sendMessage(msg.chat_id,"No se pudo enviar la imagen original");
      }
      else //Tenemos un identificador previo
      {
        bot.sendPhotoRemote(msg.chat_id,mountain_id,"imagen referenciada");
      }
    }
    else //Si no es ningún comando reconocido...
    {
      //...respondemos con un eco
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text);
    }

    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    msg=bot.getNextMessage();
  }
}

void setup()
{
  Serial.begin(115200);
  //Inicializamos el bot
  bot.begin(botToken);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
  //Fijamos el modo de conexión a un punto de acceso
  WiFi.mode(WIFI_STA);
  //Forzamos a su desconexión (por si estaba conectado anteriormente)
  WiFi.disconnect();
  //Esperamos un momento
  delay(100);
  //Pedimos conexión al punto de acceso
  WiFi.begin(ssid,password);
  //Mientras no estemos conectados...
  while (WiFi.status() != WL_CONNECTED)
  {
    //Cambiaremos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  }
  //Hemos conseguido conectar

  //Nos aseguramos que el led esté apagado
  digitalWrite(pinLed,HIGH);
  
  //Inicializamos el acceso a archivos de SPIFFS
  SPIFFS.begin();
}

void loop()
{
  //Si ya hay que comprobar si tenemos mensajes nuevos...
  if (millis()>nextCheckTime)
  {
    //Procesa todos los nuevos mensajes
    handleNewMessages();
    //Calculamos la próxima vez que debemos comprobar mensajes
    nextCheckTime=millis()+checkingGap;
  }
}
