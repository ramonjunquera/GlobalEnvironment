/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de texto de acción
  Descripción:
    Antes de ejecutar el programa se debe subir la carpeta data con el plugin
    Antes de comenzar a enviar las imágenes locales, se envía un mensaje de acción que le
    llega al usuario para informar que las imágenes se están enviando

    No se puede elegir el texto de los mensajes de acción. Hay un número determinado.
    Los mensajes se traducen automáticamente al idioma que tenga Telegram predeterminado
    Los códigos de los distintos mensajes son:
      //  0 - escribiendo / typing
      //  1 = enviando foto / upload_photo
      //  2 = enviando vídeo / record_video
      //  3 = grabando un mensaje de voz / record_audio
      //  4 = enviando audio / upload_audio
      //  5 = enviando archivo / upload_document
      //  6 = escribiendo / find_location
      //  7 = grabando un videomensaje / record_video_note
      //  8 = grabando un videomensaje / upload_video_note
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

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start") {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Envío de acciones\n\n";
      message += "/mountain : jpg\n";
      message += "/rainbow : bmp8\n";
      message += "/rose : bmp24\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/mountain") {
      //Enviando mensaje de acción: "enviando foto" = actionCode 1
      bot.sendChatAction(msg.chat_id,1);
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".jpg");
    } else if(msg.text=="/rainbow") {
      //Enviando mensaje de acción: "enviando video" = actionCode 2
      bot.sendChatAction(msg.chat_id,2);
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    } else if(msg.text=="/rose") {
      //Enviando mensaje de acción: "grabando un videomensaje" = actionCode 7
      bot.sendChatAction(msg.chat_id,7);
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    } else { //Si no es ningún comando reconocido...
      //...respondemos con un eco
      //Enviando mensaje de acción: "escribiendo" = actionCode 0
      bot.sendChatAction(msg.chat_id,0);
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text);
    }
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
  }
}

void setup() {
  Serial.begin(115200);
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