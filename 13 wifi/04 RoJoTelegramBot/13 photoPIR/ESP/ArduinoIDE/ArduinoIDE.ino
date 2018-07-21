/*
  Autor: Ramón Junquera
  Versión: 20180711
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Enviar una imagen al detectar movimiento
  Material: placa ESP, ArduCAM-Mini-2MP, PIR sensor
  Descripción:
    Utilizando los ejemplos anteriores para como base.
    Crearemos un sistema de suscripción segura.
    Usamos el sistema de comprobación a intervalos variables.
    Cuando el sensor detecte movimiento, automáticamente se enviará una foto en la 
    resolución actual a todos los usuarios suscritos.
    Se añade la posibilidad de desactivar manualmente el sensor de movimiento
  Nota:
    Es ESP8266 es posible que haya que desconectar el pin D4 para tranferir el programa. Después se 
    puede volver a conectar al PIR.
*/
#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi
#endif
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoArduCAM.h" //Librería de gestión de la cámara
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
bool enabled = true; //Por defecto, cuando arranca, se enciende
byte currentRes=3; //Resolución actual
#ifdef ESP8266 //Si es una ESP8266...
  const byte pinPIR=D3; //Pin del sensor PIR
  const byte pinCS=D0; //Pin CS de la cámara
#elif defined(ESP32)
  const byte pinPIR=2; //Pin del sensor PIR
  const byte pinCS=4; //Pin CS de la cámara
#endif

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
//Creamos el objeto de gestión de la cámara
RoJoArduCAM camera;
//Flag de deteccción de movimiento. Inicialmente no.
bool movementDetected=false;
//Creamos objeto de gestión de diccionarios
RoJoFileDictionary subscribers;
//Teclado por defecto
const String defaultKeyb="[[\"/on\",\"/off\",\"/status\"],[\"/photo\",\"/users\"],[\"/res\",\"/resX\"]]";
//Nombre del archivo que guarda el estado
const String statusFile="/status.txt"; 

void saveStatus()
{
  //Guarda la configuración actual (resolución y estado)
  //en un archivo en SPIFFS

  //Abrimos el archivo del estado para escritura
  File f=SPIFFS.open(statusFile,"w");
  //Guardamos la resolución actual en los 4 primeros bits y el estado en el bit 5
  byte statusCode=currentRes;
  if(enabled) statusCode+=0b00010000;
  f.write(statusCode);
  //Cerramos el archivo
  f.close();
}

void readStatus()
{
  //Recupera la configuración guardada (resolución y estado) y la aplica

  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //Se guarda la confirguración actual
    saveStatus();
    //Ahora seguro que sí existe el archivo de configuración
    //Se vuelve a abrir como sólo lectura
    f=SPIFFS.open(statusFile,"w");
  }
  //Tenemos el archivo abierto
  
  //Leemos el estado guardado
  byte statusCode=f.read();
  //Cerramos el archivo
  f.close();
  //Extraemos la resolución
  currentRes=statusCode & 0b00001111;
  //Aplicamos la resolución
  camera.setResolution(currentRes);
  //Extraemos el estado
  enabled=statusCode & 0b00010000;
}

void broadcast(String text)
{
  //Enviar el mensaje actual a todos los suscriptores

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text,defaultKeyb,true);
}

void broadcastPhoto(String filename)
{
  //Envia una imagen a todos los suscriptores

  //Identificador de la imagen enviada. Inicialmente no existe
  String file_id="";
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
  {
    //Si no tenemos un identificador de imagen (si aun no hemos enviado la primera imagen)...
    if(file_id.length()==0)
    {
      //Enviamos el mensaje con la imagen y tomamos nota del identificador de la imagen
      //con más resolución
      file_id=bot.sendPhotoLocal(subscribers.key(i),filename);
    }
    else //Ya hemos enviado la imagen...
    {
      //...no volveremos a hacerlo. Ahora sólo enviaremos su identificador
      //Es mucho más rápido
      bot.sendPhotoRemote(subscribers.key(i),file_id);
    }
  }
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
    Serial.println("Recibido mensaje: "+msg.text);
    //Si no hay suscriptores...
    if(subscribers.count()==0)
    {
      Serial.println("Aun no hay suscriptores");
      //Si se trata del comando /start...
      if(msg.text=="/start")
      {
        Serial.println("Reconocido mensaje /start");
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Telegram Bot library\n";
        message += "Photo PIR\n\n";
        message += "/subscribe : Añadirse a la lista\n";
        //Enviamos el mensaje
        bot.sendMessage(msg.chat_id,message);
      }
      else if(msg.text=="/subscribe")
      {
        Serial.println("Reconocido mensaje /subscribe");
        //El primer usuario en suscribirse
        subscriptionCodeGenerator=" él mismo";
        subscribe(&msg);
      }
    }
    else //Hay algún suscriptor
    {
      Serial.println("Hay suscriptores");
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor
      
      Serial.println("Identificador de suscriptor: "+msg.chat_id);
      //Si el autor es suscriptor...
      if(subscribers.containsKey(msg.chat_id))
      {
        Serial.println("Suscriptor reconocido");
        //Podemos hacerle caso
        //Si se trata del comando /start...
        if(msg.text=="/start")
        {
          Serial.println("Reconocido mensaje /start");
          String message = "RoJo Telegram Bot library\n";
          message += "Photo PIR\n\n";
          message += "/on : Activa el detector\n";
          message += "/off : Desactiva el detector\n";
          message += "/status : Estado actual\n";
          message += "/photo : toma foto\n";
          message += "/res : resolución actual\n";
          message += "/resX : resolución [0,8]\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/subscribe code : Añadirse a la lista\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          message += "/list : Mostrar la lista de suscriptores\n";
          String keyb="[[\"/on\",\"/off\"],[\"/photo\",\"/res\",\"/users\"]]";
          //Enviamos el mensaje con menú de selección de un sólo uso
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        }
        else if(msg.text=="/res")
        {
          Serial.println("Reconocido mensaje /res");
          //Mostramos la resolución actual
          String message = "Resolución actual: "+String(currentRes)+"\n";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        }
        else if(msg.text=="/status")
        {
          Serial.println("Reconocido mensaje /status");
          //Mostramos el estado actual
          String message = enabled?"encendido\n":"apagado\n";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        }
        else if(msg.text=="/resX")
        {
          Serial.println("Reconocido mensaje /resX");
          //Sólo presentaremos las opciones y el menú de selección
          String message = "Selecciona resolución\n";
          String keyb="[[\"/res0\",\"/res1\",\"/res2\"],[\"/res3\",\"/res4\",\"/res5\"],[\"/res6\",\"/res7\",\"/res8\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        }
        else if(msg.text=="/users")
        {
          Serial.println("Reconocido mensaje /users");
          //Sólo presentaremos las opciones y el menú de selección
          String message = "RoJo Telegram Bot library\n";
          message += "Photo PIR\n\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/list : Mostrar la lista de suscriptores\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          String keyb="[[\"/generate\",\"/list\",\"/unsubscribe\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        }
        else if(msg.text=="/generate")
        {
          Serial.println("Reconocido mensaje /generate");
          //Genera un código de suscripción que será válido durante el próximo minuto
          subscriptionCode=random(65535)+1; //Entre 1 y 65535
          //Anotamos el nombre del generador
          subscriptionCodeGenerator=msg.from_name;
          //Anotamos la hora en la que caduca. Dentro de 60 segundos
          subscriptionCodeTimeout=millis()+60000;
          //Informamos del código
          bot.sendMessage(msg.chat_id,"Código: " + String(subscriptionCode),defaultKeyb,true);
        }
        else if(msg.text=="/unsubscribe")
        {
          Serial.println("Reconocido mensaje /unsubscribe");
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        }
        else if(msg.text=="/list")
        {
          Serial.println("Reconocido mensaje /list");
          //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores:"+list(),defaultKeyb,true);
        }
        else if(msg.text=="/photo")
        {
          Serial.println("Reconocido mensaje /photo");
          //...solicitamos que se haga la foto...
          camera.takePhoto();
          //...enviamos un mensaje de acción de "escribiendo"
          bot.sendChatAction(msg.chat_id,0);
          //Resolución de la imagen guardada
          uint32_t resX,resY;
          //...descargamos la foto a local...
          //Como nombre de archivo sólo el nombre. Sin barra inicial
          //La extensión la pone él
          byte errorCode=camera.savePhoto("photo",&resX,&resY);
          //Si hubo algún error...
          if(errorCode)
          {
            //...informamos
            Serial.println("Fallo al enviar. Error " + String(errorCode));
            bot.sendMessage(msg.chat_id,"Fallo al enviar. Error " + String(errorCode));
          }
          else //No hubo errores
          {
            //...enviamos un mensaje de acción de "enviando foto"
            bot.sendChatAction(msg.chat_id,1);
            //...enviamos la foto por Telegram
            bot.sendPhotoLocal(msg.chat_id,"/photo.jpg");
          }
        }
        else if(msg.text=="/on")
        {
          Serial.println("Reconocido mensaje /on");
          //Se activa el detector de movimento
          enabled=true;
          //Guardamos configuración actual
          saveStatus();
          //Informamos
          broadcast(msg.from_name + " ha activado el detector");
        }
        else if(msg.text=="/off")
        {
          Serial.println("Reconocido mensaje /off");
          //Se desactiva el detector de movimento
          enabled=false;
          //Guardamos configuración actual
          saveStatus();
          //Informamos
          broadcast(msg.from_name + " ha desactivado el detector");
        }
        //Si el comando tiene una longitud de 5...
        else if(msg.text.length()==5)
        {
          //...si el comando comienza por /res...
          if(msg.text.substring(0,4)=="/res")
          {
            //...si el último carácter es numérico...
            String n=msg.text.substring(4);
            if(n>="0" && n<="8")
            {
              Serial.println("Reconocido mensaje /resX");
              //...obtenemos el valor del código de la resolución
              currentRes=n[0]-48;
              //Aplicamos la resolución
              camera.setResolution(currentRes);
              //Guardamos la configuración actual
              saveStatus();
              //Informamos a todos los suscriptores del cambio de resolución
              broadcast(msg.from_name + " cambia resolución a " + n);
            }
          }
        }
        else //Si no es ningún comando reconocido...
        {
          Serial.println("Mensaje no reconocido");
          //...enviamos el mensaje a todos los suscriptores
          broadcast(msg.from_name + " dijo: " + msg.text);
        }
      }
      else //Al autor no es suscriptor
      {
        Serial.println("Suscriptor no reconocido");
        if(msg.text.substring(0,11)=="/subscribe ")
        {
          Serial.println("Reconocido mensaje /subscribe");
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
    
    //Reseteamos el tiempo de espera
    currentWait=startWait;
    //Refrescamos los procesos de fondo
    yield();
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    msg=bot.getNextMessage();
  }
}

void interruptPIR()
{
  //Función de gestión de interrupciones del PIR

  //Se ha detectado movimiento
  //Si el detector está activo...activamos el flag
  if(enabled) movementDetected=true;
}

void setup()
{
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  //Inicializamos el bot
  bot.begin(botToken);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
  //Nos aseguramos que el led esté apagado
  digitalWrite(pinLed,HIGH);
  //Fijamos el modo de conexión a un punto de acceso
  WiFi.mode(WIFI_STA);
  //Forzamos a su desconexión (por si estaba conectado anteriormente)
  WiFi.disconnect();

  //Inicializamos la cámara con el pin CS
  Serial.print("\nInicializando cámara...");
  byte errorCode=camera.begin(pinCS);
  //Si hubo algún error...
  if(errorCode)
  {
    Serial.println("Error "+String(errorCode));
    //Hacemos parpadear el led tantas veces como el código de error
    for(byte i=0;i<errorCode*2;i++)
    {
      digitalWrite(pinLed,!digitalRead(pinLed));
      delay(300);
      yield();
    }
    //Reiniciamos
    //Atención!. La función restart se queda colgada la primera vez que se ejecuta después de
    //transferir el programa. Para evitarlo se debe reiniciar manualmente una vez.
    ESP.restart();
  }
  //Hemos inicializado la cámara correctamente
  //Por defecto arranca en modo jpg con la resolución 2 = 320x240
  Serial.println("Ok");
  
  //Inicializamos WiFi
  //Pedimos conexión al punto de acceso
  WiFi.begin(ssid,password);
  Serial.print("Conectando");
  //Mientras no estemos conectados...
  while (WiFi.status() != WL_CONNECTED)
  {
    //Cambiaremos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    Serial.print(".");
    yield();
    delay(100);
  }
  //Hemos conseguido conectar
  Serial.println("Ok");

  //Nos aseguramos que el led esté apagado
  digitalWrite(pinLed,HIGH);
  //Configuramos el pin del PIR como entrada
  pinMode(pinPIR,INPUT);
  //Activamos las interrupciones para el pin del PIR
  attachInterrupt(pinPIR,interruptPIR,RISING);
  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
  //Leemos la última configuración guardada
  readStatus();
  //Informamos del reinicio
  broadcast("Dispositivo reiniciado");
  Serial.println("Inicio correcto");
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
    //Si se ha detectado movimiento...
    if(movementDetected)
    {
      //Debemos tomar una foto y enviarla a todos los suscriptores
      
      //Tomamos la foto
      camera.takePhoto();
      //Esperamos hasta que la foto se haya tomado
      while(!camera.photoIsReady()) { delay(100); }
      //Resolución de la imagen guardada
      uint32_t resX,resY;
      //Descargamos la foto a local...
      //Como nombre de archivo sólo el nombre. Sin barra inicial
      //La extensión la pone él
      byte errorCode=camera.savePhoto("photo",&resX,&resY);
      //Si tenemos algún error...
      if(errorCode)
      {
        //...informamos a todos
        broadcast("Fallo guardar imagen. Error " + String(errorCode));
      }
      else //No hay errores
      {
        //Tenemos la imagen guardada en local
        //la enviamos a todos
        broadcastPhoto("/photo.jpg");
      }
      //Desactivamos el flag
      movementDetected=false;
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
