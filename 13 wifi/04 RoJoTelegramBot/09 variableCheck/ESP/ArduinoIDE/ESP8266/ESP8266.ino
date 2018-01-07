/*
  Autor: Ramón Junquera
  Fecha: 20171226
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Sistema de reducción de consultas an Internet
  Material: placa ESP8266
  Descripción:
    Utilizaramos como base el ejemplo de control remoto de un led
    Hasta ahora hacíamos comprobaciones de nuevos mensajes con un periodo fijo
    Habitualmente fijábamos el periodo en un segundo.
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
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;
//Tiempo de espera inicial
uint32_t startWait=1+1/(factorWait-1);
//Tiempo de espera actual
uint32_t currentWait=startWait;

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
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
    //Reseteamos el tiempo de espera
    currentWait=startWait;
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    msg=bot.getNextMessage();
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
  //Procesa todos los nuevos mensajes
  handleNewMessages();
  //Esperamos
  delay(currentWait);
  //Aumentamos el tiempo de espera para el siguiente ciclo
  currentWait*=factorWait;
  //Si sobrepasamos el máximo...lo limitamos
  if(currentWait>maxWait) currentWait=maxWait;
  //Mostramos el tiempo de espera actual a modo informativo
  Serial.println(currentWait);
}
