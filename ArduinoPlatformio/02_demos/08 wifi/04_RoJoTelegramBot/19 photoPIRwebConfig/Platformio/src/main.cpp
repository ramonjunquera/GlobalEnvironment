/*
  Autor: Ramón Junquera
  Versión: 20220114
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Configuración basada en interface web
  Descripción:
    Basado en PhotoPIR.
    La diferencia es que la configuración se puede realizar a través de un servidor web
    incluido en el dispositivo.
    Los parámetros de configuración se guardan en un diccionario.
*/

#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <LittleFS.h> //Gestión de LittleFS
#elif defined(ESP32)
  #include <RoJoLittleFS32.h> //Gestión de LittleFS
#endif
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <RoJoArduCAM.h> //Librería de gestión de la cámara
#include <RoJoFileDictionary.h> //Librería de gestión de diccionarios en archivo

//Definición de constantes globales
const char *localSSID="RoJoPhotoPIR"; //Nombre del punto de acceso (SSID) local
const char *localPassword="12345678"; //Contraseña del punto de acceso local
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const bool ledON=LOW; //Estado del pin del led cuando está encendido
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
bool enabled = false; //Por defecto, cuando arranca, está desactivado
byte currentRes=3; //Resolución por defecto
#ifdef ESP8266 //Si es una ESP8266...
  const byte pinPIR=D3; //Pin del sensor PIR
  const byte pinCS=D0; //Pin CS de la cámara
#elif defined(ESP32)
  const byte pinPIR=15; //Pin del sensor PIR
  const byte pinCS=4; //Pin CS de la cámara
#endif
//Teclado por defecto
const String defaultKeyb="[[\"/on\",\"/off\"],[\"/photo\",\"/resX\"],[\"/info\",\"/users\"]]";

//Definición de variables globales
RoJoTelegramBot bot; //Creamos el objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
uint16_t subscriptionCode=0; //Código de suscripción
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint32_t subscriptionCodeTimeout=0; //Hora de caducidad del código de suscripción
RoJoArduCAM camera; //Creamos el objeto de gestión de la cámara
bool movementDetected=false; //Flag de deteccción de movimiento. Inicialmente no.
RoJoFileDictionary subscribers; //Diccionario de suscriptores
RoJoFileDictionary config; //Diccionario de parámetros de configuración
//Creamos un servidor web en el puerto 80 para cambiar la configuración
//La ip del servidor es fija en el punto de acceso propio: 192.168.4.1
//La URL para cambiar la dirección será: http://192.168.4.1
WiFiServer server(80);
bool connecting=false; //No estamos conectando
uint32_t timeoutConnecting=0; //Tiempo límite de intento de conexión
String errorMessageWiFi=""; //Error de WiFi
String errorMessageBot=""; //Error de bot Telegram
uint32_t timeoutCheckingBot=0; //Tiempo límite para comprobar de nuevo mensajes Telegram

//Enviar el mensaje actual a todos los suscriptores
void broadcast(String text) {
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text,defaultKeyb,true);
}

//Envia una imagen a todos los suscriptores
void broadcastPhoto(String filename) {
  String file_id=""; //Identificador de la imagen enviada. Inicialmente no existe
  for(uint16_t i=0;i<subscribers.count();i++) { //Recorremos todos los suscriptores...
    //Si no tenemos un identificador de imagen (si aun no hemos enviado la primera imagen)...
    if(file_id.length()==0) {
      //Enviamos el mensaje con la imagen y tomamos nota del identificador de la imagen
      //con más resolución
      file_id=bot.sendPhotoLocal(subscribers.key(i),filename);
      if(!file_id.length()) { //Si no tenemos un file_id...
        //...es que no se ha enviado correctamente
        //Finalizamos el bucle
        i=subscribers.count();
        //Informamos a todos
        broadcast("Fallo al enviar foto");
      }
      //Parece que la foto se ha enviado correctamente
      else Serial.println("DEBUG. broadcastPhoto. Primera foto enviada correctamente");
    }
    else { //Ya hemos enviado la imagen...
      //...no volveremos a hacerlo. Ahora sólo enviaremos su identificador
      //Es mucho más rápido
      bot.sendPhotoRemote(subscribers.key(i),file_id);
    }
  }
}

//Incluye al autor del mensaje del parámetro en la lista de suscriptores
void subscribe(TelegramMessage *msg) {
  //Si ya existía se actualiza el nombre
  subscribers.add((*msg).chat_id,(*msg).from_name);
  //Informamos a todos
  broadcast((*msg).from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator);
}

//Elimina un usuario de la lista de destinatarios
void unsubscribe(TelegramMessage *msg) {
  //Eliminamos el item del diccionario
  subscribers.remove((*msg).chat_id);
  //Informamos a todos
  broadcast((*msg).from_name + " se ha dado de baja");
}

//Devuelve la lista de suscriptores actuales
String list() {
  //Cadena con al lista. Inicialmente vacía
  String answerList="";

  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++) {
    //Si no es el primero...añadimos una coma
    if(answerList.length()) answerList+=",";
    //Añadimos el nombre
    answerList+=subscribers.value(i);
  }
  //Devolvemos la lista
  return answerList;
}

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos un nuevo mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya mensaje...
    Serial.println("Recibido mensaje: "+msg.text);
    if(subscribers.count()==0) { //Si no hay suscriptores...
      Serial.println("Aun no hay suscriptores");
      if(msg.text=="/start") { //Si se trata del comando /start...
        Serial.println("Reconocido mensaje /start");
        //Componemos el mensaje a enviar en una sola cadena (es más rápido)
        String message = "RoJo Telegram Bot library\n";
        message += "Photo PIR\n\n";
        message += "/subscribe : Añadirse a la lista\n";
        bot.sendMessage(msg.chat_id,message); //Enviamos el mensaje
      } else if(msg.text=="/subscribe") {
        Serial.println("Reconocido mensaje /subscribe");
        //El primer usuario en suscribirse
        subscriptionCodeGenerator="él mismo";
        subscribe(&msg);
      }
    } else { //Hay algún suscriptor
      Serial.println("Hay suscriptores");
      //Sólo atenderemos peticiones de suscriptores
      //Lo primero que necesitamos saber es si el autor del mensaje recibido es suscriptor
      Serial.println("Identificador de suscriptor: "+msg.chat_id);
      if(subscribers.containsKey(msg.chat_id)) { //Si el autor es suscriptor...
        Serial.println("Suscriptor reconocido");
        //Podemos hacerle caso
        if(msg.text=="/start") { //Si se trata del comando /start...
          Serial.println("Reconocido mensaje /start");
          String message = "RoJo Telegram Bot library\n";
          message += "Photo PIR\n\n";
          message += "/on : Activa el detector\n";
          message += "/off : Desactiva el detector\n";
          message += "/photo : toma foto\n";
          message += "/resX : seleccionar resolución\n";
          message += "/info : información\n";
          message += "/users : gestión de usuarios\n";
          message += "/subscribe code : Añadirse a la lista\n";
          //Enviamos el mensaje con menú de selección de un sólo uso
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        } else if(msg.text=="/status") {
          Serial.println("Reconocido mensaje /status");
          //Mostramos el estado actual
          String message = "Sensor de movimiento: ";
          message+=(enabled==true)?"encendido":"apagado";
          message+="\nResolución actual: "+String(currentRes);
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,defaultKeyb,true);
        } else if(msg.text=="/resX") {
          Serial.println("Reconocido mensaje /resX");
          //Sólo presentaremos las opciones y el menú de selección
          String message = "Selecciona resolución\n";
          String keyb="[[\"/res0\",\"/res1\",\"/res2\"],[\"/res3\",\"/res4\",\"/res5\"],[\"/res6\",\"/res7\",\"/res8\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        } else if(msg.text=="/users") {
          Serial.println("Reconocido mensaje /users");
          //Sólo presentaremos las opciones y el menú de selección
          String message = "RoJo Telegram Bot library\n";
          message += "Photo PIR\n\n";
          message += "/generate : Genera un código de suscripción\n";
          message += "/unsubscribe : Borrarse de la lista\n";
          String keyb="[[\"/generate\",\"/unsubscribe\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        } else if(msg.text=="/info") {
          Serial.println("Reconocido mensaje /info");
          //Sólo presentaremos las opciones y el menú de selección
          String message = "RoJo Telegram Bot library\n";
          message += "Photo PIR\n\n";
          message += "/status : Estado actual\n";
          message += "/subscribers : Mostrar la lista de suscriptores\n";
          String keyb="[[\"/status\",\"/subscribers\"]]";
          //Enviamos el mensaje con menú de selección
          bot.sendMessage(msg.chat_id,message,keyb,true);
        } else if(msg.text=="/generate") {
          Serial.println("Reconocido mensaje /generate");
          //Genera un código de suscripción que será válido durante el próximo minuto
          subscriptionCode=random(65535)+1; //Entre 1 y 65535
          //Anotamos el nombre del generador
          subscriptionCodeGenerator=msg.from_name;
          //Anotamos la hora en la que caduca. Dentro de 60 segundos
          subscriptionCodeTimeout=millis()+60000;
          //Informamos del código
          bot.sendMessage(msg.chat_id,"Código: " + String(subscriptionCode),defaultKeyb,true);
        } else if(msg.text=="/unsubscribe") {
          Serial.println("Reconocido mensaje /unsubscribe");
          //El usuario actual quiere eliminar la suscripción
          unsubscribe(&msg);
          //Informamos al usuario
          bot.sendMessage(msg.chat_id,"Suscripción borrada");
        } else if(msg.text=="/subscribers") {
          Serial.println("Reconocido mensaje /subscribers");
          //Solicitan la lista de suscriptores
          bot.sendMessage(msg.chat_id,"Suscriptores:"+list(),defaultKeyb,true);
        } else if(msg.text=="/photo") {
          Serial.println("Reconocido mensaje /photo");
          uint32_t resX,resY; //Resolución de la imagen guardada
          //Solicitamos que tome una foto
          camera.takePhoto();
          //Enviamos un mensaje de acción de "escribiendo"
          bot.sendChatAction(msg.chat_id,0);
          //Pedimos que se guarde la foto y anotamos el posible error
          byte errorCode=camera.savePhoto("photo",&resX,&resY);
          //Si no ha dado tiempo a sacar la foto...
          if(errorCode==3) {
            //Añadiremos unos cuantos intentos más, para asegurarnos
            //Número de ciclos de espera
            byte tryCount=20;
            //Mientras no haya tenido tiempo de tomar la foto y no hayamos alcanzado el máximo número de intentos...
            while(errorCode==3 && tryCount-->0) {
              //Damos tiempo a que termine de capturar la foto
              delay(100);
              //Pedimos que se guarde la imágen y tomamos nota del error
              //Como nombre de archivo sólo el nombre. Sin barra inicial. La extensión la pone él
              errorCode=camera.savePhoto("photo",&resX,&resY);
            }
            Serial.println("DEBUG. tryCount="+String(tryCount));
          }
          //Si hubo algún error...
          if(errorCode) {
            //...informamos
            Serial.println("Fallo al enviar. Error " + String(errorCode));
            bot.sendMessage(msg.chat_id,"Fallo al enviar. Error " + String(errorCode));
          } else { //No hubo errores
            //...enviamos un mensaje de acción de "enviando foto"
            bot.sendChatAction(msg.chat_id,1);
            //...enviamos la foto por Telegram. Si tiene un identificador...
            if(bot.sendPhotoLocal(msg.chat_id,"/photo.jpg").length()>0) {
              Serial.println("DEBUG. Foto enviada correctamente.");
            } else { //No tiene identificador
              Serial.println("DEBUG. Fallo al enviar foto.");
            }
          }
        } else if(msg.text=="/on") {
          Serial.println("Reconocido mensaje /on");
          enabled=true; //Se activa el detector de movimento
          broadcast(msg.from_name + " ha activado el detector"); //Informamos
        } else if(msg.text=="/off") {
          Serial.println("Reconocido mensaje /off");
          enabled=false; //Se desactiva el detector de movimento
          broadcast(msg.from_name + " ha desactivado el detector"); //Informamos
        } else if(msg.text.length()==5) { //Si el comando tiene una longitud de 5...
          if(msg.text.substring(0,4)=="/res") { //...si el comando comienza por /res...
            //...si el último carácter es numérico...
            String n=msg.text.substring(4);
            if(n>="0" && n<="8") {
              Serial.println("Reconocido mensaje /resX");
              currentRes=n[0]-48; //...obtenemos el valor del código de la resolución
              camera.setResolution(currentRes); //Aplicamos la resolución
              //Informamos a todos los suscriptores del cambio de resolución
              broadcast(msg.from_name + " cambia resolución a " + n);
            }
          }
        } else { //Si no es ningún comando reconocido...
          Serial.println("Mensaje no reconocido");
          //...enviamos el mensaje a todos los suscriptores
          broadcast(msg.from_name + " dijo: " + msg.text);
        }
      } else { //El autor no es suscriptor
        Serial.println("Suscriptor no reconocido");
        if(msg.text.substring(0,11)=="/subscribe ") {
          Serial.println("Reconocido mensaje /subscribe");
          //Parece que alguien quiere suscribirse...
          //Si el código actual ha caducado...lo eliminamos
          if(millis()>subscriptionCodeTimeout) subscriptionCode=0;
          if(subscriptionCode) { //Si tenemos algún código válido...
            //Si el código indicado es el correcto...incluimos el nuevo usuario como suscriptor
            if(msg.text.substring(11)==String(subscriptionCode)) subscribe(&msg);
            //Si el código indicado no es el correcto...informamos
            else bot.sendMessage(msg.chat_id,"Código incorrecto"); 
            subscriptionCode=0; //Desactivamos el código. Ya ha sido utilizado
          }
        }
      }
    }
    
    //Reseteamos el tiempo de espera
    currentWait=startWait;
    //Refrescamos los procesos de fondo
    yield();
    //Hemos terminado de procesar el mensaje actual
    //Obtenemos el siguiente mensaje
    bot.getNextMessage(&msg);
  }
}

//Función de gestión de interrupciones del PIR
void interruptPIR() {
  //Se ha detectado movimiento
  //Si el detector está activo...activamos el flag
  if(enabled) movementDetected=true;
}

//Compone una respuesta correcta en HTML con la página de configuración
String htmlConfig() {
  String answer="HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n\n";
  answer+="<!DOCTYPE html><html><h1>photoPIR configuration</h1>";
  answer+="<p><label>WiFi SSID:<input id=\"wifiSSID\" type=\"text\" name=\"SSID\" value=\"" + config.value("SSID") +"\"/></label></p>";
  answer+="<p><label>WiFi password:<input id=\"wifiPassword\" type=\"text\" name=\"password\" value=\"" + config.value("password") +"\" /></label></p>";
  answer+="<p><label>Telegram bot token:<input id=\"botToken\" type=\"text\" name=\"botToken\" value=\"" + config.value("botToken") + "\" size=\"50\"/></label></p>";
  answer+="<p><a href=\"/?\" onclick=\"location.href=this.href+wifiSSID.value+'&'+wifiPassword.value+'&'+botToken.value;return false;\"><button>set</button></a>";
  answer+="<a href=\"/\"><button>refresh</button></a></p>";

  answer+="<p><h3>";
  if(errorMessageWiFi.length()>0) answer+=errorMessageWiFi;
  else if(connecting) answer+="Conectando";
  else if(errorMessageBot.length()>0) answer+=errorMessageBot;
  answer+="</h3></p>";

  answer+="</html>";
  return answer; //Devolvemos la cadena HTML
}

//Comprueba si hay conexiones web en el servidor
void serverHandleClient() {
  if(!server.hasClient()) return; //Si no hay clientes conectados...hemos terminado
  //Hay algún cliente conectado
  
  WiFiClient client=server.available(); //Tomamos nota de quién es
  //Sólo leeremos la primera línea de la solicitud recibida
  String requestHeader=""; //Variable en la que guardaremos la primera linea de la solicitud
  bool lineEnd=false; //Inicialmente no hemos terminado de leer la primera línea
  while(client.available() && !lineEnd) { //Mientras haya información pendiente de recibir y no hayamos completado la primera línea...
    char c=client.read(); //Anotamos el siguiente carácter
    if(c=='\n') lineEnd=true; //Si en un fin de línea...hemos terminado de leer la primera línea
    else requestHeader+=c; //Si no es un fin de línea...lo añadimos a los caracteres de la línea
  }
  //Hemos terminado de leer la primera línea con alguna de las siguientes sintaxis:
  //  GET / HTTP/1.1
  //  GET /favicon.ico HTTP/1.1
  //  GET /?ssd1&pass1&token1 HTTP/1.1
  //Los que más nos interesan son:
  //  GET / HTTP/1.1 : Refrescaremos el contenido de la página pero no actualizaremos nada
  //  GET /?ssd1&pass1&token1 HTTP/1.1 : Actualizaremos a los valores indicados
  //Cualquier otra sintaxis no será tenida en cuenta

  int pos=requestHeader.indexOf('?'); //Anotamos la posición del inicio de los parámetros '?'
  if(pos>=0) { //Si se han encontrado parámetros...
    requestHeader=requestHeader.substring(pos+1); //Recortamos el prefijo
    pos=requestHeader.indexOf('&'); //Localizamos el final del primer parámetro '&'
    if(pos>=0) { //Si hemos encontrado el final del primer parámetro...
      String ssid=requestHeader.substring(0,pos); //Lo anotamos
      requestHeader=requestHeader.substring(pos+1); //Recortamos el prefijo
      pos=requestHeader.indexOf('&'); //Localizamos el final del segundo parámetro '&'
      if(pos>=0) { //Si hemos encontrado el final del segundo parámetro...
        String pass=requestHeader.substring(0,pos); //Lo anotamos
        requestHeader=requestHeader.substring(pos+1); //Recortamos el prefijo
        pos=requestHeader.indexOf(' '); //Localizamos el final del tercer parámetro ' '
        if(pos>=0) { //Si hemos encontrado el final del tercer parámetro...
          String token=requestHeader.substring(0,pos); //Lo anotamos
          //Tenemos todos los parámetros
          //Los guardaremos
          config.add("SSID",ssid);
          config.add("password",pass);
          config.add("botToken",token);
          WiFi.disconnect(); //Desconectamos de WiFi
          errorMessageBot=errorMessageWiFi=""; //Borramos posibles errores
          connecting=false; //No estamos conectando
        }
      }
    }
  }
  //Hemos terminado de procesar la solicitud

  client.println(htmlConfig()); //Enviamos respuesta HTML
  //Damos tiempo a que se envíe el texto HTML
  //Si no lo hacemos, se cerrará la conexión sin enviarlo completamente y obtendremos un error en el navegador
  //o simplemente el browser no sabrá interpretar el código incompleto y no mostrará nada
  delay(20);
  client.stop(); //Cerramos conexión con este cliente
}

void setup() {
  Serial.begin(115200); //Activamos el puerto serie para mensajes de debug
  while(!Serial) delay(1);

  //Configuración de led integrado
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  digitalWrite(pinLed,!ledON); //Led apagado

  //Inicializamos la cámara con el pin CS
  Serial.print("\nInicializando cámara...");
  byte errorCode=camera.begin(pinCS);
  if(errorCode) { //Si hubo algún error...
    Serial.println("Error "+String(errorCode));
    //Hacemos parpadear el led tantas veces como el código de error
    for(byte i=0;i<errorCode*2;i++) {
      digitalWrite(pinLed,!digitalRead(pinLed));
      delay(300);
    }
    //Reiniciamos
    //Atención!. La función restart se queda colgada la primera vez que se ejecuta después de
    //transferir el programa. Para evitarlo se debe reiniciar manualmente una vez.
    ESP.restart();
  }
  Serial.println("Ok"); //Cámara inicializada correctamente

  //Inicialización de diccionarios
  config.begin("/config.txt"); //Inicializamos el diccionario de configuración
  subscribers.begin("/subscribers.txt"); //Inicializamos el diccionario de suscriptores

  //Configuramos el wifi en modo mixto
  //Se conectará a un punto de acceso y al mismo tiempo creará su propio punto de acceso
  //La conexión cliente servirá para salir a Internet
  //La conexión servidor servirá para cambiar la configuración
  WiFi.mode(WIFI_AP_STA);
  //Configuramos el punto de acceso wifi propio
  WiFi.softAP(localSSID,localPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor web para cambio de configuración

  //Inicialización de sensor PIR
  pinMode(pinPIR,INPUT); //Configuramos el pin del PIR como entrada
  attachInterrupt(pinPIR,interruptPIR,RISING); //Activamos las interrupciones para el pin del PIR

  server.begin(); //Arrancamos el servidor web para cambio de configuración
  Serial.println("Inicio correcto");
  randomSeed(millis()); //Inicializamos la semilla de números aleatorios
}

void loop() {
  if(errorMessageWiFi.length()==0) { //Si no hay mensaje de error WiFi...
    if(connecting) { //Si estamos conectando...
      if(WiFi.status()==WL_CONNECTED) { //Si ya estamos conectados...
        Serial.println("Conectado!");
        connecting=false;
        //Si no hay token para el bot...error
        if(config.value("botToken").length()==0) errorMessageBot="bot token es obligatorio";
        else { //Parece que tenemos token para el bot
          bot.begin(config.value("botToken")); //Inicializamos el bot
          broadcast("Dispositivo reiniciado"); //Informamos del reinicio
        }
      } else { //Si no estamos conectados...
        if(millis()>timeoutConnecting) { //Si hemos superado el tiempo máximo de conexión...
          errorMessageWiFi="SSID y/o password incorrectos";
          Serial.println(errorMessageWiFi); //Debug
        }
      }
    } else { //Si no estamos conectando...
      if(WiFi.status()!=WL_CONNECTED) { //Si no estamos conectados...
        //Si el SSID o password están vacíos...
        if(config.value("SSID").length()==0 || config.value("password").length()==0) {
          errorMessageWiFi="SSID y password son obligatorios";
          Serial.println(errorMessageWiFi); //Debug
        } else { //Si SSID & password están rellenos...
          WiFi.begin(config.value("SSID").c_str(),config.value("password").c_str()); //Solicitamos conexión al punto de acceso
          timeoutConnecting=millis()+15000; //Timeout de 15s
          connecting=true; //Estamos conectando
          Serial.print("Conectando...");
        }
      } else { //Si estamos conectados...
        if(errorMessageBot.length()==0) { //Si no tenemos un error de Telegram...
          if(movementDetected) { //Si se ha detectado movimiento...
            //Debemos tomar una foto y enviarla a todos los suscriptores
            camera.takePhoto(); //Tomamos la foto
            uint32_t resX,resY; //Resolución de la imagen guardada
            byte errorCode=0; //Código de error
            //Solicitamos descargar la imagen a un archivo local
            //Guardamos el código de error
            //Repetimos mientras no se haya terminado la captura
            while((errorCode=camera.savePhoto("photo",&resX,&resY))==3) delay(5);
            //Si tenemos algún error...informamos a todos
            if(errorCode) broadcast("Fallo guardar imagen. Error " + String(errorCode));
            //No hay errores...enviamos la imagena todos
            else broadcastPhoto("/photo.jpg");
            movementDetected=false; //Desactivamos el flag
          }
          if(millis()>timeoutCheckingBot) { //Si ya hay que comprobar nuevos mensajes de Telegram...
            currentWait*=factorWait; //Aumentamos el tiempo de espera para el siguiente ciclo
            if(currentWait>maxWait) currentWait=maxWait; //Si sobrepasamos el máximo...lo limitamos
            timeoutCheckingBot=millis()+currentWait; //Calculamos el tiempo para la siguiente comprobación
            handleNewMessages(); //Procesa todos los nuevos mensajes recibidos de Telegram
          }
        }
      }
    }
  }
  serverHandleClient(); //Atendemos al servidor web de configuración
}