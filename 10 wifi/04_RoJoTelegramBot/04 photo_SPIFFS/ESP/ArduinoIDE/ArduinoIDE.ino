/*
  Autor: Ramón Junquera
  Fecha: 20181007
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen local (SPIFFS)
  Material: placa ESP
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
  TelegramMessage msg;
  bot.getNextMessage(&msg);
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    //Si el comando es /start...
    if (msg.text == "/start")
    {
      //Componemos el texto a enviar en un sólo mensaje (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Envío de imágenes locales\n\n";
      message += "/mountain : jpg\n";
      message += "/rainbow : bmp8\n";
      message += "/rose : bmp24\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Analizamos los distintos comandos...
    else if(msg.text=="/mountain")
    {
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".jpg");
    }
    else if(msg.text=="/rainbow")
    {
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    }
    else if(msg.text=="/rose")
    {
      //Contruimos el nombre completo del archivo de imagen y lo enviamos al chat
      bot.sendPhotoLocal(msg.chat_id,msg.text + ".bmp");
    }    
    else //Si no es ningún comando reconocido...
    {
      //...respondemos con un eco
      bot.sendMessage(msg.chat_id,msg.from_name + " dijo: " + msg.text);
    }

    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
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
