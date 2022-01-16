/*
  Autor: Ramón Junquera
  Versión: 20220111
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Enviar una imagen al detectar movimiento
  Descripción:
    Sistema de videovigilancia con control por Telegram
    El sistema consta de una placa ESP, una cámara y un sensor infrarrojo de movimiento.
  Configuración del la transferencia:
    Puesto que no tenemos limitación de alimentación, usaremos la máxima frecuencia de
    reloj en el procesador.
    La configuración de memoria por defecto es de 1Mb para programa y 3Mb para LittleFS.
    Transferir tanto la aplicación como los datos.
  Transferencia por cable:
    En ESP8266 es posible que haya que desconectar el pin D4 para tranferir el programa. Después se 
    puede volver a conectar al PIR.
  Configuración del programa:
    El dispositivo crea un punto de acceso wifi con el nombre definido en la constante global
    mySSID y contraseña en mySSIDpassword.
    Nos conectamos y accedemos a la URL http://192.168.4.1
    Se mostrará la configuración actual (inicialmente vacía).
    Se puede cambiar el nombre y contraseña del punto de acceso wifi que le proporcionará la
    salida a internet.
    También se configura el token del bot de Telegram.
    Por último se solicita la contraseña del dispositivo, definida en la constante global
    devicePassword.
    Si todo es correcto, se guarda la configuración y se reinicia el dispositivo.
  Menús:
    General
      /on /off
      /photo /resX
      /info /users
    /resX
      /res0 /res1 /res2
      /res3 /res4 /res5
      /res6 /res7 /res8
    /info
      /status /subscribers
    /users
      /generate /unsubscribe
  Primer uso:
    En Telegram debemos buscar el nombre del bot.
    La primera vez no hay definidos usuarios.
    El primer usuario puede suscribirse a sí mismo. A partir de entonces, el resto tendrán que 
    ser invitados por alguno de los suscriptores.
  Inicio de conversación con el bot:
    Para iniciar cualquier bot se escribe el comando /start
    A partir de aquí se mostrarán las opciones en menús (botones).
  Sistema de detección de movimiento:
    Tras el primer arranqe está desactivado.
    Se puede cambiar el estado con los botones /on y /off.
    Cuando se cambia el estado, se informa a todos los suscriptores.
    Cuando está activado y el sensor infrarrojo detecta movimiento, se toma una fotografía
    y se envía a todos los suscriptores.
    El tiempo de envío depende de la resolución de la imágen.
  Fotografía forzada:
    Con independencia se si está activo o no el sistema de detección de movimiento, siempre se
    puede forzar a tomar una imágen con el botón /photo. Esta acción no informará al resto
    de suscriptores.
  Corte de alimentación:
    Si el dispositivo sufre un corte de alimentación, la próxima vez que reinicie enviará un
    mensaje a todos los suscriptores informando y recuperará la última configuración utilizada.
  Tiempo de reacción a comandos:
    El tiempo de respuesta a un comando es variable. Se va alargando si no recibe nuevas 
    peticiones, hasta alcanzar un máximo de 30 segundos.
  Menú resX:
    Permite cambiar la resolución entre 9 opciones.
    La /res0 es la más baja y las /res9 la más alta.
  Menú /info:
    Permite:
      - Consultar el estado actual del detector de movimiento
      - Consultar la resolución actual
      - Consultar el listado de suscriptores
  Menú /users:
    Permite:
      - Generar un código para una invitación
          Los nuevos suscriptores (invitado) siempre deben ser invitados por alguien suscrito (anfitrión).
          El anfitrión debe generar un código a través del botón /users y botón /generate.
          El código tiene una validez de 60 segundos. Antes de este tiempo el invitado debe enviar al bot
          el comando /subscribe seguido del código.
      - Darse de baja como suscriptor
  Chat:
    Si un suscriptor envía al bot mensajes que no son reconocidos como comandos, se supondrá que 
    es texto normal y se le enviará el mensaje a todos los suscriptores.
  Librerías:
    Se ha utilizado la librería WiFiServer.h para gestionar el servidor web de configuración.
    ESP8266 tiene una excelente librería para este propósito llamana ESP8266WebServer.h.
    El problema es que el programa pretende ser compatible también con ESP32, y no existe una
    librería equivalente oficial para este microprocesador.

  Posibles errores en la inicialización de la cámara:
      0: No hubo errores
      1: Error al inicializar el sistema de archivos
      2: Falta el archivo RoJoArduCAM.dat
      3: Error de conexión SPI
      4: La cámara no es el modelo esperado

  Posibles errores en el envío de una imágen:
      0: Imagen guardada con éxito
      1: Cámara no inicializada
      2: No se ha capturado ninguna imagen
      3: La imagen aun no ha terminado de capturarse
      4: No se puede crear el archivo
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
const char *ssid="xxx"; //Nombre del punto de acceso (SSID)
const char *password="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
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
//Creamos objeto de gestión de diccionarios en archivo
RoJoFileDictionary subscribers;

//Enviar el mensaje actual a todos los suscriptores
void broadcast(String text) {
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++)
    //...y les enviamos el mensaje
    bot.sendMessage(subscribers.key(i),text,defaultKeyb,true);
}

//Envia una imagen a todos los suscriptores
void broadcastPhoto(String filename) {
  //Identificador de la imagen enviada. Inicialmente no existe
  String file_id="";
  //Recorremos todos los suscriptores...
  for(uint16_t i=0;i<subscribers.count();i++) {
    //Si no tenemos un identificador de imagen (si aun no hemos enviado la primera imagen)...
    if(file_id.length()==0) {
      //Enviamos el mensaje con la imagen y tomamos nota del identificador de la imagen
      //con más resolución
      file_id=bot.sendPhotoLocal(subscribers.key(i),filename);
      //Si no tenemos un file_id...
      if(!file_id.length()) {
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
  //Creamos un nuevo mensaje
  TelegramMessage msg;
  //Obtenemos el siguiente mensaje
  bot.getNextMessage(&msg);
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
        //Si se trata del comando /start...
        if(msg.text=="/start") {
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

void setup() {
  Serial.begin(115200); //Activamos el puerto serie para mensajes de debug
  while(!Serial) delay(1);

  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100); //Esperamos un momento
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  while (WiFi.status() != WL_CONNECTED) { //Mientras no estemos conectados...
    //Cambiaremos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  }
  digitalWrite(pinLed,!ledON); //Nos aseguramos que el led esté apagado
  //Hemos conseguido conectar
  
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

  pinMode(pinPIR,INPUT); //Configuramos el pin del PIR como entrada
  attachInterrupt(pinPIR,interruptPIR,RISING); //Activamos las interrupciones para el pin del PIR
  
  Serial.print("Se inicializa subscribers...");
  subscribers.begin("/subscribers.txt"); //Inicializamos el diccionario de suscriptores
  Serial.println("ok");
  
  bot.begin(botToken); //Inicializamos el bot
  randomSeed(millis()); //Inicializamos la semilla de números aleatorios

  //Informamos del reinicio
  broadcast("Dispositivo reiniciado");
  Serial.println("Inicio correcto");
}

void loop() {
  handleNewMessages(); //Procesa todos los nuevos mensajes recibidos de Telegram
  uint32_t maxTime=millis()+currentWait; //Calculamos el tiempo máximo de espera
  while(millis()<maxTime) { //Mientras no lleguemos al máximo tiempo de espera...
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
    //Refrescamos los procesos de fondo
    //Si no lo hacemos, se reseteará periódicamente debido a WDT
    yield();
  }
  currentWait*=factorWait; //Aumentamos el tiempo de espera para el siguiente ciclo
  if(currentWait>maxWait) currentWait=maxWait; //Si sobrepasamos el máximo...lo limitamos
}