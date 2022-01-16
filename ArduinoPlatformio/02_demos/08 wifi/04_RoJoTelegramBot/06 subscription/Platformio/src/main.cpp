/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de suscripción a mensajes de un bot
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
    número de suscriptores, excepto por la capacidad de LittleFS y nuestra paciencia.
    Para un gran número de suscriptores, este no sería el mejor de los métodos.

    Telegram tiene una limitación en el número de mensajes que puede enviar un bot por segundo.
    El máximo es 30 mensajes/segundo.
    Teniendo en cuenta las especificaciones del ESP8266, jamás alcanzamos esta cifra. Por lo tanto, no nos
    preocuparemos de este detalle, al menos con este procesador.
*/
#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi para ESP8266
#elif defined(ESP32)
  #include <WiFi.h> //Librería para gestión de wifi para ESP32
#endif
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <RoJoFileDictionary.h> //Librería de gestión de diccionarios en archivo

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
RoJoFileDictionary subscribers; //Objeto de gestión de diccionarios

//Incluye al autor del mensaje del parámetro a la lista de destinatarios
void subscribe(TelegramMessage *msg) {
  String txt="Suscripción completada"; //Mensaje a responder
  //Lo añadimos. Y si ya existía...cambiamos el mensaje
  if(subscribers.add((*msg).chat_id,(*msg).from_name)) txt="Ya estabas suscrito";
  bot.sendMessage((*msg).chat_id,txt); //Informamos
}

//Elimina un usuario de la lista de destinatarios
void unsubscribe(TelegramMessage *msg) {
  String txt="No estás suscrito"; //Mensaje de respuesta
  //Si el suscriptor existía...cambiamos el mensaje
  if(subscribers.remove((*msg).chat_id)) txt="Suscripción eliminada";
  bot.sendMessage((*msg).chat_id,txt); //Enviamos la respuesta
}

//Envía la lista de suscriptores actuales
void list(TelegramMessage *msg) {
  String answerList=""; //Cadena con al lista. Inicialmente vacía
  //Si el autor no está en la lista de suscriptores...
  if(!subscribers.containsKey((*msg).chat_id)) {
    //...informamos
    bot.sendMessage((*msg).chat_id,"Solo los suscriptores pueden ver la lista");
    return;
  }
  //El autor está en la lista de suscriptores

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++) {
    if(answerList.length()) answerList+=","; //Si no es el primero...añadimos una coma
    answerList+=subscribers.value(i); //Añadimos el nombre
  }
  bot.sendMessage((*msg).chat_id,"Suscriptores:"+answerList); //Informamos
}

//Enviar el mensaje actual a todos los suscriptores
//Si el autor del mensaje no es suscriptor, no puede enviar
//El mensaje será enviado a todos los suscriptores
void broadcast(TelegramMessage *msg) {
  //Si el autor no está en la lista de suscriptores...
  if(!subscribers.containsKey(msg->chat_id)) {
    bot.sendMessage(msg->chat_id,"Solo los suscriptores pueden enviar mensajes");
  } else { //El autor es suscriptor
    //Enviaremos el mensaje a todos los suscriptores
    //Recorremos todos los suscriptores...
    for(uint16_t i=0;i<subscribers.count();i++)
      //...y les enviamos el mensaje
      bot.sendMessage(subscribers.key(i),msg->from_name + " dijo: " + msg->text);
  }
}

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start") {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "Suscripción a un bot\n\n";
      message += "/subscribe : Añadirse a la lista\n";
      message += "/unsubscribe : Eliminarse de la lista\n";
      message += "/list : Mostrar lista\n";
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    } else if(msg.text=="/subscribe") {
      subscribe(&msg); //Incluimos el autor en la lista de destinatarios
    } else if(msg.text=="/unsubscribe") {
      unsubscribe(&msg); //Eliminamos al autor de la lista
    } else if(msg.text=="/list") {
      list(&msg); //Mostramos la lista de usuarios suscritos al que lo ha pedido
    } else if(msg.text=="/nolist") {
      subscribers.clear(); //Eliminamos todos los suscriptores
      bot.sendMessage(msg.chat_id,"Suscriptores borrados"); //Informamos
    } else { //Si no es ningún comando reconocido...
      broadcast(&msg); //...hemos recibido un mensaje que se debe distribuir
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
  subscribers.begin("/subscribers.txt"); //Inicializamos el diccionario de suscriptores
}

void loop() {
  handleNewMessages(); //Procesa todos los mensajes nuevos
  delay(2000); //Esperamos un momento hasta volver a comprobar si hay nuevos mensajes
}