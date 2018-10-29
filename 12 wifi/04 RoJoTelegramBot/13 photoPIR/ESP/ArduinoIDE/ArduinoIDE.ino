/*
  Autor: Ramón Junquera
  Versión: 20181029
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Enviar una imagen al detectar movimiento
  Material: placa ESP, ArduCAM-Mini-2MP, PIR sensor
  Descripción:
    Sistema de videovigilancia con control por Telegram
    El sistema consta de una placa ESP, una cámara y un sensor infrarrojo de movimiento.
  Configuración del la transferencia:
    Puesto que no tenemos limitación de alimentación, usaremos la máxima frecuencia de
    reloj en el procesador.
    La configuración de memoria por defecto es de 1Mb para programa y 3Mb para SPIFFS.
    Transferir tanto la aplicación como los datos.
  Transferencia por cable:
    En ESP8266 es posible que haya que desconectar el pin D4 para tranferir el programa. Después se 
    puede volver a conectar al PIR.
  Transferencia por OTA:
    La aplicación tiene en cuenta la constante global devicePassword como contraseña de OTA.
    Es importante seleccionar la opción: Herramientas/Erase Flash/Sketch + WiFi Settings
    Esto es porque estamos utilizando contraseñas para OTA y podrían quedarse los parámetros de la
    configuración previa.
    Recordar que si queremos actualizar datos, no podremos hacerlo con contraseña.
    Se tendría que transferir una nueva apliación sin contraseña OTA para después actualizar los
    datos por OTA.
    La transferencia se puede hacer tanto a la dirección IP del punto de acceso que da conexión
    a Internet como a través de la dirección 192.168.4.1 del punto de acceso que se crea para
    configuración.
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
  Solicitud de estado:
    El botón /status informa del estado del sistema de detección de movimiento.
  Solicitud de resolución:
    El botón /res informa de la resolución con la que se están tomándo las imágenes.
  Cambio de resolución:
    El botón /resX abre un nuevo menú de 9 opciones para seleccionar la nueva resolución.
    La /res0 es la más baja y las /res9 la más alta.
  Listado de suscriptores:
    Se puede solicitar el listado de suscriptores a través de botón /users y botón /list
  Desuscribirse:
    Un suscriptor puede darse de baja él mismo a través de botón /users y botón /unsuscribe.
    Si era el último, el sistema quedará abierto, puesto que el siguiente que acceda podrá darse
    de alta a sí mismo sin la ayuda de nadie más.
  Añadir un nuevo suscriptor:
    Los nuevos suscriptores (invitado) siempre deben ser invitados por alguien suscrito (anfitrión).
    El anfitrión debe generar un código a través del botón /users y botón /generate.
    El código tiene una validez de 60 seundos. Antes de este tiempo el invitado debe enviar al bot
    el comando /subscribe seguido del código.
  Chat:
    Si un suscriptor envía al bot mensajes que no son reconocidos como comandos, se supondrá que 
    es texto normal y se le enviará el mensaje a todos los suscriptores.
*/
#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi para ESP8266
  #include <ESP8266WebServer.h> //Gestión de servidor web
  #include <FS.h> //Gestión de SPIFFS
#elif defined(ESP32)
  #include <WiFi.h> //Librería para gestión de wifi para ESP32
  #include <WebServer.h> //Gestión de servidor web
  #include <SPIFFS.h> //Gestión de SPIFFS
#endif
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoArduCAM.h" //Librería de gestión de la cámara
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo
#include <ArduinoOTA.h> //Para gestión de OTA

//Definición de constantes globales
const uint32_t checkingGap=1000; //Tiempo de espera en milisegundos para comprobación de nuevos mensajes
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
  const byte pinPIR=2; //Pin del sensor PIR
  const byte pinCS=4; //Pin CS de la cámara
#endif
//Teclado por defecto
const String defaultKeyb="[[\"/on\",\"/off\",\"/status\"],[\"/photo\",\"/users\"],[\"/res\",\"/resX\"]]";
//Nombre del archivo que guarda el estado
const String statusFile="/status.txt";
//Definimos nombre del punto de acceso wifi y su contraseña (para cambiar la configuración)
const String mySSID="RoJoPhotoPIR";
const String mySSIDpassword="12345678";
//Contraseña del dispositivo. Utilizada para:
//- Cambiar la configuración
//- Actualizar por OTA
const String devicePassword="xxx";

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
//Creamos un servidor web en el puerto 80 para cambiar la configuración
//La ip del servidor es fija en el punto de acceso propio: 192.168.4.1
//La URL para cambiar la dirección será: http://192.168.4.1
#ifdef ESP8266 //Si es una ESP8266...
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#endif
String wifiClientSSID; //Nombre del punto de acceso (SSID) al que nos conectamos como clientes
String wifiClientPassword; //Contraseña de punto de acceso al que nos conectamos como clientes
String botToken; //Token del bot de Telegram ToniCamera1

String htmlConfig()
{
  //Compone un texto HTML con la página de configuración según la siguiente plantilla
  //<html>
  //<h1>photoPIR configuration</h1>
  //<p><label>Wifi client SSID:<input id="wifiSSID" type="text" name="SSID" value="var1"/></label></p>
  //<p><label>Wifi client password:<input id="wifiPassword" type="text" name="password" value="var2" /></label></p>
  //<p><label>Telegram bot token:<input id="botToken" type="text" name="botToken" value="var3" size="50"/></label></p>
  //<p><label>device Id:<input id="devicePassword" type="text" name="devicePassword" /></label></p>
  //<p><a href="?" onclick="location.href=this.href+'p0='+wifiSSID.value+'&p1='+wifiPassword.value+'&p2='+botToken.value+'&p3='+devicePassword.value;return false;"><button>set</button></a>
  //</html>

  //Se sustituyen los valores de los campos por los valores de la configuración actual
  String answer="<html><h1>photoPIR configuration</h1>";
  answer+="<p><label>Wifi client SSID:<input id=\"wifiSSID\" type=\"text\" name=\"SSID\" value=\"" + wifiClientSSID +"\"/></label></p>";
  answer+="<p><label>Wifi client password:<input id=\"wifiPassword\" type=\"text\" name=\"password\" value=\"" + wifiClientPassword +"\" /></label></p>";
  answer+="<p><label>Telegram bot token:<input id=\"botToken\" type=\"text\" name=\"botToken\" value=\"" + botToken + "\" size=\"50\"/></label></p>";
  answer+="<p><label>device Id:<input id=\"devicePassword\" type=\"text\" name=\"devicePassword\" /></label></p>";
  answer+="<p><a href=\"?\" onclick=\"location.href=this.href+'p0='+wifiSSID.value+'&p1='+wifiPassword.value+'&p2='+botToken.value+'&p3='+devicePassword.value;return false;\"><button>set</button></a></html>";
  //Devolvemos la cadena HTML
  return answer;
}

void serverHandleRoot()
{
  //Se ha solicitado la página web raíz

  //Para que un cambio de configuración sea aceptado se debe cumplir que al menos tenga
  //cuatro parámetros de nombres p0, p1, p2 y p3.
  //Que el valor del parámetro p4 coincida con el identificador del dispositivo (deviceId)
  //Variable para anotar si tiene todos los parámetros. Inicialmente no tienen ninguno
  byte paramCounter=0;
  //Array en el que guardaremos los parámetros ordenados
  String params[4];
  //Recorremos todos los argumentos recibidos...
  for(byte i=0;i<server.args();i++)
  {
    //...si el parámetro es reconocido...
    if(server.argName(i)=="p0")
    {
      paramCounter|=1; //Anotamos que tenemos el parámetro 0 (bit 0) 2^0
      params[0]=server.arg(i); //Anotamos el parámetro
    }
    else if(server.argName(i)=="p1")
    {
      paramCounter|=2; //Anotamos que tenemos el parámetro 1 (bit 1) 2^1
      params[1]=server.arg(i); //Anotamos el parámetro
    }
    else if(server.argName(i)=="p2")
    {
      paramCounter|=4; //Anotamos que tenemos el parámetro 2 (bit 2) 2^2
      params[2]=server.arg(i); //Anotamos el parámetro
    }
    else if(server.argName(i)=="p3")
    {
      paramCounter|=8; //Anotamos que tenemos el parámetro 3 (bit 3) 2^3
      params[3]=server.arg(i); //Anotamos el parámetro
    }
  }
  //Hemos procesado todos los parámetros recibidos
  //Si no se han recibido todos los parámetros necesarios...
  //...o el parámetro 3 (devicePassword) no coincide...
  //...devolvemos de nuevo la misma página de configuración
  if(paramCounter<15 || params[3]!=devicePassword) server.send(200,"text/html",htmlConfig());
  else //Si la configuración es correcta...
  {
    //...cambiamos los valores de las variables
    wifiClientSSID=params[0]; //Parámetro 0 = SSID
    wifiClientPassword=params[1]; //Parámetro 1 = password
    botToken=params[2]; //Parámetro 2 = botToken;
    //Guardaremos los valores actuales en el archivo de configuración
    saveStatus();
    //Devolvemos la página de cambio de configuración correcto
    //La página se base en el HTML:
    //<html>
    //<h1>photoPIR configuration applied!</h1>
    //<h2>restarting...</h2>
    //</html>
    server.send(200,"text/html","<html><h1>photoPIR configuration applied!</h1><h2>restarting...</h2></html>");
    //Esperamos un momento
    delay(1000);
    //Reiniciamos el dispositivo para que conecte con la nueva configuración
    ESP.restart();
  }
}

void try2connect()
{
  //Intenta conectar con el punto de acceso y contraseña definidas en constantes globales
  //Si lo consigue, devuelve el control.
  //Si no lo consigue en menos de 10 segundos, resetea y comienza de nuevo.

  //Configuramos el wifi en modo mixto
  //Se conectará a un punto de acceso y al mismo tiempo creará su propio punto de acceso
  //La conexión cliente servirá para salir a Internet
  //La conexión servidor servirá para cambiar la configuración
  WiFi.mode(WIFI_AP_STA);
  //Configuramos el punto de acceso wifi propio
  //Necesitamos convertir los Strings en arrays de char
  WiFi.softAP(mySSID.c_str(),mySSIDpassword.c_str()); //Nombre y contraseña
  //Indicamos la función a la que se debe llamar cuando se solicite la URL raíz
  server.on("/",serverHandleRoot);
  //Arrancamos el servidor web para cambio de configuración
  server.begin();
  //Inicializamos conexión wifi con las credenciales
  //Necesitamos convertir los Strings en arrays de char
  WiFi.begin(wifiClientSSID.c_str(),wifiClientPassword.c_str());
  //Configuramos el pin del led integrado como salida
  pinMode(pinLed,OUTPUT);
  //Variable para contar las veces que repetimos el bucle
  //Si el bucle dura 100ms -> 100*100=10000ms=10s
  byte loops=100;
  //Repetir mientras no se haya conectado ni haya superado el tiempo máximo
  while (WiFi.waitForConnectResult() != WL_CONNECTED && loops>0)
  {
    //Cambiamos el estado del led integrado
    digitalWrite(pinLed,!digitalRead(pinLed));
    //Esperamos un momento
    delay(100);
    //Ya tenemos un ciclo menos
    loops--;
  }
  //Si no hemos podido conectar...reseteamos
  if(WiFi.waitForConnectResult() != WL_CONNECTED) ESP.restart();
  //Hemos podido conectar!
  //Apagamos el led integrado
  digitalWrite(pinLed,!ledON);
  //Devolvemos el control
}

void saveStatus()
{
  //Guarda la configuración actual (resolución y estado)
  //en un archivo en SPIFFS

  //Abrimos el archivo del estado para escritura
  File f=SPIFFS.open(statusFile,"w");
  //Línea 1: resolución
  f.println(currentRes);
  //Línea 2: estado
  f.println(enabled);
  //Línea 3: SSID
  f.println(wifiClientSSID);
  //Línea 4: WiFi password
  f.println(wifiClientPassword);
  //Línea 5: botToken
  f.println(botToken);
  //Cerramos el archivo
  f.close();
}

String readLine(File f)
{
  //Lee una línea de texto de un archivo abierto
  //La línea debe finalizar con un \n

  //Leemos la línea
  String txt=f.readStringUntil('\n');
  //Le quitamos el último carácter (el \n)
  //Y devolvemos la respuesta
  return txt.substring(0,txt.length()-1);
}

void readStatus()
{
  //Recupera la configuración guardada (resolución y estado) y la aplica

  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //Se guarda la configuración actual
    saveStatus();
    //Ahora seguro que sí existe el archivo de configuración
    //Se vuelve a abrir como sólo lectura
    f=SPIFFS.open(statusFile,"r");
  }
  //Tenemos el archivo abierto

  //El archivo debe tener 5 líneas
  Serial.println("Leyendo archivo de configuración:");
  //  línea 1: resolución
  currentRes=readLine(f).toInt();
  Serial.println("currentRes="+String(currentRes));
  //Aplicamos la resolución
  camera.setResolution(currentRes);
  //  línea 2: estado
  enabled=readLine(f).toInt();
  Serial.println("enabled="+String(enabled));
  //  línea 3: SSID
  wifiClientSSID=readLine(f);
  Serial.println("wifiClientSSID="+wifiClientSSID);
  //  línea 4: password
  wifiClientPassword=readLine(f);
  Serial.println("wifiClientPassword="+wifiClientPassword);
  //  línea 5: botToken
  botToken=readLine(f);
  Serial.println("botToken="+botToken);
  //Cerramos el archivo
  f.close();
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

  //Creamos un nuevo mensaje
  TelegramMessage msg;
  //Obtenemos el siguiente mensaje
  bot.getNextMessage(&msg);
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
    //Obtenemos el siguiente mensaje
    bot.getNextMessage(&msg); 
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
  //Inicializamos el sistema de archivos
  SPIFFS.begin();
  //Leemos la última configuración guardada
  readStatus();
  //Inicializamos el bot
  bot.begin(botToken);
  //Configuramos el pin del led como salida
  pinMode(pinLed,OUTPUT);
  //Nos aseguramos que el led esté apagado
  digitalWrite(pinLed,!ledON);
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
  try2connect();

  //Configuramos el pin del PIR como entrada
  pinMode(pinPIR,INPUT);
  //Activamos las interrupciones para el pin del PIR
  attachInterrupt(pinPIR,interruptPIR,RISING);
  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());

  //Definimos las funciones de OTA
  ArduinoOTA.onStart([]()
  {
    //Se ha detectado el inicio de una recepción OTA
    //Configuramos el pin del led integrado como salida
    pinMode(pinLed,OUTPUT);
    //La petición OTA puede ser de dos tipos:
    // - Para actualizar el programa (ArduinoOTA.getCommand()==U_FLASH)
    // - Para actualizar SPIFFS (ArduinoOTA.getCommand()==U_SPIFFS)
    //Tenemos de distinguirlo, porque si es para actualizar SPIFFS, debemos
    //desmontar la partición SPIFFS para dejar que lo actualice sin problemas.
    if(ArduinoOTA.getCommand()==U_SPIFFS) SPIFFS.end();
  });
  ArduinoOTA.onEnd([]()
  {
    //Ha finalizado la transferencia OTA
    //Apagamos el led integrado
    digitalWrite(pinLed,!ledON);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    //Ha cambiado el porcentaje de progreso OTA
    //Cambiamos el estado del led integrado
    digitalWrite(pinLed,!digitalRead(pinLed));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    //Se ha producido un error en la recepción OTA

    //Led apagado
    digitalWrite(pinLed,!ledON);
    //Esperamos un segundo para comenzar con el código de error
    delay(1000);

    //Repetiremos 3 veces...
    for(byte i=0;i<3;i++)
    {
      //Repetimos tantas el doble de veces de error+1
      //Porque el primer error es el 0
      //Y porque el bucle cambia el estado del led
      //Suponemos que siempre comenzamos desde apagado
      for(byte e=0;e<2*(error+1);e++)
      {
        //Cambiamos el estado del led integrado
        digitalWrite(pinLed,!digitalRead(pinLed));
        //Esperamos medio segundo
        delay(500);
      }
      //Hemos mostrado el código de error
      //Esperamos un segundo para indicar que hemos terminado de emitir el código
      delay(1000);
    }
    //Hemos terminado

    //Los códigos de error son los siguientes
    // 1 : OTA_AUTH_ERROR
    // 2 : OTA_BEGIN_ERROR
    // 3 : OTA_CONNECT_ERROR
    // 4 : OTA_RECEIVE_ERROR
    // 5 : OTA_END_ERROR
    
    //Tras un error OTA, siempre se reinicia automáticamente
  });

  //Asignamos una contraseña para impedir que cualquiera pueda actualizarlo
  //No es obligatorio
  //Atención!. Es incompatible con el plugin de subida de archivos a SPIFFS
  //Si tenemos contraseña...la fijamos
  if(devicePassword.length()>0) ArduinoOTA.setPassword(devicePassword.c_str());
  //Asignamos un nombre al dispositivo para tenerlo identificado
  //Si no lo hacemos tomará como nombre por defecto: "esp8266-[ChipID]"
  ArduinoOTA.setHostname("PhotoPIR");
  //Iniciamos el servicio de OTA
  ArduinoOTA.begin();
  
  //Informamos del reinicio
  broadcast("Dispositivo reiniciado");
  Serial.println("Inicio correcto");
}

void loop()
{
  //Procesa todos los nuevos mensajes recibidos de Telegram
  handleNewMessages();
  //Comprobamos si hay alguna petición de actualización por OTA
  ArduinoOTA.handle();
  //Refresca las conexiones del servidor web
  server.handleClient();
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
