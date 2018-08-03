/*
  Autor: Ramón Junquera
  Fecha: 20180315
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Sistema de suscripción por invitación
  Material: placa ESP
  Descripción:
    Basado en el ejemplo de suscripción.
    Se ha incluido el sistema de comprobación de nuevos mensajes con tiempo variable.

    En el ejemplo de suscripción, cualquiera podía suscribirse a un chat con el bot.
    Depuraremos el sistema para que no sea público.

    Cuando no existe el archivo de suscriptores, o está vacío, se permite una suscripción pública.
    Sólo tiene que escribir alguien el comando /subscribe y será el primer suscriptor.
    A partir de ese instante, las suscripciones no serán públicas.
    Cuando alguien quiera suscribirse, un usuario suscrito deberá generar un código con el
    comando /generate. Es un número aleatorio entre 1 y 65535.
    Se podrá utilizar este código para suscribirse antes de que transcurra un minuto.
    Se debe utilizar el comando /subscribe code
    Si se introduce un código incorrecto, el código quedará anulado y tendrá que generarse uno de nuevo.
    Si se introduce el código correcto, el usuario pasa a ser suscriptor y se informa a todos los
    suscriptores que hay uno nuevo y por quién ha sido invitado (quién generó el código utilizado).
    A un usuario no suscrito no se le devuelven mensajes.

    Para eliminar una suscripción utilizamos el comando /unsubscribe.
    Si el usuario eliminado era el último, se pasará a modo público.

    Los suscriptores pueden controlar el led con los comandos /on, /off y /status.
    Si un suscriptor envía cualquier texto que no es un comando, se enviará a todos los suscriptores.
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
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;
//Tiempo de espera inicial
uint32_t startWait=1+1/(factorWait-1);
//Tiempo de espera actual
uint32_t currentWait=startWait;
//Código de suscripción
uint16_t subscriptionCode=0;
//Nombre del generador del código de suscripción
String subscriptionCodeGenerator="";
//Hora de caducidad del código de suscripción
uint32_t subscriptionCodeTimeout=0;
//Creamos objeto de gestión de diccionarios
RoJoFileDictionary subscribers;

void broadcast(String text)
{
  //Enviar el mensaje actual a todos los suscriptores

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text);
}

void subscribe(TelegramMessage *msg)
{
  //Incluye al autor del mensaje del parámetro a la lista de suscriptores

  //Lo añadimos
  subscribers.add((*msg).chat_id,(*msg).from_name))
  //Informamos a todos
  broadcast((*msg).from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator);
}

void unsubscribe(TelegramMessage *msg)
{
  //Elimina un usuario de la lista de destinatarios

  //Mensaje de respuesta
  String txt="No estás suscrito";
  //Eliminamos el item del diccionario
  subscribers.remove((*msg).chat_id);
  //Informamos a todos
  broadcast((*msg).from_name + " se ha dado de baja");
}

String list()
{
  //Devuelve la lista de suscriptores actuales

  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
  {
    //Si no es el primero...añadimos una coma
    if(answerList.length()) answerList+=",";
    //Añadimos el nombre
    answerList+=subscribers.value(i);
  }
  //Devolvemos la lista
  return answerList;
}

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    if(msg.text=="/nolist")
    {
      //Eliminamos todos los items del diccionario
      subscribers.clear();
      //Informamos
      bot.sendMessage(msg.chat_id,"Suscriptores borrados");
    }
    //Si no hay suscriptores...
    if(subscribers.count()==0)
    {
      //Si se trata del comando /start...
      if(msg.text=="/start")
      {
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Telegram Bot library\n";
        message += "Secure Subscription\n\n";
        message += "/subscribe : Añadirse a la lista\n";
        //Enviamos el mensaje
        bot.sendMessage(msg.chat_id,message);
      }
      else if(msg.text=="/subscribe")
      {
        //El primer usuario en suscribirse
        subscriptionCodeGenerator=" él mismo";
        subscribe(&msg);
      }
    }
    else //Hay algún suscriptor
    {
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor

      //Si el autor es suscriptor...
      if(subscribers.containsKey(msg.chat_id))
      {
        //Podemos hacerle caso
        //Si se trata del comando /start...
        if(msg.text=="/start")
        {
          //Componemos el mensaje a enviar en una sola cadena (es más rápido)
          String message = "RoJo Telegram Bot library\n";
          message += "Secure Subscription\n\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/subscribe code : Añadirse a la lista\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          message += "/list : Mostrar la lista de suscriptores\n";
          message += "/on : Encender\n";
          message += "/off : Apagar\n";
          message += "/status : Mostrar estado actual\n";
          //Enviamos el mensaje
          bot.sendMessage(msg.chat_id,message);
        }
        else if(msg.text=="/generate")
        {
          //Genera un código de suscripción que será válido durante el próximo minuto
          subscriptionCode=random(65535)+1; //Entre 1 y 65535
          //Anotamos el nombre del generador
          subscriptionCodeGenerator=msg.from_name;
          //Anotamos la hora en la que caduca. Dentro de 60 segundos
          subscriptionCodeTimeout=millis()+60000;
          //Informamos del código
          bot.sendMessage(msg.chat_id,"Código: " + String(subscriptionCode));
        }
        else if(msg.text=="/unsubscribe")
        {
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        }
        else if(msg.text=="/list")
        {
          //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores:"+list());
        }
        else if(msg.text=="/on")
        {
          //...encendemos el led
          digitalWrite(pinLed,LOW);
          //Informamos a todos
          broadcast(msg.from_name + " ha encendido");
        }
        else if(msg.text=="/off")
        {
          //...apagamos el led
          digitalWrite(pinLed,HIGH);
          //Informamos a todos
          broadcast(msg.from_name + " ha apagado");
        }
        else if(msg.text=="/status")
        {
          //Obtenemos el texto del estado
          String statusText = digitalRead(pinLed)?"Apagado":"Encendido";
          //Enviamos mensaje informado del estado
          bot.sendMessage(msg.chat_id,statusText);
        }
        else //Si no es ningún comando reconocido...
        {
          //...enviamos el mensaje a todos los suscriptores
          broadcast(msg.from_name + " dijo: " + msg.text);
        }
      }
      else //Al autor no es suscriptor
      {
        if(msg.text.substring(0,11)=="/subscribe ")
        {
          //Si parece que alguien quiere suscribirse...
          //Si tenemos algún código válido...
          if(subscriptionCode)
          {
            //Si el código indicado en el correcto...
            if(msg.text.substring(11)==String(subscriptionCode))
            {
              //Incluimos el nuevo usuario como suscriptor
              subscribe(&msg);
            }
            //Desactivamos el código. Ya ha sido utilizado
            subscriptionCode=0;
          }
          else //El código indicado no es el correcto
          {
            //Informamos
            bot.sendMessage(msg.chat_id,"Código incorrecto");
          }
        }
      }
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
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
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
  //Si tenemos un código de suscripción válido...
  if(subscriptionCode)
  {
    //Si ha caducado...
    if(millis()>subscriptionCodeTimeout)
    {
      //...lo eliminamos
      subscriptionCode=0;
    }
  }
}
