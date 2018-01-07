/*
  Autor: Ramón Junquera
  Fecha: 20171229
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Sistema de suscripción por invitación
  Material: placa ESP32 (WEMOS LOLIN32)
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
#include <SPIFFS.h> //Librería para gestión de archivos
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en milisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
String suscribersFile="/subscribers.txt"; //Nombre del archivo que guarda los suscriptores
byte suscribersCount=0; //Número de suscriptores. Inicialmente ninguno

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
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg=bot.getNextMessage();
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    if(msg.text=="/nolist")
    {
      //Eliminamos el archivo de suscriptores
      SPIFFS.remove(suscribersFile);
      //Ya no hay suscriptores
      suscribersCount=0;
      //Informamos
      bot.sendMessage(msg.chat_id,"Suscriptores borrados");
    }
    //Si no hay suscriptores...
    if(!suscribersCount)
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
      if(chatInList(msg.chat_id))
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

  //Obtenemos el número de suscriptores
  suscribersCount=getSuscribersCount();

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
