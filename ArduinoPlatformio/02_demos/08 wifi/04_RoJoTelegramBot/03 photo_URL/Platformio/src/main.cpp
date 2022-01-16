/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen de una URL
  Descripción:
    Al recibir el comando adecuado, enviaremos una imagen obtenida de una dirección de Internet
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
      String message = "RoJo Telegram Bot library\n\n";
      message += "/Mickey : foto sin texto\n";
      message += "/Minnie : foto con texto\n";
      message += "/Pluto : foto sin notificación\n";
      message += "/Goofy : foto en referencia a un mensaje\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/Mickey") { //Si el comando es /Mickey...
      //...enviamos una imagen de una URL de Internet
      String photoURL="https://upload.wikimedia.org/wikipedia/en/d/d4/Mickey_Mouse.png";
      //Enviamos la foto sin texto
      bot.sendPhotoRemote(msg.chat_id,photoURL);
    } else if(msg.text=="/Minnie") { //Si el comando es /Minnie...
      //...enviamos una imagen de una URL de Internet
      String photoURL="https://upload.wikimedia.org/wikipedia/en/6/67/Minnie_Mouse.png";
      //Enviamos la foto con texto
      bot.sendPhotoRemote(msg.chat_id,photoURL,"Para " + msg.from_name);
    } else if(msg.text=="/Pluto") { //Si el comando es /Pluto...
      //...enviamos una imagen de una URL de Internet
      String photoURL="https://vignette.wikia.nocookie.net/parody/images/a/ac/DSNY-SM-13.jpg/revision/latest?cb=20140920064657";
      //Enviamos la foto pero no se notificará cuando se reciba
      bot.sendPhotoRemote(msg.chat_id,photoURL,"",true);
    } else if(msg.text=="/Goofy") { //Si el comando es /Goofy...
      //...enviamos una imagen de una URL de Internet
      String photoURL="https://vignette.wikia.nocookie.net/disney/images/2/27/Goofy_transparent.png/revision/latest?cb=20170426165903";
      //Enviamos la foto como réplica al mensaje recibido
      bot.sendPhotoRemote(msg.chat_id,photoURL,"",false,msg.message_id);
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