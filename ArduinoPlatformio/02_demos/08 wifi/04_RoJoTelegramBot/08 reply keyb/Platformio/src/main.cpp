/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de control de un led por comandos
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
      String message = "RoJo Telegram Bot library\n\n";
      message += "/on : to switch the led ON\n";
      message += "/off : to switch the led OFF\n";
      message += "/status : returns current led status\n";
      message += "/options1 : normal keyboard\n";
      message += "/options2 : resize keyboard\n";
      message += "/options3 : one time keyboard\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/on") {
      
      digitalWrite(pinLed,LOW); //...encendemos el led
      bot.sendMessage(msg.chat_id,"Led is ON"); //Enviamos mensaje informado
    } else if(msg.text=="/off") {
      digitalWrite(pinLed,HIGH); //...apagamos el led
      bot.sendMessage(msg.chat_id,"Led is OFF"); //Enviamos mensaje informado
    } else if(msg.text=="/status") {
      String statusText = digitalRead(pinLed)?"OFF":"ON"; //Obtenemos el texto del estado
      bot.sendMessage(msg.chat_id,"Led is "+statusText); //Enviamos mensaje informado del estado
    } else if(msg.text=="/options1") {
      //Creamos la cadena JSON con las distintas opciones
      //Ejemplo: [["/on","/off"],["/status"]]
      String keyb = "[[\"/on\", \"/off\"],[\"/status\"]]";
      //Enviamos el el mensaje con las opciones de respuesta
      //No incluimos ningún parámetro adicional
      //Los botones aparecen muy grandes porque no se redimensionan
      bot.sendMessage(msg.chat_id,"Choose one option",keyb);
    } else if(msg.text=="/options2") {
      //Creamos la cadena JSON con las distintas opciones
      //Ejemplo: [["/on","/off"],["/status"]]
      String keyb = "[[\"/on\", \"/off\"],[\"/status\"]]";
      //Enviamos el el mensaje con las opciones de respuesta
      //Añadimos el parámetro de resize
      //Los botones se adaptan al tamaño del texto que contienen
      bot.sendMessage(msg.chat_id,"Choose one option",keyb,true);
    } else if(msg.text=="/options3") {
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