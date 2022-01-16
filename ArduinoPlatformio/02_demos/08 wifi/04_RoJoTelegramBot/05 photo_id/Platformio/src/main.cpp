/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen por referencia
  Descripción:
    Al recibir el comando adecuado, enviaremos una imagen referenciada a otra enviada anteriormente

    La primera vez, enviamos la imagen. Ya sea desde local o desde Internet con su URL.
    Una vez enviada, anotamos su file_id para poder referenciarla en sucesivos mensaje.
    Esto siempre es más rápido que enviar la imagen de nuevo.

    Para poder distinguirla, siempre añadimos el comentario a la imagen referenciada.
*/

#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi para ESP8266
#elif defined(ESP32)
  #include <WiFi.h> //Librería para gestión de wifi para ESP32
#endif
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
#ifdef ARDUINO_LOLIN32LITE
  const byte pinLed=22;
#else
  const byte pinLed=LED_BUILTIN;
#endif

//Definición de variables globales
RoJoTelegramBot bot; //Objeto de gestión del bot
//Identificadores de imágenes
String mickey_id="";
String mountain_id="";

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start") {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Envío de imágenes por id\n\n";
      message += "/Mickey : imagen de Internet\n";
      message += "/mountain : imagen local\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/Mickey") {
      if(mickey_id.length()==0) { //Si no tenemos un identificador previo para esta imagen...
        //Enviamos una imagen de Internet y anotamos su identificador
        mickey_id=bot.sendPhotoRemote(msg.chat_id,"https://upload.wikimedia.org/wikipedia/en/d/d4/Mickey_Mouse.png","imagen original");
      }
      if(mickey_id.length()==0) { //Si no tenemos un identificador previo para esta imagen...
        bot.sendMessage(msg.chat_id,"No se pudo enviar la imagen original");
      } else { //Tenemos un identificador previo
        bot.sendPhotoRemote(msg.chat_id,mickey_id,"imagen referenciada");
      }
    } else if(msg.text=="/mountain") {
      if(mountain_id.length()==0) { //Si no tenemos un identificador previo para esta imagen...
        //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
        mountain_id=bot.sendPhotoLocal(msg.chat_id,msg.text + ".jpg");
        bot.sendMessage(msg.chat_id,"Imágen original");
      }
      if(mountain_id.length()==0) { //Si no tenemos un identificador previo para esta imagen...
        bot.sendMessage(msg.chat_id,"No se pudo enviar la imagen original");
      } else { //Tenemos un identificador previo
        bot.sendPhotoRemote(msg.chat_id,mountain_id,"imagen referenciada");
      }
    } else { //Si no es ningún comando reconocido...
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text); //...respondemos con un eco
    }
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
  }
}

void setup() {
  bot.begin(botToken); //Inicializamos el bot
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  digitalWrite(pinLed,LOW); //Encendemos el led
  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100); //Esperamos un momento a que desconecte
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  while (WiFi.status() != WL_CONNECTED) delay(100); //Esperamos hasta que conecte el wifi
  digitalWrite(pinLed,HIGH); //Apagamos el led
}

void loop() {
  handleNewMessages(); //Procesa todos los mensajes nuevos
  delay(2000); //Esperamos un momento hasta volver a comprobar si hay nuevos mensajes
}