/*
  Autor: Ramón Junquera
  Fecha: 20171224
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de eco
  Material: placa ESP8266

  Descripción:
  El bot responderá con el mismo mensaje que se le envía, precidido por el nombre del usuario.

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
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en milisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa

//Definición de variables globales
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;
//Tiempo en milisegundos de la próxima vez que se debe comprobar si hay nuevos mensajes
uint32_t nextCheckTime=0;

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
  while(msg.text.length())
  {
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start")
    {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n\n";
      message += "Devuelve el mismo mensaje, precedido por el nombre del autor\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    else //Si no es el comando /start
    {
      //Devolvemos el mismo texto recibido, precedido del nombre del autor
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text);
    }
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    msg=bot.getNextMessage();
  }
}

void setup()
{
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
