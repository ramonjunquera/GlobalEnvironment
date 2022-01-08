/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen local (LittleFS)
  Descripción:
    Antes de ejecutar el programa se debe subir la carpeta data con el plugin
    Al recibir el comando adecuado, enviaremos una imagen obtenida del sistema de archivos local
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
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start") {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Envío de imágenes locales\n\n";
      message += "/mountain : jpg\n";
      message += "/rainbow : bmp8\n";
      message += "/rose : bmp24\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/mountain") {
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".jpg");
    } else if(msg.text=="/rainbow") {
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    } else if(msg.text=="/rose") {
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    } else { //Si no es ningún comando reconocido...
      //...respondemos con un eco
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text);
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