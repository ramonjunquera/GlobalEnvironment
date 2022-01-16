/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de eco

  Descripción:
  El bot responderá con el mismo mensaje que se le envía, precedido por el nombre del usuario.

  Notas:
    Antes de utilizar un bot en Telegram es necesario crearlo:
      Crear una nueva conversación con el usuario BotFather
      Enviar comando /newbot
      Nos solicita un nombre. No puede comenzar por números y debe terminar en bot (no importa mayúsculas o minúsculas)
      Nosotros utilizaremos el nombre RoJoTestBot
      En cuanto se lo demos, nos devolverá un token parecido a:
        464959828:AAHWE3WGVWtwJ_dp2oY7anOToTur2do6omU
    Es conveniente (no imprescindible):
      Completar el perfil del bot añadiendo una imágen con /setuserpic
      Añadir un texto "acerca de" de 120 caracteres máximo que se mostrará en la página de perfil del bot con /setabouttext
      Añadir un texto de "descripción" de 512 caracteres máximo que verán los usuarios al comenzar la conversación con /setdescription
*/

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h> //Librería para gestión de wifi para ESP32
#else //ESP8266
  #include <ESP8266WiFi.h> //Librería para gestión de wifi para ESP8266
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
      message += "Devuelve el mismo mensaje, precedido por el nombre del autor\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else { //Si no es el comando /start
      //Devolvemos el mismo texto recibido, precedido del nombre del autor
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