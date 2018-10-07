/*
  Autor: Ramón Junquera
  Fecha: 20181007
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de control de un led por comandos
  Material: placa ESP
  Descripción:
    Basado en el ejemplo de control del led integrado.
    Añadimos varias opciones que muestran un teclado para seleccionar las distintas posibilidades
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
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start")
    {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n\n";
      message += "/on : to switch the led ON\n";
      message += "/off : to switch the led OFF\n";
      message += "/status : returns current led status\n";
      message += "/options1 : normal keyboard\n";
      message += "/options2 : resize keyboard\n";
      message += "/options3 : one time keyboard\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Si es el comando /on...
    else if (msg.text == "/on")
    {
      //...encendemos el led
      digitalWrite(pinLed,LOW);
      //Enviamos mensaje informado
      bot.sendMessage(msg.chat_id,"Led is ON");
    }
    //Si el comando es /off...
    else if(msg.text=="/off")
    {
      //...apagamos el led
      digitalWrite(pinLed,HIGH);
      //Enviamos mensaje informado
      bot.sendMessage(msg.chat_id,"Led is OFF");
    }
    //Si el comando es /status...
    else if(msg.text=="/status")
    {
      //Obtenemos el texto del estado
      String statusText = digitalRead(pinLed)?"OFF":"ON";
      //Enviamos mensaje informado del estado
      bot.sendMessage(msg.chat_id,"Led is "+statusText);
    }
    //Si el comando es /options...
    else if(msg.text=="/options1")
    {
      //Creamos la cadena JSON con las distintas opciones
      //Ejemplo: [["/on","/off"],["/status"]]
      String keyb = "[[\"/on\", \"/off\"],[\"/status\"]]";
      //Enviamos el el mensaje con las opciones de respuesta
      //No incluimos ningún parámetro adicional
      //Los botones aparecen muy grandes porque no se redimensionan
      bot.sendMessage(msg.chat_id,"Choose one option",keyb);
    }
    else if(msg.text=="/options2")
    {
      //Creamos la cadena JSON con las distintas opciones
      //Ejemplo: [["/on","/off"],["/status"]]
      String keyb = "[[\"/on\", \"/off\"],[\"/status\"]]";
      //Enviamos el el mensaje con las opciones de respuesta
      //Añadimos el parámetro de resize
      //Los botones se adaptan al tamaño del texto que contienen
      bot.sendMessage(msg.chat_id,"Choose one option",keyb,true);
    }
    else if(msg.text=="/options3")
    {
      //Creamos la cadena JSON con las distintas opciones
      //Ejemplo: [["/on","/off"],["/status"]]
      String keyb = "[[\"/on\", \"/off\"],[\"/status\"]]";
      //Enviamos el el mensaje con las opciones de respuesta
      //Añadimos el parámetro de resize y one time
      //Los botones sólo aparecen para esta respuesta. Una vez seleccionada una
      //opción, desaparece
      bot.sendMessage(msg.chat_id,"Choose one option",keyb,true,true);
    }
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
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
