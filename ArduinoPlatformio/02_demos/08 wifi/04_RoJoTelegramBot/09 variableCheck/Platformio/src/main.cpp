/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Sistema de reducción de consultas an Internet
  Descripción:
    Utilizaremos como base el ejemplo de control remoto de un led
    Hasta ahora hacíamos comprobaciones de nuevos mensajes con un periodo fijo
    Para reducir el número de comprobaciones creamos un nuevo sistema en el que definimos dos
    valores: el tiempo máximo de espera y el factor de incremento.
    El funcionamiento se basa en que cuando se recibe un mensaje de activa el sistema que hace 
    comprobación de nuevos mensajes con mayor frecuencia.
    Si dejamos de recibir mensajes, la frecuencia decrece geométricamente con el factor
    indicado hasta alcanzar un el tiempo máximo.
    A modo informativo, enviamos por el puerto serie el valor del tiempo de espera
    actual en milisegundos.
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
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
RoJoTelegramBot bot; //Objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual

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
    }
    //Reseteamos el tiempo de espera
    currentWait=startWait;
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
  handleNewMessages(); //Procesa todos los nuevos mensajes
  delay(currentWait); //Esperamos
  currentWait*=factorWait; //Aumentamos el tiempo de espera para el siguiente ciclo
  if(currentWait>maxWait) currentWait=maxWait; //Si sobrepasamos el máximo...lo limitamos
  Serial.println(currentWait); //Mostramos el tiempo de espera actual a modo informativo
}