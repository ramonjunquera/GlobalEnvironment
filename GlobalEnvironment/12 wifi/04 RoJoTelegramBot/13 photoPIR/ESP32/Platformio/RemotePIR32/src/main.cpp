/*
  Autor: Ramón Junquera
  Versión: 20190112
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Enviar una imagen al detectar movimiento
  Material: placa ESP32, ArduCAM-Mini-2MP, PIR sensor
  Descripción:
    Sistema de videovigilancia con control por Telegram.
    El sistema consta de una placa ESP, una cámara y un sensor de movimiento (ya sea infrarrojo
    o por microondas).
  Multitarea:  
    Aprovecharemos que el ESP32 tiene 2 cores para intentar trabajar en paralelo.
    El programa principal siempre corre en el core 1.
    Distribuiremos los procesos de la siguiente manera:
      Core 0:
        Gestión de cámara
        Gestión de comunicaciones de salida (Telegram)
      Core 1:
        Gestión de comunicaciones de entrada (Telegram)
        Interrupción PIR
        loop principal : microservicios
          Conexión wifi
          Inicialización de la cámara
          WebServer refresh
    Los módulos de gestión (de cámara y de envíos de Telegram) se hacen en base a una lista de tareas pública.
    La lista contiene como valores una estructura con todos los datos necesarios para trabajar.
    Las nuevas peticiones se añaden siempre al final y se procesan desde el principio (FIFO).
  Configuración del programa:
    El dispositivo crea un punto de acceso wifi con el nombre definido en la constante global
    mySSID y contraseña en mySSIDpassword. Por defecto RoJoPhotoPIR32
    Nos conectamos y accedemos a la URL http://192.168.4.1
    Se mostrará la configuración actual (inicialmente vacía), que consta de:
    - Nombre del punto de acceso wifi que dara conexión a Internet
    - Contraseña del punto de acceso wifi a Internet
    - Token del bot
    También se muestran algunos valores globales del estado actual.
  Menús:
    General
      /on /off
      /photo /resX
      /info /users /config
    /resX
      /res0 /res1 /res2
      /res3 /res4 /res5
      /res6 /res7 /res8
    /info
      /status /subscribers
    /users
      /generate /unsubscribe
    /config
      /reboot /format
  Primer uso:
  - Definir configuración a través del servidor web local.
  - La primera vez no hay definidos usuarios.
  - El primer usuario puede suscribirse a sí mismo. A partir de entonces, el resto tendrán que 
    ser invitados por alguno de los suscriptores.
  Inicio de conversación con el bot:
    Para iniciar cualquier bot se utiliza el comando /start
    A partir de aquí se mostrarán las opciones en menús (botones).
  Sistema de detección de movimiento:
    Tras el primer arranque está desactivado.
    Se puede cambiar el estado con los botones /on y /off.
    Cuando se cambia el estado, se informa a todos los suscriptores.
    Cuando el sensor de movimiento se activa, se toma una fotografía y se envía a todos los suscriptores.
    El tiempo de envío depende de la resolución de la imágen.
  Fotografía forzada:
    Con independencia se si está activo o no el sistema de detección de movimiento, siempre se
    puede forzar a tomar una imágen con el botón /photo. Esta acción no informará al resto
    de suscriptores.
  Corte de alimentación:
    Si el dispositivo sufre un corte de alimentación, la próxima vez que reinicie enviará un
    mensaje a todos los suscriptores informando y recuperará la última configuración utilizada.  
  Tiempo de reacción a comandos:
    El tiempo de respuesta a un comando es variable. Se va alargando si no recibe nuevas peticiones,
    hasta alcanzar un máximo de 30 segundos.
  Menú resX:
    Permite cambiar la resolución entre 9 opciones.
    La /res0 es la más baja y las /res8 la más alta.
  Menú /users:
    Permite:
      - Generar un código para una invitación
          Los nuevos suscriptores (invitado) siempre deben ser invitados por alguien suscrito (anfitrión).
          El anfitrión debe generar un código a través del botón /users y botón /generate.
          El código tiene una validez de 60 segundos. Antes de este tiempo el invitado debe enviar al bot
          el comando /subscribe seguido del código.
          Si se escribe mal el código se tendrá que generar uno nuevo.
      - Darse de baja como suscriptor
  Menú /config:
    Permite:
      - Reiniciar el dispositivo con el comando /reboot
      - Formatea la memoria de archivos con el comando /format. Intenta conservar los archivos
        necesarios para poder seguir funcionando correctamente: la configuración de comunicaciones y
        las secuencias de configuración de la cámara.
        Se pierde la información de suscriptores.
        El formateo tarda más cuanta más memoria de archivos tenga asignada.
  Chat:
    Si un suscriptor envía al bot mensajes que no son reconocidos como comandos, se supondrá que 
    es texto normal y se le enviará el mensaje a todos los suscriptores.
  Librerías:
    Se ha utilizado la librería WiFiServer.h para gestionar el servidor web de configuración.
    Para ESP32 no existen dibrerías específicas para la gestión de servidores web como tiene
    ESP8266 (ESP8266WebServer.h).

  Listado de errores
    De inicialización de la cámara
      0: No hubo errores
      1: Error al inicializar el sistema de archivos
      2: Falta el archivo RoJoArduCAM.dat
      3: Error de conexión SPI
      4: La cámara no es el modelo esperado
    De envío de una imágen:
      0: Imagen guardada con éxito
      1: Cámara no inicializada
      2: No se ha capturado ninguna imagen
      3: La imagen aun no ha terminado de capturarse
      4: No se puede crear el archivo
*/

#include <Arduino.h>
#include <SPIFFS.h> //Gestión de SPIFFS
#include <WiFiServer.h> //Gestión de servidor web
#include "RoJoArduCAM.h" //Librería de gestión de la cámara
#include "RoJoList.h" //Librería de gestión de listas para los parámetros de una solicitud web
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo
#include "soc/rtc_cntl_reg.h" //Para eliminar los mensajes de error de brownout (WRITE_PERI_REG) (alimentación insuficiente?)

//Estructuras globales
struct outCommQueueStruct
{
  //Estrutura de tareas pendientes para comunicaciones (Telegram)

  //Tipo de tarea
  //0 = enviar mensaje
  //1 = enviar foto desde archivo
  //2 = enviar última foto
  byte taskCode;
  //Solicitante
  //0 = Se responde a todos los suscriptores (broadcast)
  //Si el solicitante está identificado, se le entregará el mensaje/foto a él
  String chat_id;
  //Nombre del archivo de imágen (sin extensión) o texto del mensaje
  String fileNameOrText;
  //Cadena con el teclado
  String keybString;
};
struct cameraQueueStruct
{
  //Estrutura de tareas pendientes para la cámara

  //Tipo de tarea
  // 0 = cambiar a resolución 0
  // 1 = cambiar a resolución 1
  // 2 = cambiar a resolución 2
  // 3 = cambiar a resolución 3
  // 4 = cambiar a resolución 4
  // 5 = cambiar a resolución 5
  // 6 = cambiar a resolución 6
  // 7 = cambiar a resolución 7
  // 8 = cambiar a resolución 8
  // 9 = tomar foto
  byte taskCode;
  //Solicitante
  //vacío = No se responde (cambio de resolución inicial)
  //0 = Se responde a todos los suscriptores (broadcast)
  //Si el solicitante está identificado, se le entregará el mensaje/foto a él
  String chat_id;
  //Nombre del solicitante
  String from_name;
  //Cadena con el teclado
  String keybString;
};

//Constantes globales
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const bool ledON=LOW; //Estado del pin del led cuando está encendido
const String configFile="/config.txt";
const String defaultKeyb="[[\"/on\",\"/off\"],[\"/photo\",\"/resX\"],[\"/info\",\"/users\",\"/config\"]]"; //Teclado por defecto
const byte pinPIR=15; //Pin del sensor PIR
const byte pinCS=4; //Pin CS de la cámara
const uint32_t maxWait=30000; //Tiempo máximo de espera para consultar nuevos mensajes = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
const uint32_t startWait=1+1/(factorWait-1); //Valor inicial (mínimo) para el tiempo de espera
const String mySSID="RoJoPhotoPIR32"; //Definimos nombre del punto de acceso wifi (para cambiar la configuración)
const String mySSIDpassword="12345678"; //Contraseña de punto de acceso wifi local
const String yesno[2]={"no","sí"};

//Objetos globales
RoJoTelegramBot bot; //Gestión del bot de Telegram
RoJoFileDictionary subscribers;
RoJoList<outCommQueueStruct> outCommQueue; //Lista de tareas pendientes de mensajes de salida
RoJoList<cameraQueueStruct> cameraQueue; //Lista de tareas pendiente de la cámara
RoJoArduCAM camera;
WiFiServer server(80); //Servidor web local http://192.168.4.1

//Parámetros de configuración
byte currentRes=3; //Código de resolución actual
bool PIRenabled = false; //PIR activado?
String wifiClientSSID=""; //Nombre del punto de acceso (SSID) al que nos conectamos como clientes
String wifiClientPassword=""; //Contraseña de punto de acceso al que nos conectamos como clientes
String botToken=""; //Token del bot de Telegram

//Variables globales
bool wifiConnected=false; //Tenemos conexión wifi?
byte errorCodeCamera=255; //Código de error de la inicialización de la cámara. Inicialmente no inicializada
uint32_t wifiLastTry=0; //Última vez que se intentó reconectar wifi
bool outCommQueueStopRequest=false;
bool outCommQueueRunning=true;
bool cameraQueueStopRequest=false;
bool cameraQueueRunning=true;
bool inCommQueueRunning=true;
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint16_t subscriptionCode=0; //Código de suscripción
uint32_t subscriptionCodeTimeout=0; //millis de caducidad del código de suscripción

void saveConfig()
{
  //Guarda la configuración actual (resolución y estado)
  //en un archivo en SPIFFS

  //Abrimos el archivo del estado para escritura
  File f=SPIFFS.open(configFile,"w");
  //Si no se ha posido abrir el archivo...hemos terminado
  if(!f) return;
  //Línea 1: resolución
  f.println(currentRes);
  //Línea 2: estado
  f.println(PIRenabled);
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

void cameraAddTask(byte taskCode,String chat_id,String from_name,String keybString)
{
  //Añade una tarea a la cola de la cámara

  //Creamos una tarea nueva
  cameraQueueStruct *pCameraTask=new cameraQueueStruct;
  //Completamos sus campos
  pCameraTask->taskCode=taskCode;
  pCameraTask->chat_id=chat_id;
  pCameraTask->from_name=from_name;
  pCameraTask->keybString=keybString;
  //Añadimos la tarea al final de la cola de trabajo
  cameraQueue.add2end(pCameraTask);
}

void readConfig()
{
  //Recupera la configuración guardada (resolución y datos de conectividad)

  //Abrimos el archivo de la última configuración como sólo lectura
  File f=SPIFFS.open(configFile,"r");
  //Si no se pudo abrir...
  if(!f)
  {
    //...probablemente sea porque aun no existe el archivo
    //Guardamos la configuración actual
    saveConfig();
    //Ahora seguro que sí existe el archivo de configuración
    //Se vuelve a abrir como sólo lectura
    f=SPIFFS.open(configFile,"r");
  }
  //Tenemos el archivo abierto

  //El archivo debe tener 5 líneas
  //  línea 1: resolución
  byte res=readLine(f).toInt();
  //  línea 2: estado
  PIRenabled=readLine(f).toInt();
  //  línea 3: SSID
  wifiClientSSID=readLine(f);
  //  línea 4: password
  wifiClientPassword=readLine(f);
  //  línea 5: botToken
  botToken=readLine(f);
  //Cerramos el archivo
  f.close();

  //Creamos una nueva tarea para cambiar la resolución de la cámara
  //No es necesario que devuelva respuesta a nadie
  cameraAddTask(res,"","","");

  //Inicializamos el bot con el token
  bot.begin(botToken);

  //Indicamos que nunca hemos intentado conectar y que no tenemos conexión wifi
  //Así se intentará la conexión automáticamente
  wifiLastTry=0;
  wifiConnected=false;
}

void outCommManager(void *parameter)
{
  //Gestión de comunicaciones de salida (Telegram) en base a la lista de tareas outCommQueue

  //Declaración de variables
  outCommQueueStruct *pOutCommTask; //Puntero a tarea de mensaje de salida
  String file_id=""; //Identificador de la última imagen enviada
  
  //Comprueba continuamente la lista de tareas
  while(true)
  {
    //Si tenemos conexión wifi y el gestor de tareas está funcionando...
    if(wifiConnected && outCommQueueRunning)
    {
      //Si tenemos alguna tarea...
      if(outCommQueue.count()>0)
      {
        //Recuperamos los datos de la primera tarea
        outCommQueue.index(&pOutCommTask,0);
        //Dependiendo del tipo de tarea...
        switch(pOutCommTask->taskCode)
        {
          case 0: //Enviar un mensaje
            //Si hay que enviar el mensaje a todos los suscriptores...
            if(pOutCommTask->chat_id=="0")
            {
              //Recorremos todos los suscriptores...
              for(uint16_t i=0;i<subscribers.count();i++)
              {
                //...enviamos el mensaje a cada suscriptor
                bot.sendMessage(subscribers.key(i),pOutCommTask->fileNameOrText,pOutCommTask->keybString,true);
              } 
            }
            else //El mensaje se envía a un sólo suscriptor
            {
              bot.sendMessage(pOutCommTask->chat_id,pOutCommTask->fileNameOrText,pOutCommTask->keybString,true);
            }
            break;
          case 1: //Enviar una foto desde un archivo
            //Completamos el nombre del archivo
            pOutCommTask->fileNameOrText="/"+pOutCommTask->fileNameOrText+".jpg";
            //Si hay que enviar la foto a todos los suscriptores...
            if(pOutCommTask->chat_id=="0")
            {
              //Inicialmente no tenemos un identificador de la foto enviada
              file_id="";
              //Recorremos todos los suscriptores...
              for(uint16_t i=0;i<subscribers.count();i++)
              {
                //Si no tenemos un identificador de imagen (si aun no hemos enviado la primera imagen)...
                if(file_id.length()==0)
                {
                  //Enviamos el mensaje con la imagen y tomamos nota del identificador de la imagen
                  //con más resolución
                  file_id=bot.sendPhotoLocal(subscribers.key(i),pOutCommTask->fileNameOrText);
                }
                else //Ya hemos enviado la imagen a algún suscriptor...
                {
                  //...no enviaremos la imágen del archivo. Ahora sólo enviaremos su identificador
                  //Es mucho más rápido
                  bot.sendPhotoRemote(subscribers.key(i),file_id);
                }
              }
            }
            else //La foto sólo se envía a un suscriptor...
            {
              //Enviamos la imágen del archivo y tomamos nota de su identificador
              file_id=bot.sendPhotoLocal(pOutCommTask->chat_id,pOutCommTask->fileNameOrText);
              //Si tenemos un file_id vacío es que no se ha enviado correctamente
              //Es su lugar enviamos un mensaje de error
              if(file_id.length()==0) bot.sendMessage(pOutCommTask->chat_id,"Error al enviar la foto",pOutCommTask->keybString,true);
            }
            //Una vez enviada la foto, siempre borramos su archivo
            SPIFFS.remove(pOutCommTask->fileNameOrText);
            break;
          case 2: //Enviar de nuevo la última foto
            //Si hay que enviar la foto a todos los suscriptores...
            if(pOutCommTask->chat_id=="0")
            {
              //Recorremos todos los suscriptores...
              for(uint16_t i=0;i<subscribers.count();i++)
              {
                //Si no tenemos foto anterior...enviamos un mensaje de error
                if(file_id.length()==0) bot.sendMessage(subscribers.key(i),"Error al enviar la foto",pOutCommTask->keybString,true);
                //Si tenemos foto anterior...la enviamos
                else bot.sendPhotoRemote(subscribers.key(i),file_id);
              } 
            }
            else //La foto sólo se envía un un suscriptor...
            {
              //Si no tenemos foto anterior...enviamos un mensaje de error
              if(file_id.length()==0) bot.sendMessage(pOutCommTask->chat_id,"Error al enviar la foto",pOutCommTask->keybString,true);
              //Si tenemos foto anterior...la enviamos
              bot.sendPhotoRemote(pOutCommTask->chat_id,file_id);
            }
            break;
        }
        //Hemos terminado de procesar la primera tarea. La borramos
        outCommQueue.remove();
      }
      //Hemos terminado de procesar una tarea
    }
    //Si hos han pedido que paremos...
    if(outCommQueueStopRequest)
    {
      outCommQueueRunning=false; //Ya no procesaremos más tareas
      outCommQueueStopRequest=false; //Hemos procesado la petición de stop
    }
    delay(1); //Para evitar errores por WatchDog
  }
}

void outCommAddTask(byte taskCode,String chat_id,String fileNameOrText,String keybString)
{
  //Añade una tarea a la cola de mensajes de salida

  //Creamos una tarea nueva
  outCommQueueStruct *pOutCommTask=new outCommQueueStruct;
  //Completamos sus campos
  pOutCommTask->taskCode=taskCode;
  pOutCommTask->chat_id=chat_id;
  pOutCommTask->fileNameOrText=fileNameOrText;
  pOutCommTask->keybString=keybString;
  //Añadimos la tarea al final de la cola de trabajo
  outCommQueue.add2end(pOutCommTask);
}

void subscribe(TelegramMessage *msg)
{
  //Incluye al autor del mensaje del parámetro en la lista de suscriptores
  //e informa a todos

  //Si ya existía se actualiza el nombre
  subscribers.add((*msg).chat_id,(*msg).from_name);
  //Informamos a todos
  outCommAddTask(0,"0",(*msg).from_name + " es un nuevo suscriptor invitado por " + subscriptionCodeGenerator,defaultKeyb);
}

void cameraManager(void *parameter)
{
  //Función de gestión de la cámara en base a la lista de tareas cameraQueue
  //Se ejecuta en paralelo a la gestión de comunicaciones de Telegram

  //Declaración de variables
  cameraQueueStruct *pCameraTask; //Puntero a tarea de cámara
  
  //Comprueba continuamente la lista de tareas
  while(true)
  {
    //Si tenemos alguna tarea y la cola de tareas está activa y la cámara no tiene errores...
    if(cameraQueue.count()>0 && cameraQueueRunning==true && errorCodeCamera==0)
    {
      //Recuperamos los datos de la primera tarea
      cameraQueue.index(&pCameraTask,0);
      //Si hay que cambiar la resolución...
      if(pCameraTask->taskCode<9)
      {
        //Si nos piden una resolución distinta a la actual...
        if(currentRes!=pCameraTask->taskCode)
        {
          //...cambiamos la resolución
          camera.setResolution(pCameraTask->taskCode);
          //Anotamos el nuevo código de resolución
          currentRes=pCameraTask->taskCode;
          //Guardamos la configuración actual
          saveConfig();
        }
        //Si debemos informar...
        if(pCameraTask->chat_id.length()>0)
        {
          //Cambiamos el mensaje dependiendo de si la cámara está inicializada
          String message=(errorCodeCamera==0)?pCameraTask->from_name+" cambia a resolución "+String(currentRes):pCameraTask->from_name+" solicita un cambio de resolución, pero la cámara no ha sido inicializada correctamente. Error="+String(errorCodeCamera);
          //Creamos una tarea para enviar el mensaje
          outCommAddTask(0,pCameraTask->chat_id,message,pCameraTask->keybString);
        }
        //Hemos terminado de procesar la tarea de cambio de resolución (la primera). La borramos
        cameraQueue.remove();
      }
      else //Hay que hacer una foto!
      {
        //Solicitamos que tome una foto
        camera.takePhoto();
        //Decidimos el nombre del archivo (sin extensión)
        String fileName=String(millis());
        //Necesitamos crear variables para las dimensiones si queremos guardar la foto
        uint32_t resX,resY;
        //Inicialmente suponemos que tenemos el error 3 (aun no ha terminado de tomar la foto)
        byte errorCode=3;
        //Definimos el número de veces que lo intentaremos
        byte tryCount=30;
        //Mientras no haya tenido tiempo de sacar la foto y no hayamos agotado todos los intentos...
        while(errorCode==3 && tryCount-->0)
        {
          //Damos tiempo a que termine de toma la foto)
          delay(100);
          //Pedimos que guarde la foto en el archivo
          errorCode=camera.savePhoto(fileName,&resX,&resY);
        }
        //Si hemos tenido algún error...
        if(errorCode>0)
        {
          //No enviaremos una foto, sino un mensaje de error
          outCommAddTask(0,pCameraTask->chat_id,"No se pudo guardar la foto. Error="+String(errorCode),pCameraTask->keybString);
          //Y daremos por cerrada esta tarea
          cameraQueue.remove();
        }
        else //No hubo errores...
        {
          //Creamos una tarea para que se envíe la foto desde el archivo creado
          outCommAddTask(1,pCameraTask->chat_id,fileName,"");
          //Hemos terminado de procesar la primera tarea. La borramos
          cameraQueue.remove();

          //Miraremos las siguientes tareas de la lista por si también solicitan una foto
          //Hasta que encontremos una petición de cambio de resolución o hasta que lleguemos
          //al final de la lista de tareas.
          //Si encontramos alguna, enviaremos la misma foto. Esta vez por referencia

          //Flag para mantenernos en el loop
          bool inLoop=true;
          while(inLoop)
          {
            //Si quedan tareas en la cola...
            if(cameraQueue.count()>0)
            {
              //Recuperamos los datos de la primera tarea
              cameraQueue.index(&pCameraTask,0);
              //Si la tarea en para tomar una foto...
              if(pCameraTask->taskCode==9)
              {
                //Creamos tarea para que se envíe la imágen de la última vez (código 2)
                outCommAddTask(2,pCameraTask->chat_id,"","");
                //Hemos terminado de procesar la primera tarea. La borramos y no salimos del bucle
                cameraQueue.remove();
              }
              //La tarea no es para tomar una foto (es un cambio de resolución). Salimos del bucle
              else inLoop=false;
            }
            //Ya no hay más tareas en la cola. Salimos del bucle
            else inLoop=false;
          }
        }
      }
    }
    //Hemos terminado de procesar una tarea
    //Si hos han pedido que paremos...
    if(cameraQueueStopRequest)
    {
      cameraQueueRunning=false; //Ya no procesaremos más tareas
      cameraQueueStopRequest=false; //Hemos procesado la petición de stop
    }
    delay(1); //Para evitar errores por WatchDog
  }
}

void systemsDown()
{
  //Desactiva todos los sistemas
  //1- interrupción de PIR
  //2- gestión de mensajes de entrada
  //3- gestión de cámara
  //4- gestión de mensajes de salida

  //Variable para guardar un mensaje entrante de Telegram
  TelegramMessage msg;
  
  //Desactivamos las interrupciones del sensor PIR
  detachInterrupt(pinPIR);
  //Leemos el siguiente mensaje para que al reiniciar no recibamos de nuevo el comando de restartç
  bot.getNextMessage(&msg);
  //Solicitamos desactivar la gestión de la cámara
  cameraQueueStopRequest=true;
  //Esperamos a que se complete
  while(!cameraQueueRunning) {delay(1);};
  //Solicitamos desactivar la gestión de los mensajes de salida
  outCommQueueStopRequest=true;
  //Esperamos a que se complete
  while(!outCommQueueRunning) {delay(1);};
}

void inCommManager(void *parameter)
{
  //Gestión de comunicaciones de entrada (Telegram)

  //Variable para guardar un mensaje entrante de Telegram
  TelegramMessage msg;
  //Tiempo de espera actual en milisegundos
  uint32_t currentWait=startWait;

  //Bucle infinito
  while(true)
  {
    //Si tenemos conexión a Internet y la gestión de mensajes de entrada está activa...
    if(wifiConnected && inCommQueueRunning)
    {
      //Obtenemos el siguiente mensaje
      bot.getNextMessage(&msg);
      //Si tenemos algún mensaje...
      if(msg.text.length())
      {
        //Reseteamos el tiempo de espera
        currentWait=startWait;
        //Si no hay suscriptores...
        if(subscribers.count()==0)
        {
          //Si se trata del comando /start...
          if(msg.text=="/start")
          {
            //Componemos el mensaje a enviar en una sola cadena (es más rápido)
            String message = "RoJo Telegram Bot library\n";
            message += "photoPIR32\n\n";
            message += "/subscribe : Añadirse a la lista\n";
            //Creamos una tarea para que se envíe el mensaje
            outCommAddTask(0,msg.chat_id,message,defaultKeyb);
          }
          else if(msg.text=="/subscribe")
          {
            //El primer usuario en suscribirse
            subscriptionCodeGenerator="él mismo";
            subscribe(&msg);
          }
        }
        else //Hay algún suscriptor
        {
          //Si el autor es suscriptor...
          if(subscribers.containsKey(msg.chat_id))
          {
            //Podemos hacerle caso
            //Si se trata del comando /start...
            if(msg.text=="/start")
            {
              String message = "RoJo Telegram Bot library\n";
              message += "photoPIR32\n\n";
              message += "/on : Activa el detector\n";
              message += "/off : Desactiva el detector\n";
              message += "/photo : toma foto\n";
              message += "/resX : seleccionar resolución\n";
              message += "/info : información\n";
              message += "/users : gestión de usuarios\n";
              message += "/config : configuración\n";
              message += "/subscribe code : Añadirse a la lista\n";
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,defaultKeyb);
            }
            else if(msg.text=="/status")
            {
              //Mostramos el estado actual
              String message = "Sensor de movimiento activo: "+yesno[PIRenabled];
              message+="\nError de cámara: "+String(errorCodeCamera);
              message+="\nResolución actual: "+String(currentRes);
              message+="\nTareas de cámara pendientes: "+String(cameraQueue.count());
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,defaultKeyb);
            }
            else if(msg.text=="/resX")
            {
              //Sólo presentaremos las opciones y el menú de selección
              String message = "Selecciona resolución\n";
              String keyb="[[\"/res0\",\"/res1\",\"/res2\"],[\"/res3\",\"/res4\",\"/res5\"],[\"/res6\",\"/res7\",\"/res8\"]]";
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,keyb);
            }
            else if(msg.text=="/config")
            {
              //Sólo presentaremos las opciones y el menú de selección
              String message = "Configuración\n";
              message += "/reboot : Reinicia el dispositivo\n";
              message += "/format : Formatea el sistema de archivos\n";
              String keyb="[[\"/reboot\",\"/format\"]]";
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,keyb);
            }
            else if(msg.text=="/users")
            {
              //Sólo presentaremos las opciones y el menú de selección
              String message = "RoJo Telegram Bot library\n";
              message += "photoPIR32\n\n";
              message += "/generate : Genera un código de suscripción\n";
              message += "/unsubscribe : Borrarse de la lista\n";
              String keyb="[[\"/generate\",\"/unsubscribe\"]]";
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,keyb);
            }
            else if(msg.text=="/info")
            {
              //Sólo presentaremos las opciones y el menú de selección
              String message = "RoJo Telegram Bot library\n";
              message += "photoPIR32\n\n";
              message += "/status : Estado actual\n";
              message += "/subscribers : Mostrar la lista de suscriptores\n";
              String keyb="[[\"/status\",\"/subscribers\"]]";
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,message,keyb);
            }
            else if(msg.text=="/generate")
            {
              //Genera un código de suscripción que será válido durante el próximo minuto
              subscriptionCode=random(65535)+1; //Entre 1 y 65535
              //Anotamos el nombre del generador
              subscriptionCodeGenerator=msg.from_name;
              //Anotamos la hora en la que caduca. Dentro de 60 segundos
              subscriptionCodeTimeout=millis()+60000;
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,"Código: " + String(subscriptionCode),defaultKeyb);
            }
            else if(msg.text=="/unsubscribe")
            {
              //El usuario actual quiere eliminar la suscripción
              subscribers.remove(msg.chat_id);
              //Creamos una tarea para que se envíe el mensaje al antiguo suscriptor
              outCommAddTask(0,msg.chat_id,"Suscripción borrada","");
              //Creamos una tarea para que se envíe el mensaje informando a todos
              outCommAddTask(0,"0",msg.from_name + " se ha dado de baja",defaultKeyb);
            }
            else if(msg.text=="/subscribers")
            {
              //Solicitan la lista de suscriptores
              String message="";
              //Recorremos todos los suscriptores...
              for(uint16_t i=0;i<subscribers.count();i++)
              {
                //Si no es el primero...añadimos una coma
                if(message.length()) message+=",";
                //Añadimos el nombre
                message+=subscribers.value(i);
              }
              //Creamos una tarea para que se envíe el mensaje
              outCommAddTask(0,msg.chat_id,"Suscriptores: "+message,defaultKeyb);
            }
            else if(msg.text=="/reboot")
            {
              //Solicitan el reinicio del dispositivo

              //Informamos a todos
              outCommAddTask(0,"0",msg.from_name+" solicita reiniciar",defaultKeyb);
              //Desactivamos todos los sistemas
              systemsDown();
              //Ya podemos resetear el dispositivo
              ESP.restart();
            }
              else if(msg.text=="/format")
            {
              //Solicitan el formateo de SPIFFS

              //Informamos a todos
              outCommAddTask(0,"0",msg.from_name+" solicita formatear",defaultKeyb);
              //Desactivamos todos los sistemas
              systemsDown();

              //Si formateamos SPIFFS perderemos todo su contenido
              //Hay tres archivos importantes que deberíamos salvar:
              // - config.txt que guarda la configuración actual (estado, conexión a wifi & botToken)
              // - RoJoArduCAM.dat con las secuencias de configuración de la cámara
              // - subscribers.txt con el listado de suscriptores
              //El archivo config.txt lo puede volver a generar automáticamente la función saveStatus()
              //El archivo subscribers.txt se perderá (a propósito)
              //El archivo RoJoArduCAM.dat es imprescindible para que la cámara funcione.
              //Tiene un tamaño de casi 2000 bytes.
              //No es demasiado. Intentaremos guardarlo en memoria para regenerarlo después
              File datFile=SPIFFS.open(F("/RoJoArduCAM.dat"),"r"); //Abrimos el archivo como lectura
              uint16_t datSize=datFile.size(); //Anotamos el tamaño del archivo
              byte *datMem; //Puntero a array de bytes en memoria
              datMem=new byte[datSize]; //Reservamos memoria suficiente para todo el archivo
              datFile.readBytes((char *)datMem,datSize); //Leemos el archivo completo en memoria
              datFile.close(); //Cerramos el archivo
              //Formateamos SPIFFS. Dependiendo del tamaño de SPIFFS tarda más o menos.
              SPIFFS.format();
              //Guardamos de nuevo la configuración actual
              saveConfig();
              //Regeneramos el archivo RoJoArduCAM.dat
              datFile=SPIFFS.open(F("/RoJoArduCAM.dat"),"w"); //Abrimos el archivo como escritura
              datFile.write(datMem,datSize); //Escribimos el contenido del archivo
              datFile.close(); //Cerramos el archivo
              delete[] datMem; //Liberamos la memoria
              //Hemos regenerado los archivo status.txt y RoJoArduCAM.dat
              //Reseteamos el dispositivo
              delay(100);
              ESP.restart();
            }
            else if(msg.text=="/photo")
            {
              //Solicitan sacar una foto

              //Creamos una tarea para que la cámara saque una foto
              cameraAddTask(9,msg.chat_id,"","");
            }
            else if(msg.text=="/on")
            {
              //Se activa el detector de movimento
              PIRenabled=true;
              //Guardamos configuración actual
              saveConfig();
              //Creamos una tarea para que se envíe el mensaje informando a todos
              outCommAddTask(0,"0",msg.from_name + " ha activado el detector",defaultKeyb);
            }
            else if(msg.text=="/off")
            {
              //Se desactiva el detector de movimento
              PIRenabled=false;
              //Guardamos configuración actual
              saveConfig();
              //Creamos una tarea para que se envíe el mensaje informando a todos
              outCommAddTask(0,"0",msg.from_name + " ha desactivado el detector",defaultKeyb);
            }
            else //No reconocemos el comando
            {
              //Intentaremos reconocer si es un cabio de resolución
              //Variable para anotar si el comando es de cambio de resolución. Inicialmente no
              bool changeResCommand=false;
              //Si el comando tiene una longitud de 5...
              if(msg.text.length()==5)
              {
                //...si el comando comienza por /res...
                if(msg.text.substring(0,4)=="/res")
                {
                  //...si el último carácter es numérico...
                  String n=msg.text.substring(4);
                  if(n>="0" && n<="8")
                  {
                    //Creamos tarea para el cambio de resolución y que informe a todos
                    cameraAddTask(n[0]-48,"0",msg.from_name,defaultKeyb);
                    //Es un comando de cambio de resolución
                    changeResCommand=true;
                  }
                }
              }
              //Si no se trataba de un comando de cambio de resolución...
              if(!changeResCommand)
              {
                //...entonces si que no reconocemos el comando
                //Enviamos el mensaje a todos los suscriptores
                outCommAddTask(0,"0",msg.from_name + " dijo: " + msg.text,defaultKeyb);
              }
            }
          }
          else //El autor no es suscriptor
          {
            //Si quiere suscribirse...
            if(msg.text.substring(0,11)=="/subscribe ")
            {
              //Si tenemos algún código válido...
              if(subscriptionCode)
              {
                //Si el código no ha caducado
                if(millis()<subscriptionCodeTimeout)
                {
                  //Si el código indicado es correcto...incluimos el nuevo usuario como suscriptor
                  if(msg.text.substring(11)==String(subscriptionCode)) subscribe(&msg);
                  //Si el código indicado no es el correcto...le informamos
                  else outCommAddTask(0,msg.chat_id,"Código incorrecto","");
                }
                //Desactivamos el código por alguna de las siguientes razones:
                //- Ha caducado
                //- Ha sido utilizado
                //- Se ha introducido un código incorrecto
                subscriptionCode=0;
              }
            }
          }
        }
      }
      //Aumentamos el tiempo de espera para el siguiente ciclo
      currentWait*=factorWait;
      //Si sobrepasamos el máximo...lo limitamos
      if(currentWait>maxWait) currentWait=maxWait;
      //Esperamos el tiempo calculado
      delay(currentWait);
    }
    //Evita el error de watchdog cuando no se hace nada en un ciclo
    delay(1);
  }
}

void interruptPIR()
{
  //Función de gestión de interrupciones del sensor de movimiento
  //Se ha detectado movimiento
  //Si el detector está activo...pedimos a la cámara que saque una foto para que se envíe a todos
  if(PIRenabled) cameraAddTask(9,"0","","");
}

void try2connect()
{
  //Intenta conectar al punto de acceso wifi que nos permite usar Internet

  //Si no tenemos nombre de punto de acceso...hemos terminado
  if(wifiClientSSID.length()==0) return;
  //Si nunca hemos intentado conectar o
  //hace más de 20 segundos que lo estamos intentando...
  if(wifiLastTry==0 || millis()-wifiLastTry>20000)
  {
    //...lo intentamos de nuevo
    //Inicializamos conexión wifi con las credenciales actuales
    //Necesitamos convertir los Strings en arrays de char
    WiFi.begin(wifiClientSSID.c_str(),wifiClientPassword.c_str());
    //Acabamos de intentar conectar ahora mismo
    wifiLastTry=millis();
  }
  //Si ya tenemos conexión...lo anotamos
  wifiConnected=(WiFi.waitForConnectResult()==WL_CONNECTED);
}

void getRequestParameters(WiFiClient *client,RoJoList<String> *requestParams)
{
  //Dato un cliente, obtenemos su solicitud y descomponemos los parámetros en una lista

  //La primera línea de la petición de un cliente tiene la siguiente sintaxis:
  //  GET / HTTP/1.1
  //El path / puede ser más complejo que sólo el raíz:
  //  GET /firstLevel HTTP/1.1
  //O puede indicar algún archivo concreto:
  //  GET /firstLevel/index.html HTTP/1.1
  //También puede contener parámetros:
  //  GET /?p0=miPuntoDeAcceso&p1=myPassword&p2=123456789:ABCDEFGHIJK HTTP/1.1
  //La petición puede tener más líneas en las que se especifican detalles del servidor, tipo de browser
  //utilizado, tipo de seguridad en la conexión, tipos de codificación, tipos de archivo admitidos,
  //lenguaje, etc.

  //Leemos la primera línea de la solicitud recibida
  String requestHeader=""; //Variable en la que guardaremos la primera linea de la solicitud
  bool lineEnd=false; //Inicialmente no hemos terminado de leer la primera línea
  //Mientras haya información pendiente de recibir y no hayamos completado la primera línea...
  while(client->available() && !lineEnd)
  {
    //Anotamos el siguiente carácter
    char c=client->read();
    //Si en un fin de línea...hemos terminado de leer la primera línea
    if(c=='\n') lineEnd=true;
    //Si no es un fin de línea...lo añadimos a los caracteres de la línea
    else requestHeader+=c;
  }
  //Hacemos tiempo asegurándonos que la lista de parámetros está vacía
  requestParams->clear();
  //Despreciamos el resto de la solicitud
  //No utilizamos client->flush() porque después de descargar el buffer, detiene la comexión
  while(client->available()) client->read();
  //Tenemos la primera línea de la solicitud
  //Sólo nos interesan los parámetros
  //Intentamos localizar el principio de la los parámetros
  //Anotamos la posición del inicio de los parámetros '?'
  int pos=requestHeader.indexOf('?');
  //Si no se han encontrado parámetros...hemos terminado!
  if(pos<0) return;
  //Se han encontrado parámetros
  //Recortamos el prefijo
  requestHeader=requestHeader.substring(pos+1);
  //Localizamos el final de los parámetros ' '
  pos=requestHeader.indexOf(' ');
  //Si no se ha encontrado el final...hemos terminado!
  if(pos<0) return;
  //Hemos encontrado el final de los parámetros
  //Recortamos el sufijo
  requestHeader=requestHeader.substring(0,pos);
  //Formato actual de parámetros:
  // p0=miPuntoDeAcceso&p1=myPassword&p2=123456789:ABCDEFGHIJK
  //Procesamos la cadena mientras haya algo que procesar
  while(requestHeader.length())
  {
    //Localizamos el separador del nombre del parámetro y su valor
    pos=requestHeader.indexOf('=');
    //Si no se ha encontrado el separador...hemos terminado!
    if(pos<0) requestHeader="";
    else //Separador localizado
    {
      //Recortamos prefijo
      requestHeader=requestHeader.substring(pos+1);
      //Localizamos inicio de siguiente parámetro
      pos=requestHeader.indexOf('&');
      //Puntero a valor a guardar
      String *paramString;
      //Si no se ha encontrado...
      if(pos<0)
      {
        //...es porque este es el último parámetro
        //El parámetro será el resto de la línea
        paramString=new String(requestHeader);
        //No tenemos nada más para procesar
        requestHeader="";
      }
      else //Hemos encontrado más parámetros
      {
        //Anotamos el valor hasta antes del siguiente parámetro
        paramString=new String(requestHeader.substring(0,pos));
        //Dejaremos el siguiente parámetro
        requestHeader=requestHeader.substring(pos+1);
      }
      //Guardamos el parámetro en la lista
      requestParams->add2end(paramString);
    }
  }
  //Hemos terminado de procesar la línea de parámetros
}

String htmlConfig()
{
  //Compone una respuesta correcta en HTML con la página de configuración
  //La cabecera de una respuesta correcta de HTML es:
  
  //HTTP/1.1 200 OK
  //Content-Type: text/html; charset=utf-8
  //

  //Es importante notar que tras la última línea de la cabecera siempre va una línea vacía
  //Después utilizamos la siguiente plantilla para mostrar el contenido:
  
  //<!DOCTYPE html><html>
  //<h1>Configuración photoPIR32</h1>
  //<p><label>Wifi client SSID:<input id="wifiSSID" type="text" name="SSID" value="var1"/></label></p>
  //<p><label>Wifi client password:<input id="wifiPassword" type="text" name="password" value="var2" /></label></p>
  //<p><label>Telegram bot token:<input id="botToken" type="text" name="botToken" value="var3" size="50"/></label></p>
  //<p><a href="?" onclick="location.href=this.href+'p0='+wifiSSID.value+'&p1='+wifiPassword.value+'&p2='+botToken.value;return false;"><button>Ok</button></a>
  //<br><br><h1>Estado actual</h1>
  //<p>Sensor de movimiento activo: no
  //<br>Error de la cámara: 0
  //<br>Resolución: 3
  //<br>Conectado a wifi: sí
  //<br>Tareas de cámara pendientes: 0
  //<br>Tareas de salida pendientes: 1</p>
  //<p><a href=\"/\"><button>Refrescar</button></a></p>
  //</html>

  //Se sustituyen los valores de los campos por los valores de la configuración actual
  String answer="HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n\n";
  answer+="<!DOCTYPE html><html><h1>Configuración photoPIR32</h1>";
  answer+="<p><label>Wifi client SSID:<input id=\"wifiSSID\" type=\"text\" name=\"SSID\" value=\"" + wifiClientSSID +"\"/></label></p>";
  answer+="<p><label>Wifi client password:<input id=\"wifiPassword\" type=\"text\" name=\"password\" value=\"" + wifiClientPassword +"\" /></label></p>";
  answer+="<p><label>Telegram bot token:<input id=\"botToken\" type=\"text\" name=\"botToken\" value=\"" + botToken + "\" size=\"50\"/></label></p>";
  answer+="<p><a href=\"?\" onclick=\"location.href=this.href+'p0='+wifiSSID.value+'&p1='+wifiPassword.value+'&p2='+botToken.value;return false;\"><button>Ok</button></a></p>";
  
  answer+="<br><br><h1>Estado actual</h1>";
  answer+="<p>Sensor de movimiento activo: "+yesno[PIRenabled];
  answer+="<br>Error de la cámara: " + String(errorCodeCamera);
  answer+="<br>Resolución: " + String(currentRes);
  answer+="<br>Conectado a wifi: " + yesno[wifiConnected];
  answer+="<br>Tareas de cámara pendientes: " +  String(cameraQueue.count());
  answer+="<br>Tareas de salida pendientes: " +  String(outCommQueue.count()) + "</p>";
  answer+="<p><a href=\"/\"><button>Refrescar</button></a></p>";
  answer+="</html>";

  //Devolvemos la cadena HTML
  return answer;
}

void serverHandleClient()
{
  //Comprueba si hay conexiones pendientes de atender en el servidor web local

  //Si no hay clientes conectados...hemos terminado
  if(!server.hasClient()) return;
  //Hay algún cliente conectado
  //Tomamos nota de quién es
  WiFiClient client=server.available();
  //Creamos una lista para guardar los parametros de la solicitud
  RoJoList<String> requestParams;
  //Leemos la solicitud y descomponemos sus parámetros en la lista
  getRequestParameters(&client,&requestParams);
  //Para que un cambio de configuración sea aceptado debe tener tres
  //parámetros con los siguientes datos:
  //  1. wifiClientSSID
  //  2. wifiClientPassword
  //  3. botToken
  //Si tiene 3 parámetros...
  if(requestParams.count()==3)
  {
    //Informamos a todos
    outCommAddTask(0,"0","Aplicando nueva configuración",defaultKeyb);
    //Creamos puntero para obtener los parámetros
    String *param;
    //Cambiamos los valores de las variables
    requestParams.index(&param,0); //Parámetro 0 = SSID
    wifiClientSSID=*param;
    requestParams.index(&param,1); //Parámetro 1 = password
    wifiClientPassword=*param;
    requestParams.index(&param,2); //Parámetro 2 = botToken;
    botToken=*param;
    //Guardaremos los valores actuales en el archivo de configuración
    saveConfig();
    //En vez de actualizar el botToken con: bot.begin(botToken);
    //Y resetear las comunicaciones con:
    //  wifiLastTry=0; //Nunca antes lo habíamos intentado
    //  wifiConnected=false; //Actualmente no estamos conectados
    //Simplemente llamaremos a la función de lectura que hace el proceso completo
    readConfig();
  }
  //Se haya modificado la configuración o no, respondemos con la misma página
  client.println(htmlConfig());
  //Esperamos a que se termine de enviar el texto HTML
  delay(100);
  //Cerramos conexión con este cliente
  client.stop();
}

void setup()
{
  //Desactivamos los mensajes de error de "Brownout detector was triggered"
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  //Apagamos el led integrado
  pinMode(pinLed,OUTPUT);
  digitalWrite(pinLed,!ledON);
  //Inicializamos el sistema de archivos
  SPIFFS.begin();
  //Recuperamos la última configuración
  readConfig();
  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Activamos el gestor de comunicaciones de salida
  //- outCommManager: nombre de la función
  //- "": nombre de la tarea (no importa)
  //- 6000: tamaño de pila (4500 es insuficiente y 4600 es ok)
  //- NULL: puntero a parámetros
  //- 1: prioridad baja
  //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
  //- 0: core en el que se ejecuta
  xTaskCreatePinnedToCore(outCommManager,"",6000,NULL,1,NULL,0);
  //Activamos el gestor de la cámara
  //- cameraManager: nombre de la función
  //- "": nombre de la tarea (no importa)
  //- 5000: tamaño de pila (3500 es insuficiente y 3800 es ok)
  //- NULL: puntero a parámetros
  //- 1: prioridad baja
  //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
  //- 0: core en el que se ejecuta
  xTaskCreatePinnedToCore(cameraManager,"",5000,NULL,1,NULL,0);
  //Activamos el gestor de comunicaciones de entrada
  //- inCommManager: nombre de la función
  //- "": nombre de la tarea (no importa)
  //- 7000: tamaño de pila (5000 es insuficiente y 5500 es ok)
  //- NULL: puntero a parámetros
  //- 1: prioridad baja
  //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
  //- 1: core en el que se ejecuta
  xTaskCreatePinnedToCore(inCommManager,"",7000,NULL,1,NULL,1);
  //Configuramos el pin del sensor de movimiento como entrada
  pinMode(pinPIR,INPUT);
  //Activamos las interrupciones para el pin del sensor de movimiento
  attachInterrupt(pinPIR,interruptPIR,RISING);
  //Configuramos el wifi en modo mixto
  //Se conectará a un punto de acceso y al mismo tiempo creará su propio punto de acceso
  //La conexión cliente servirá para salir a Internet
  //La conexión servidor servirá para cambiar la configuración
  WiFi.mode(WIFI_AP_STA);
  //Configuramos el punto de acceso wifi propio
  //Necesitamos convertir los Strings en arrays de char
  WiFi.softAP(mySSID.c_str(),mySSIDpassword.c_str()); //Nombre y contraseña
  //Arrancamos el servidor web local para cambio de configuración
  server.begin();
  //Informamos a todos los suscriptores que se ha reiniciado
  outCommAddTask(0,"0","Dispositivo reiniciado",defaultKeyb);
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
}

void loop()
{
  //Gestión de microservicios

  if(!wifiConnected) try2connect(); //Conexión wifi
  if(errorCodeCamera) errorCodeCamera=camera.begin(pinCS); //Inicio de cámara
  serverHandleClient(); //Refresca conexiones del servidor web local
  delay(1000);
}