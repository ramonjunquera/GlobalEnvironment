/*
  Autor: Ramón Junquera
  Fecha: 20171225
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de envío de una imágen de una URL
  Material: placa ESP32 (WEMOS LOLIN32)
  Descripción:
    Al recibir el comando adecuado, enviaremos una imagen obtenida de una dirección de Internet
 */
#include <Arduino.h>
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
      String message = "RoJo Telegram Bot library\n\n";
      message += "/Mickey : foto sin texto\n";
      message += "/Minnie : foto con texto\n";
      message += "/Pluto : foto sin notificación\n";
      message += "/Goofy : foto en referencia a un mensaje\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Si el comando es /Mickey...
    else if(msg.text=="/Mickey")
    {
      //...enviamos una imagen de una URL de Internet

      //Definimosl la URL de la imagen
      String photoURL="https://upload.wikimedia.org/wikipedia/en/d/d4/Mickey_Mouse.png";
      //Enviamos la foto sin texto
      bot.sendPhotoRemote(msg.chat_id,photoURL);
    }
    //Si el comando es /Minnie...
    else if(msg.text=="/Minnie")
    {
      //...enviamos una imagen de una URL de Internet

      //Definimosl la URL de la imagen
      String photoURL="https://upload.wikimedia.org/wikipedia/en/6/67/Minnie_Mouse.png";
      //Enviamos la foto con texto
      bot.sendPhotoRemote(msg.chat_id,photoURL,"Para " + msg.from_name);
    }
    //Si el comando es /Pluto...
    else if(msg.text=="/Pluto")
    {
      //...enviamos una imagen de una URL de Internet

      //Definimosl la URL de la imagen
      String photoURL="https://vignette.wikia.nocookie.net/parody/images/a/ac/DSNY-SM-13.jpg/revision/latest?cb=20140920064657";
      //Enviamos la foto pero no se notificará cuando se reciba
      bot.sendPhotoRemote(msg.chat_id,photoURL,"",true);
    }
    //Si el comando es /Goofy...
    else if(msg.text=="/Goofy")
    {
      //...enviamos una imagen de una URL de Internet

      //Definimosl la URL de la imagen
      String photoURL="https://vignette.wikia.nocookie.net/disney/images/2/27/Goofy_transparent.png/revision/latest?cb=20170426165903";
      //Enviamos la foto como réplica al mensaje recibido
      bot.sendPhotoRemote(msg.chat_id,photoURL,"",false,msg.message_id);
    }

    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    msg=bot.getNextMessage();
  }
}

void setup()
{
  //Inicializamos bot
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
