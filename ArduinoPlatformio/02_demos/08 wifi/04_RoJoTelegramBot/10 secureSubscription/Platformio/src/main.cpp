/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Sistema de suscripción por invitación
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
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
RoJoTelegramBot bot; //Objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
uint16_t subscriptionCode=0; //Código de suscripción
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint32_t subscriptionCodeTimeout=0; //Hora de caducidad del código de suscripción
RoJoFileDictionary subscribers; //Creamos objeto de gestión de diccionarios

//Enviar el mensaje actual a todos los suscriptores
void broadcast(String text) {
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text);
}

//Incluye al autor del mensaje del parámetro a la lista de suscriptores
void subscribe(TelegramMessage *msg) {
  //Lo añadimos
  subscribers.add(msg->chat_id,msg->from_name);
  //Informamos a todos
  broadcast(msg->from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator);
}

//Elimina un usuario de la lista de destinatarios
void unsubscribe(TelegramMessage *msg) {
  //Eliminamos el item del diccionario
  subscribers.remove(msg->chat_id);
  //Informamos a todos
  broadcast(msg->from_name + " se ha dado de baja");
}

//Devuelve la lista de suscriptores actuales
String list() {
  String answerList=""; //Cadena con la lista. Inicialmente vacía
  for(uint16_t i=0;i<subscribers.count();i++) { //Recorremos todos los suscriptores...
    if(answerList.length()) answerList+=","; //Si no es el primero...añadimos una coma
    answerList+=subscribers.value(i); //Añadimos el nombre
  }
  return answerList; //Devolvemos la lista
}

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya un mensaje...
    Serial.println("Comando recibido: "+msg.text);
    if(msg.text=="/nolist") {
      subscribers.clear(); //Eliminamos todos los items del diccionario
      bot.sendMessage(msg.chat_id,"Suscriptores borrados"); //Informamos
    }
    if(subscribers.count()==0) { //Si no hay suscriptores...
      if(msg.text=="/start") { //Si se trata del comando /start...
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Telegram Bot library\n";
        message += "Secure Subscription\n\n";
        message += "/subscribe : Añadirse a la lista\n";
        //Enviamos el mensaje
        bot.sendMessage(msg.chat_id,message);
      } else if(msg.text=="/subscribe") {
        //El primer usuario en suscribirse
        subscriptionCodeGenerator=" él mismo";
        subscribe(&msg);
      }
    } else { //Hay algún suscriptor
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor
      if(subscribers.containsKey(msg.chat_id)) { //Si el autor es suscriptor...
        //Podemos hacerle caso
        if(msg.text=="/start") {
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
        } else if(msg.text=="/generate") {
          //Genera un código de suscripción que será válido durante el próximo minuto
          subscriptionCode=random(65535)+1; //Entre 1 y 65535
          //Anotamos el nombre del generador
          subscriptionCodeGenerator=msg.from_name;
          //Anotamos la hora en la que caduca. Dentro de 60 segundos
          subscriptionCodeTimeout=millis()+60000;
          //Informamos del código
          bot.sendMessage(msg.chat_id,"Código: "+String(subscriptionCode));
        } else if(msg.text=="/unsubscribe") {
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        } else if(msg.text=="/list") {
          //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores:"+list());
        } else if(msg.text=="/on") {
          //...encendemos el led
          digitalWrite(pinLed,LOW);
          //Informamos a todos
          broadcast(msg.from_name + " ha encendido");
        } else if(msg.text=="/off") {
          //...apagamos el led
          digitalWrite(pinLed,HIGH);
          //Informamos a todos
          broadcast(msg.from_name + " ha apagado");
        } else if(msg.text=="/status") {
          //Obtenemos el texto del estado
          String statusText = digitalRead(pinLed)?"Apagado":"Encendido";
          //Enviamos mensaje informado del estado
          bot.sendMessage(msg.chat_id,statusText);
        } else { //Si no es ningún comando reconocido...
          //...enviamos el mensaje a todos los suscriptores
          broadcast(msg.from_name + " dijo: " + msg.text);
        }
      } else { //El autor no es suscriptor
        if(msg.text.substring(0,11)=="/subscribe ") { //Si parece que alguien quiere suscribirse...
          if(subscriptionCode) { //Si tenemos algún código válido...
            //Si el código indicado en el correcto...
            if(msg.text.substring(11)==String(subscriptionCode)) {
              subscribe(&msg); //Incluimos el nuevo usuario como suscriptor
            }
            subscriptionCode=0; //Desactivamos el código. Ya ha sido utilizado
          } else { //El código indicado no es el correcto
            bot.sendMessage(msg.chat_id,"Código incorrecto"); //Informamos
          }
        }
      }
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
  subscribers.begin("/subscribers.txt"); //Inicializamos el diccionario de suscriptores
  randomSeed(millis()); //Inicializamos la semilla de números aleatorios
  byte errorCode=bot.webhookCheckTelegram(); //Solicitamos estado actual de webhook
  if(errorCode==1) bot.webhookDisableTelegram(); //Si está activo...lo desactivamos.
  if(errorCode!=2) digitalWrite(pinLed,HIGH); //Si no hubo errores...Apagamos el led
  Serial.println("\nRunning");
}

void loop() {
  handleNewMessages(); //Procesa todos los nuevos mensajes
  delay(currentWait); //Esperamos
  currentWait*=factorWait; //Aumentamos el tiempo de espera para el siguiente ciclo
  if(currentWait>maxWait) currentWait=maxWait; //Si sobrepasamos el máximo...lo limitamos

  if(subscriptionCode) { //Si tenemos un código de suscripción válido...
    if(millis()>subscriptionCodeTimeout) { //Si ha caducado...
      subscriptionCode=0; //...lo eliminamos
    }
  }
}