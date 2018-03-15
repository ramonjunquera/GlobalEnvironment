/*
  Autor: Ramón Junquera
  Fecha: 20180314
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Control remoto a distancia
  Material: placa ESP-12E o Sonoff S20
  Descripción:
    Basado en el ejemplo de suscripción segura.
    Se incluye la definición de contantes del led, interruptor y relay.
    El estado actual del relay se guarda siempre en un archivo.
    Cuando el sistema es reseteado, siempre recupera el último estado que tenía el realy y lo comunica.
    El interruptor permite cambiar localmente el estado del relay. Esta acción también es comunicada
    a todos los suscriptores.
 */

//Si se define la siguiente constante se compilará para Sonoff S20
//Si no se define, se hará para un ESP12-E
//#define SONOFF

#include <Arduino.h>
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include <FS.h> //Librería para gestión de archivos
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes

#ifdef SONOFF
  //Definición de constantes de pin para Sonoff S20
  const byte pinLed=13; //Pin del led verde
  const byte pinRelay=12; //Pin relay y led azul
  const byte pinSwitch=0; //Pin interruptor
#else
  //Definición de constantes de pin para ESP-12E
  //Como interruptor se utiliza el botón FLASH y como relay el segundo led integrado (el de antena)
  const byte pinLed=LED_BUILTIN; //Pin del led LED_BUILTIN=D0=16
  const byte pinRelay=D4; //Pin relay D4=2
  const byte pinSwitch=D3; //Pin interruptor D3=0
#endif


//Definición de variables globales
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
String statusRelayFile="/relaystatus.txt"; //Nombre del archivo que guarda el estado del relé
bool statusRelay; //Estado del relé
bool switchPressed=false; //El interruptor ha sido pulsado?
RoJoTelegramBot bot; //Creamos el objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
uint16_t subscriptionCode=0; //Código de suscripción
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint32_t subscriptionCodeTimeout=0; //Hora de caducidad del código de suscripción
RoJoFileDictionary subscribers; //Creamos objeto de gestión de diccionarios

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

  //Mensaje a responder
  String txt="Suscripción completada";
  //Lo añadimos. Y si ya existía...cambiamos el mensaje
  if(subscribers.add((*msg).chat_id,(*msg).from_name)) txt="Ya estabas suscrito";
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

  TelegramMessage msg=bot.getNextMessage();
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    /*
    //Sección a eliminar. Sólo para pruebas
    //Implementa el comando /nolist para eliminar la lista de suscriptores
    //Comando público!!!!!
    if(msg.text=="/nolist")
    {
      //Eliminamos todos los items del diccionario
      subscribers.clear();
      //Informamos
      bot.sendMessage(msg.chat_id,"Suscriptores borrados");
    }
    */

    //Reseteamos el contador de espera porque se ha recibido un mensaje
    currentWait=startWait;
    //Si no hay suscriptores...
    if(subscribers.count()==0)
    {
      //Si se trata del comando /start...
      if(msg.text=="/start")
      {
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Telegram Bot library\n";
        message += "Secure Switch\n\n";
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
          message += "Secure Switch\n\n";
          message += "/generate : Genera un código\n";
          message += "/subscribe code : Añadirse a la lista\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          message += "/list : Suscriptores\n";
          message += "/on : Encender\n";
          message += "/off : Apagar\n";
          message += "/status : Estado actual\n";
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
          //Si el led ya está encendido...
          if(statusRelay)
          {
            //Informamos al usuario
            bot.sendMessage(msg.chat_id,"Ya estaba encendido");
          }
          else //Si estaba apagado...
          {
            //Cambiamos el estado a apagado
            statusRelay=true;
            //Aplicamos el estado
            setStatus();
            //Informamos a todos
            broadcast(msg.from_name + " ha encendido");
          }
        }
        else if(msg.text=="/off")
        {
          //Si ya estaba apagado...
          if(!statusRelay)
          {
            //Informamos al usuario
            bot.sendMessage(msg.chat_id,"Ya estaba apagado");
          }
          else //Si estaba encendido...
          {
            //Cambiamos el estado a apagado
            statusRelay=false;
            //Aplicamos el estado
            setStatus();
            //Informamos a todos
            broadcast(msg.from_name + " ha apagado");
          }
        }
        else if(msg.text=="/status")
        {
          //Obtenemos el texto del estado
          String statusText = statusRelay?"Encendido":"Apagado";
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
    //Refrescamos los proceos de fondo
    yield();
    //Leemos el siguiente
    msg=bot.getNextMessage();
  }
}

void setStatus()
{
  //Aplica el estado actual (statusRelay) al relé y lo guarda
  
  //Aplicamos estado al relé (estados intercambiados)
  digitalWrite(pinRelay,!statusRelay);
  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusRelayFile,"w");
  //Guardamos el estado del led en el archivo
  f.write(statusRelay);
  //Cerramos el archivo
  f.close();
  //Apagamos el led (estados intercambiados)
  digitalWrite(pinLed,HIGH);
}

void readStatus()
{
  //Lee el estado desde el archivo y lo aplica

  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusRelayFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(statusRelayFile,"w");
    //Se guarda el estado "apagado", por defecto
    f.write(0);
    //Se cierra
    f.close();
    //Se vuelve a abrir como sólo lectura
    f=SPIFFS.open(statusRelayFile,"w");
  }
  //Tenemos el archivo abierto
  
  //Leemos el estado guardado
  //Sólo puede ser 0=off o 1=on
  statusRelay=f.read();
  //Cerramos el archivo
  f.close();
  //Aplicamos el estado leido
  setStatus();
}

void interruptSwitch()
{
  //Función de gestión de interrupciones del interruptor

  //Si no se había anotado la pulsación...
  if(!switchPressed)
  {
    //Lo anotamos ahora
    switchPressed=true;
    //Y para que se de cuenta, encendemos el led (estados intercambiados)
    digitalWrite(pinLed,LOW);
  }
}

void setup()
{
  //Inicializamos el bot
  bot.begin(botToken);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
  //Configuramos el pin del relé como salida
  pinMode(pinRelay,OUTPUT);
  //Configuramos el pin del interruptor como entrada con las resistencias de pullup activas
  pinMode(pinSwitch,INPUT_PULLUP);
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

  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Leemos el estado desde el archivo y lo aplicamos
  readStatus();
  //Informamos del reinicio
  broadcast("Dispositivo reiniciado");
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
  //Activamos las interrupciones para el pin del interruptor sólo para pulsaciones
  attachInterrupt(pinSwitch,interruptSwitch,FALLING);
}

void loop()
{
  //Procesa todos los nuevos mensajes
  handleNewMessages();
  //Calculamos el tiempo máximo de espera
  uint32_t maxTime=millis()+currentWait;
  //Mientras no lleguemos al máximo tiempo de espera...
  while(millis()<maxTime)
  {
    //Si se ha pulsado el interruptor...
    if(switchPressed)
    {
      //Cambiamos el estado actual
      statusRelay=!statusRelay;
      //Aplicamos el estado actual al led y al relé
      setStatus();
      //Informamos
      String s = statusRelay?"encendido":"apagado";
      broadcast("Se ha " + s + " con el interruptor");
      //Reseteamos el contador de espera porque ha habido un cambio
      currentWait=startWait;
      //Quitamos el flag de interruptor pulsado
      switchPressed=false;
    }
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
    //Refrescamos los procesos de fondo
    //Si no lo hacemos, se reseteará periódicamente debido a WDT
    yield();
  }
  //Aumentamos el tiempo de espera para el siguiente ciclo
  currentWait*=factorWait;
  //Si sobrepasamos el máximo...lo limitamos
  if(currentWait>maxWait) currentWait=maxWait;
}
