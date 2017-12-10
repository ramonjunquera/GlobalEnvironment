/*
  Autor: Ramón Junquera
  Fecha: 20171205
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Control remoto a distancia
  Material: placa Sonoff S20
  Descripción:
    Basado en el ejemplo de suscripción segura.
    Se incluye la definición de contantes del led, interruptor y relay.
    El estado actual del relay se guarda siempre en un archivo.
    Cuando el sistema es reseteado, siempre recupera el último estado que tenía el realy y lo comunica.
    El interruptor permite cambiar localmente el estado del relay. Esta acción también es comunicada
    a todos los suscriptores.
 */
#include <Arduino.h>
#include <ESP8266WiFi.h> //Librería para gestión de wifi
#include <FS.h> //Librería para gestión de archivos
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes

//Definición de constantes de pin para Sonoff S20
const byte pinLed=13; //Pin del led verde
const byte pinRelay=12; //Pin relay y led azul
const byte pinSwitch=0; //Pin interruptor

//Definición de variables globales
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
String suscribersFile="/subscribers.txt"; //Nombre del archivo que guarda los suscriptores
byte suscribersCount=0; //Número de suscriptores. Inicialmente ninguno
String statusRelayFile="/relaystatus.txt"; //Nombre del archivo que guarda el estado del relé
bool statusRelay; //Estado del relé
bool switchPressed=false; //El interruptor ha sido pulsado?
RoJoTelegramBot bot(botToken); //Creamos el objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
uint16_t subscriptionCode=0; //Código de suscripción
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint32_t subscriptionCodeTimeout=0; //Hora de caducidad del código de suscripción

bool chatInList(uint64_t chat_id)
{
  //Comprueba si un identificador de chat se encuentra en la lista de suscriptores

  //Identificador de chat
  uint64_t chat_id_file;
  //Nombre del suscriptor
  String from_name_file;

  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(suscribersFile,"r");
  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id_file=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name_file=f.readStringUntil('\n');
    //Si coincide el identificador de chat...
    if(chat_id_file==chat_id)
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

void broadcast(String text)
{
  //Enviar el mensaje actual a todos los suscriptores

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;

  //Enviaremos el mensaje a todos los suscriptores
  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(suscribersFile,"r");
  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
    //Enviamos el mensaje
    bot.sendMessage(chat_id,text);
  }
  //Cerramos el archivo
  f.close();
}

void subscribe(TelegramMessage *msg)
{
  //Incluye al autor del mensaje del parámetro en la lista de suscriptores

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;

  //Abrimos el archivo de lista de suscriptores como lectura/escritura
  File f=SPIFFS.open(suscribersFile,"r+");

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
  //Incrementamos el número actual de suscriptores
  suscribersCount++;
  //Informamos a todos
  broadcast((*msg).from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator);
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
  File f=SPIFFS.open(suscribersFile,"r");

  //Creamos un archivo temporal
  File fTemp=SPIFFS.open("/tmp","w");

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //...leemos el identificador de chat
    chat_id=f.parseInt();
    //Leemos el nombre del suscriptor
    from_name=f.readStringUntil('\n');
    //Si no coincide con el identificador de chat del autor...
    if(chat_id!=(*msg).chat_id)
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
  //Eliminamos la lista de suscriptores actual
  SPIFFS.remove(suscribersFile);
  //Renombramos el archivo temporal a la lista definitiva
  SPIFFS.rename("/tmp",suscribersFile);
  //Reducimos el número de suscriptores actual;
  suscribersCount--;
  //Informamos a todos
  broadcast((*msg).from_name + " se ha dado de baja");
}

String list()
{
  //Envía la lista de suscriptores actuales

  //Identificador de chat
  uint64_t chat_id;
  //Nombre del suscriptor
  String from_name;
  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Abrimos el archivo de lista de suscriptores como sólo lectura
  File f=SPIFFS.open(suscribersFile,"r");
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
      //Eliminamos el archivo de suscriptores
      SPIFFS.remove(suscribersFile);
      //Ya no hay suscriptores
      suscribersCount=0;
      //Informamos
      bot.sendMessage(msg.chat_id,"Suscriptores borrados");
    }
    */

    //Reseteamos el contador de espera porque se ha recibido un mensaje
    currentWait=startWait;
    //Si no hay suscriptores...
    if(!suscribersCount)
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
      if(chatInList(msg.chat_id))
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

byte getSuscribersCount()
{
  //Obtiene el número de suscriptores actuales
  //Coincide con el número de líneas del archivo de suscriptores

  //Respuesta. Número de suscriptores. Incialmente ninguno
  byte answer=0;
  //Abrimos el archivo de suscriptores como sólo lectura
  File f=SPIFFS.open(suscribersFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //..se crea
    f=SPIFFS.open(suscribersFile,"w");
    //Se cierra
    f.close();
    //Indicamos que no hay suscriptores
    return 0;
  }
  //Tenemos el archivo abierto

  //Mientras el archivo tenga algo que leer...
  while(f.available())
  {
    //Leemos la línea completa y no la guardamos
    f.readStringUntil('\n');
    //Aumentamos el contador de suscriptores
    answer++;
  }
  //Cerramos el archivo
  f.close();
  //Devolvemos el número de suscriptores/líneas
  return answer;
}

void setStatus()
{
  //Aplica el estado actual (statusRelay) al relé y lo guarda
  
  //Aplicamos estado al relé
  digitalWrite(pinRelay,statusRelay);
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

  //Obtenemos el número de suscriptores
  suscribersCount=getSuscribersCount();
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
