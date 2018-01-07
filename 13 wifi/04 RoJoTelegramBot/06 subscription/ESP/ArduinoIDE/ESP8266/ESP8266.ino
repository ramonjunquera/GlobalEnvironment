/*
  Autor: Ramón Junquera
  Fecha: 20171226
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de suscripción a mensajes de un bot
  Material: placa ESP8266
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
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include <FS.h> //Librería para gestión de archivos
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
//Nombre del archivo de lista de suscriptores
String listFile="/subscribers.txt";

void subscribe(TelegramMessage *msg)
{
  //Incluye al autor del mensaje del parámetro a la lista de destinatarios

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;

  //Abrimos el archivo de lista de suscriptores como lectura/escritura
  File f=SPIFFS.open(listFile,"r+");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(listFile,"w");
    //Se cierra
    f.close();
    //Se abre en modo correcto
    f=SPIFFS.open(listFile,"r+");
  }
  //Tenemos el archivo abierto

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Si el identificador de chat coincide con el del solicitante...
    if(chat_id==(*msg).chat_id)
    {
      //...informamos
      bot.sendMessage((*msg).chat_id,"Ya estabas suscrito");
      //Hemos terminado con el archivo
      f.close(); 
      //Terminamos
      return;
    }   
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
  }
  //Hemos recorrido el archivo entero y el autor no está en la lista
  //Lo incluimos
  f.println(bot.uint64String((*msg).chat_id) + " " + (*msg).from_name);
  //Hemos terminado con el archivo
  f.close();
  //Informamos
  bot.sendMessage((*msg).chat_id,"Suscripción completada");
}

void unsubscribe(TelegramMessage *msg)
{
  //Elimina un usuario de la lista de destinatarios

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;
  //Hemos encontrado el registro?. Inicialmente no.
  bool found=false;

  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(listFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(listFile,"w");
    //Se cierra
    f.close();
    //Se abre en modo correcto
    f=SPIFFS.open(listFile,"r");
  }
  //Tenemos el archivo abierto

  //Creamos un archivo temporal
  File fTemp=SPIFFS.open("/tmp","w");

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
    //Si coincide con el identificador de chat del autor...
    if(chat_id==(*msg).chat_id)
    {
      //...anotamos que hemos encontrado al autor
      found=true;
    }
    else //No coincide el identificador de chat
    {
      //Pasamos el registro al archivo temporal
      fTemp.println(bot.uint64String(chat_id) + from_name);
    }
  }
  //Hemos terminado de procesar el archivo
  //Cerramos el archivo de suscriptores
  f.close();
  //Cerramos el archivo temporal
  fTemp.close();
  //Si se ha encontrado el autor en la lista de suscriptores...
  if(found)
  {
    //Eliminamos la lista de suscriptores actual
    SPIFFS.remove(listFile);
    //Renombramos el archivo temporal a la lista definitiva
    SPIFFS.rename("/tmp",listFile);
    //Informamos
    bot.sendMessage((*msg).chat_id,"Suscripción eliminada");
  }
  else //El autor no está en la lsita de suscriptores
  {
    //Eliminamos el archivo temporal
    SPIFFS.remove("/tmp");
    //Informamos
    bot.sendMessage((*msg).chat_id,"No estás suscrito");
  }
}

bool chatInList(uint64_t chat_id_author)
{
  //Comprueba si un identificador de chat se encuentra en la lista de suscriptores

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;

  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(listFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(listFile,"w");
    //Se cierra
    f.close();
    //Se abre en modo correcto
    f=SPIFFS.open(listFile,"r");
  }
  //Tenemos el archivo abierto

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
    //Si coincide el identificador de chat...
    if(chat_id==chat_id_author)
    {
      //...encontrado!
      //Cerramos el archivo
      f.close();
      //Respondemos que lo hemos encontrado
      return true;
    }
  }
  //Cerramos el archivo
  f.close();
  //No lo hemos encontrado
  return false;
}

void list(TelegramMessage *msg)
{
  //Envía la lista de suscriptores actuales

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;
  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Si el autor no está en la lista de suscriptores...
  if(!chatInList((*msg).chat_id))
  {
    //...informamos
    bot.sendMessage((*msg).chat_id,"Solo los suscriptores pueden ver la lista");
    //Hemos terminado
    return;
  }
  //El autor está en la lista de suscriptores

  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(listFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(listFile,"w");
    //Se cierra
    f.close();
    //Se abre en modo correcto
    f=SPIFFS.open(listFile,"r");
  }
  //Tenemos el archivo abierto

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
    //Si no es el primero...añadimos una coma
    if(answerList.length()) answerList+=",";
    //Añadimos el nombre
    answerList+=from_name;
  }
  //Cerramos el archivo
  f.close();
  //Informamos
  bot.sendMessage((*msg).chat_id,"Suscriptores:"+answerList);
}

void broadcast(TelegramMessage *msg)
{
  //Enviar el mensaje actual a todos los suscriptores
  //Si el autor del mensaje no es suscriptor, no puede enviar
  //El mensaje será enviado a todos los suscriptores, menos a él mismo

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;

  //Si el autor no está en la lista de suscriptores...
  if(!chatInList((*msg).chat_id))
  {
    //...informamos
    bot.sendMessage((*msg).chat_id,"Solo los suscriptores pueden enviar mensajes");
  }
  else //El autor es suscriptor
  {
    //Enviaremos el mensaje a todos los suscriptores
    //Abrimos el archivo de lista de suscriptores como sólo lectura
    File f=SPIFFS.open(listFile,"r");
    //Mientras el archivo tenga algo que leer...
    while(f.available())
    {
      //...leemos el identificador de chat
      chat_id=f.parseInt();
      //Leemos el nombre del suscriptor
      from_name=f.readStringUntil('\n');
      /*
      //Si no coincide el identificador de chat...
      if(chat_id!=(*msg).chat_id)
      {
        //...enviamos el mensaje
        bot.sendMessage(chat_id,(*msg).from_name + " dijo: " + (*msg).text);
      }
      */
      bot.sendMessage(chat_id,(*msg).from_name + " dijo: " + (*msg).text);
    }
    //Cerramos el archivo
    f.close();
    //No hace falta informar que su mensaje se ha enviado
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
      //Eliminamos el archivo de suscriptores
      SPIFFS.remove(listFile);
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
  
  //Inicializamos el acceso a archivos de SPIFFS
  SPIFFS.begin();
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
