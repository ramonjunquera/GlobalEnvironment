/*
  Autor: Ramón Junquera
  Fecha: 20180314
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de suscripción a mensajes de un bot
  Material: placa ESP
  Descripción:
    El bot envía los mensajes recibidos a todos los suscriptores que tiene anotados.
    El comando /start muestra la lista de comandos.
    El comando /subscribe suscribe al autor del mensaje a la lista de distribución.
    Si ya está suscrito, lo informa.
    El comando /unsuscribe elimina al autor de la lista de distribución.
    Si no estaba suscrito, lo informa.
    El comando /list responde al autor con la lista de suscriptores
    Un usuario no suscrito, no puede enviar mensajes
    Los mensajes nunca son enviador al propio autor.

    La lista de suscriptores se almacena en un archivo de texto.
    Cada línea coresponde con un registro.
    Un registro consta del identificador de chat (valor numérico) + un espacio + el nombre del usuario suscrito.
    Puesto que el nombre no es un campo obligatorio, si detectamos que no esstá informado, lo completamos 
    como "Anónimo". Gracias a esto, siempre podemos enviar los mensajes como: "Usuario dijo: mensaje".
    El análisis del archivo de suscriptores se realiza línea a línea. Por lo tanto, no tenemos límite de
    número de suscriptores, excepto por la capacidad de SPIFFS y nunestra paciencia.
    Para un gran número de suscriptores, este no sería el mejor de los métodos.

    Telegram tiene una limitación en el número de mensajes que puede enviar un bot por segundo.
    El máximo es 30 mensajes/segundo.
    Teniendo en cuenta las especificaciones del ESP8266, jamás alcanzamos esta cifra. POr lo tanto, no nos preocuparemos
    de este detalle, al menos con este procesador.
 */
#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi
#endif
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo

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
//Creamos objeto de gestión de diccionarios
RoJoFileDictionary subscribers;

void subscribe(TelegramMessage *msg)
{
  //Incluye al autor del mensaje del parámetro a la lista de destinatarios

  //Mensaje a responder
  String txt="Suscripción completada";
  //Lo añadimos. Y si ya existía...cambiamos el mensaje
  if(subscribers.add((*msg).chat_id,(*msg).from_name)) txt="Ya estabas suscrito";
  //Informamos
  bot.sendMessage((*msg).chat_id,txt);
}

void unsubscribe(TelegramMessage *msg)
{
  //Elimina un usuario de la lista de destinatarios

  //Mensaje de respuesta
  String txt="No estás suscrito";
  //Si el suscriptor existía...cambiamos el mensaje
  if(subscribers.remove((*msg).chat_id)) txt="Suscripción eliminada";
  //Enviamos la respuesta
  bot.sendMessage((*msg).chat_id,txt);
}

void list(TelegramMessage *msg)
{
  //Envía la lista de suscriptores actuales

  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Si el autor no está en la lista de suscriptores...
  if(!subscribers.containsKey((*msg).chat_id))
  {
    //...informamos
    bot.sendMessage((*msg).chat_id,"Solo los suscriptores pueden ver la lista");
    //Hemos terminado
    return;
  }
  //El autor está en la lista de suscriptores

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
  {
    //Si no es el primero...añadimos una coma
    if(answerList.length()) answerList+=",";
    //Añadimos el nombre
    answerList+=subscribers.value(i);
  }
  //Informamos
  bot.sendMessage((*msg).chat_id,"Suscriptores:"+answerList);
}

void broadcast(TelegramMessage *msg)
{
  //Enviar el mensaje actual a todos los suscriptores
  //Si el autor del mensaje no es suscriptor, no puede enviar
  //El mensaje será enviado a todos los suscriptores

  //Si el autor no está en la lista de suscriptores...
  if(!subscribers.containsKey((*msg).chat_id))
  {
    //...informamos
    bot.sendMessage((*msg).chat_id,"Solo los suscriptores pueden enviar mensajes");
  }
  else //El autor es suscriptor
  {
    //Enviaremos el mensaje a todos los suscriptores
    //Recorremos todos los suscriptores...
    for(uint16_t i=0;i<subscribers.count();i++)
      //...y les enviamos el mensaje
      bot.sendMessage(subscribers.key(i),(*msg).from_name + " dijo: " + (*msg).text);
  }
}

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    //Es obligatorio que todos los mensajes recibidos tengan autor
    //Pero el nombre del autor es optativo
    //Si el mensaje no tiene nombre de autor, le ponemos uno
    if(!msg.from_name.length()) msg.from_name="Anónimo";
    
    //Si el comando es /start...
    if (msg.text == "/start")
    {
      //Componemos el texto a enviar en un sólo mensaje (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Suscripción a un bot\n\n";
      message += "/subscribe : Añadirse a la lista\n";
      message += "/unsubscribe : Eliminarse de la lista\n";
      message += "/list : Mostrar lista\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Analizamos los distintos comandos...
    else if(msg.text=="/subscribe")
    {
      //Incluimos el autor en la lista de destinatarios
      subscribe(&msg);
    }
    else if(msg.text=="/unsubscribe")
    {
      //Eliminamos al autor de la lista
      unsubscribe(&msg);
    }
    else if(msg.text=="/list")
    {
      //Mostramos la lista de usuarios suscritos al que lo ha pedido
      list(&msg);
    }
    else if(msg.text=="/nolist")
    {
      //Eliminamos todos los suscriptores
      subscribers.clear();
      //Informamos
      bot.sendMessage(msg.chat_id,"Suscriptores borrados");
    }
    else //Si no es ningún comando reconocido...
    {
      //...hemos recibido un mensaje que se debe distribuir
      broadcast(&msg);
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
  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
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
