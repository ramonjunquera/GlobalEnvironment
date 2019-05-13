/*
  Autor: Ramón Junquera
  Versión: 20190513
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Enviar una imagen al detectar movimiento
  Material: placa ESP32, ArduCAM-Mini-2MP, PIR sensor
  Descripción:
    Sistema de videovigilancia con control por Telegram.
    El sistema consta de una placa ESP32, una cámara y un sensor de movimiento (ya sea infrarrojo
    o por microondas).
  Multitarea:  
    Aprovecharemos que el ESP32 tiene 2 cores para intentar trabajar en paralelo.
    El programa principal siempre corre en el core 1.
    Distribuiremos los procesos de la siguiente manera:
      Core 0:
        Gestión de cámara
        Gestión de comunicaciones de salida
      Core 1:
        Gestión de comunicaciones de entrada
        Interrupción PIR
        loop principal : microservicios
          Conexión wifi
          Inicialización de la cámara
    Los módulos de gestión (de cámara y de envíos de Telegram) se hacen en base a una
    lista de tareas pública.
    Cada elemento de la lista contiene una estructura con todos los datos necesarios para trabajar.
    Las nuevas peticiones se añaden siempre al final y se procesan desde el principio (FIFO).
  Configuración del programa:
    La primera sección del programa es la definición de constantes globales utilizadas
    para la configuración.
  Sistema de obtención de nuevos mensajes:
    La librería de gestión de Telegram nos permite trabajar con webhook.
    Tenemos dos métodos para conseguir nuevos mensajes:
    - Polling
      La aplicación hace comprobaciones periodicas al servidor de Telegram, preguntando por
      nuevos mensajes. La mayoría de las veces la respuesta será que no hay mensajes nuevos,
      pero aún así, es imprescindible seguir comprobando y gastando tiempo y datos.
    - webhook
      Se puede le puede pedir al servidor de Telegram que cuando tenga un nuevo mensaje para
      nuestro bot, se conecte a cierta dirección en la cuál encontrará un servidor web
      seguro que atenderá su llamada y que contendrá los detalles del mensaje.
  Menús:
    General
      /on /off
      /photo /resX
      /info /users /reboot
    /resX
      /res0 /res1 /res2
      /res3 /res4 /res5
      /res6 /res7 /res8
    /info
      /status /subscribers
    /users
      /generate /unsubscribe
  Primer uso:
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
    Depende del sistema de obtención de nuevos mensajes.
    Si es por webhook, tarda un par de segundos. Siempre lo mismo, porque sólo dependemos
    de recibir el mensaje del servidor de Telegram.
    Si se utiliza el sistema de polling, el tiempo de respuesta variable, puesto que el tiempo
    entre dos comprobaciones se va alargando si no recibe nuevas peticiones, hasta alcanzar
    un máximo de 30 segundos (definido en la constante maxWait)
  Menú resX:

    Permite cambiar la resolución entre 9 opciones.

    La /res0 es la más baja y las /res8 la más alta.

  Menú /users:

    Permite:
      - Generar un código para una invitación
          Los nuevos suscriptores (invitado) siempre deben ser invitados por alguien suscrito
          (anfitrión).
          El anfitrión debe generar un código a través del botón /users y botón /generate.
          El código tiene una validez de 60 segundos. Antes de este tiempo el invitado debe
          enviar al bot el comando /subscribe seguido del código.
          Si se escribe mal el código se tendrá que generar uno nuevo.
      - Darse de baja como suscriptor
  Chat:
    Si un suscriptor envía al bot mensajes que no son reconocidos como comandos, se supondrá que 
    es texto normal y se le enviará el mensaje a todos los suscriptores.
  webhook:
    Aunque la librería de Telegram permita trabajar con webhook, la activación y desactivación
    es manual.
    Ejemplo de activación de webhook:
      curl -F "url=https://myserver.mydomain:8443/" -F "certificate=@autocert.crt" -F "max_connections=4" https://api.telegram.org/bot<botId>/setWebhook
      Donde <botId> se debe sustituir por el identificador de nuestro bot
      El comando debe ejecutarse desde la carpeta que contiene el archivo del certificado
      Fijamos el 4 el número de conexiones simultáneas porque se pueden procesar simultáneamente
    Ejemplo de desactivación de webhook:
      https://api.telegram.org/bot<botId>/setWebhook
      Se puede lanzar desde un browser

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
#include "RoJoArduCAM.h" //Librería de gestión de la cámara
#include "RoJoList.h" //Librería de gestión de listas para los parámetros de una solicitud web
#include "RoJoTelegramBot32.h" //Librería para gestión de bots de Telegram
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

//Configuración general
const String configFile="/config.txt";
const byte pinPIR=15; //Pin del sensor PIR
const byte pinCS=4; //Pin CS de la cámara
//Configuración Telegram
const String botToken="xxx";
//Configuración wifi
const String wifiClientSSID="xxx";
const String wifiClientPassword="xxx";
//Configuración para polling
const uint32_t maxWait=30000; //Tiempo máximo de espera para consultar nuevos mensajes = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera
const uint32_t startWait=1+1/(factorWait-1); //Valor inicial (mínimo) para el tiempo de espera
//Configuración para webhook
const String secureServerCertificateFile="/autocert.crt";
const String secureServerPrivateKeyFile="/private.key";
const uint16_t webhookPort=8443;
//Configuración de método de obtención de mensajes
const bool useWebhook=true; //Utiliza webhook o pulling?
//Constantes de aplicación
const String defaultKeyb="[[\"/on\",\"/off\"],[\"/photo\",\"/resX\"],[\"/info\",\"/users\",\"/reboot\"]]"; //Teclado por defecto
const String yesno[2]={"no","sí"};

//Objetos globales
RoJoTelegramBot32 bot; //Gestión del bot de Telegram
RoJoFileDictionary subscribers;
RoJoList<outCommQueueStruct> outCommQueue; //Lista de tareas pendientes de mensajes de salida
RoJoList<cameraQueueStruct> cameraQueue; //Lista de tareas pendiente de la cámara
RoJoArduCAM camera;

//Parámetros de configuración
byte currentRes=3; //Código de resolución actual
bool PIRenabled = false; //PIR activado?

//Variables globales
bool outCommQueueStopRequest=false; //Solicitada parada de comunicaciones de salida?
bool outCommQueueRunning=true; //Están funcionando las comunicaciones de salida?
bool cameraQueueStopRequest=false; //Solicitada parada de gestión de cámara?
bool cameraQueueRunning=true; //Está funcionando la gestión de la cámara?
bool inCommQueueStopRequest=false; //Solicitada parada de comunicaciones de entrada?
bool inCommQueueRunning=true; //Están funcionando las comunicaciones de entrada?
String subscriptionCodeGenerator=""; //Nombre del generador del código de suscripción
uint16_t subscriptionCode=0; //Código de suscripción
uint32_t subscriptionCodeTimeout=0; //millis de caducidad del código de suscripción

void saveConfig()
{
  //Guarda la configuración actual (resolución y estado)
  //en un archivo en SPIFFS

  //Abrimos el archivo del estado para escritura
  File f=SPIFFS.open(configFile,"w");
  //Si no se ha podido abrir el archivo...hemos terminado
  if(!f) return;
  //Línea 1: resolución
  f.println(currentRes);
  //Línea 2: estado
  f.println(PIRenabled);
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

  //El archivo debe tener 3 líneas
  //  línea 1: resolución
  currentRes=readLine(f).toInt();
  //  línea 2: estado
  PIRenabled=readLine(f).toInt();
  //Cerramos el archivo
  f.close();
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
    //Si la gestión de mensages de salida está funcionando...
    if(outCommQueueRunning)
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
    //Si tenemos alguna tarea y la cola de tareas está activa...
    if(cameraQueue.count()>0 && cameraQueueRunning==true)
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
          String message=pCameraTask->from_name+" cambia a resolución "+String(currentRes);
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
  //Desactiva todos los sistemas en el siguiente orden
  //1- interrupción de PIR
  //2- gestión de mensajes de entrada
  //3- gestión de cámara
  //4- gestión de mensajes de salida

  //Variable para guardar un mensaje entrante de Telegram
  TelegramMessage msg;
  
  //Desactivamos las interrupciones del sensor PIR
  detachInterrupt(pinPIR);
  //Solicitamos desactivar la gestión de los mensajes de entrada
  inCommQueueStopRequest=true;
  //Esperamos a que se complete
  while(inCommQueueRunning) {delay(1);};
  //Si no estamos utilizando webhook...
  if(!useWebhook)
  {
    //Leemos el siguiente mensaje para que al reiniciar no recibamos de nuevo el comando de restart
    bot.getNextMessage(&msg);
  }
  //Solicitamos desactivar la gestión de la cámara
  cameraQueueStopRequest=true;
  //Esperamos a que se complete
  while(cameraQueueRunning) {delay(1);};
  //Solicitamos desactivar la gestión de los mensajes de salida
  outCommQueueStopRequest=true;
  //Esperamos a que se complete
  while(outCommQueueRunning) {delay(1);};
}

void receivedMsgManager(TelegramMessage *msg)
{
  //Procesa el mensaje recibido

  //Si no hay suscriptores...
  if(subscribers.count()==0)
  {
    //Si se trata del comando /start...
    if(msg->text=="/start")
    {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n";
      message += "photoPIR32\n\n";
      message += "/subscribe : Añadirse a la lista\n";
      //Creamos una tarea para que se envíe el mensaje
      outCommAddTask(0,msg->chat_id,message,defaultKeyb);
    }
    else if(msg->text=="/subscribe")
    {
      //El primer usuario en suscribirse
      subscriptionCodeGenerator="él mismo";
      subscribe(msg);
    }
  }
  else //Hay algún suscriptor
  {
    //Si el autor es suscriptor...
    if(subscribers.containsKey(msg->chat_id))
    {
      //Podemos hacerle caso
      //Si se trata del comando /start...
      if(msg->text=="/start")
      {
        String message = "RoJo Telegram Bot library\n";
        message += "photoPIR32\n\n";
        message += "/on : Activa el detector\n";
        message += "/off : Desactiva el detector\n";
        message += "/photo : toma foto\n";
        message += "/resX : seleccionar resolución\n";
        message += "/info : información\n";
        message += "/users : gestión de usuarios\n";
        message += "/reboot : reinicio\n";
        message += "/subscribe code : Añadirse a la lista\n";
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,message,defaultKeyb);
      }
      else if(msg->text=="/status")
      {
        //Mostramos el estado actual
        String message = "Sensor de movimiento activo: "+yesno[PIRenabled];
        message+="\nResolución actual: "+String(currentRes);
        message+="\nTareas de cámara pendientes: "+String(cameraQueue.count());
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,message,defaultKeyb);
      }
      else if(msg->text=="/resX")
      {
        //Sólo presentaremos las opciones y el menú de selección
        String message = "Selecciona resolución\n";
        String keyb="[[\"/res0\",\"/res1\",\"/res2\"],[\"/res3\",\"/res4\",\"/res5\"],[\"/res6\",\"/res7\",\"/res8\"]]";
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,message,keyb);
      }
      else if(msg->text=="/users")
      {
        //Sólo presentaremos las opciones y el menú de selección
        String message = "RoJo Telegram Bot library\n";
        message += "photoPIR32\n\n";
        message += "/generate : Genera un código de suscripción\n";
        message += "/unsubscribe : Borrarse de la lista\n";
        String keyb="[[\"/generate\",\"/unsubscribe\"]]";
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,message,keyb);
      }
      else if(msg->text=="/info")
      {
        //Sólo presentaremos las opciones y el menú de selección
        String message = "RoJo Telegram Bot library\n";
        message += "photoPIR32\n\n";
        message += "/status : Estado actual\n";
        message += "/subscribers : Mostrar la lista de suscriptores\n";
        String keyb="[[\"/status\",\"/subscribers\"]]";
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,message,keyb);
      }
      else if(msg->text=="/generate")
      {
        //Genera un código de suscripción que será válido durante el próximo minuto
        subscriptionCode=random(65535)+1; //Entre 1 y 65535
        //Anotamos el nombre del generador
        subscriptionCodeGenerator=msg->from_name;
        //Anotamos la hora en la que caduca. Dentro de 60 segundos
        subscriptionCodeTimeout=millis()+60000;
        //Creamos una tarea para que se envíe el mensaje
        outCommAddTask(0,msg->chat_id,"Código: " + String(subscriptionCode),defaultKeyb);
      }
      else if(msg->text=="/unsubscribe")
      {
        //El usuario actual quiere eliminar la suscripción
        subscribers.remove(msg->chat_id);
        //Creamos una tarea para que se envíe el mensaje al antiguo suscriptor
        outCommAddTask(0,msg->chat_id,"Suscripción borrada","");
        //Creamos una tarea para que se envíe el mensaje informando a todos
        outCommAddTask(0,"0",msg->from_name + " se ha dado de baja",defaultKeyb);
      }
      else if(msg->text=="/subscribers")
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
        outCommAddTask(0,msg->chat_id,"Suscriptores: "+message,defaultKeyb);
      }
      else if(msg->text=="/reboot")
      {
        //Solicitan el reinicio del dispositivo

        //Informamos a todos
        outCommAddTask(0,"0",msg->from_name+" solicita reiniciar",defaultKeyb);
        //Desactivamos todos los sistemas
        systemsDown();
        //Ya podemos resetear el dispositivo
        ESP.restart();
      }
      else if(msg->text=="/photo")
      {
        //Solicitan sacar una foto

        //Creamos una tarea para que la cámara saque una foto
        cameraAddTask(9,msg->chat_id,"","");
      }
      else if(msg->text=="/on")
      {
        //Se activa el detector de movimento
        PIRenabled=true;
        //Guardamos configuración actual
        saveConfig();
        //Creamos una tarea para que se envíe el mensaje informando a todos
        outCommAddTask(0,"0",msg->from_name + " ha activado el detector",defaultKeyb);
      }
      else if(msg->text=="/off")
      {
        //Se desactiva el detector de movimento
        PIRenabled=false;
        //Guardamos configuración actual
        saveConfig();
        //Creamos una tarea para que se envíe el mensaje informando a todos
        outCommAddTask(0,"0",msg->from_name + " ha desactivado el detector",defaultKeyb);
      }
      else //No reconocemos el comando
      {
        //Intentaremos reconocer si es un cabio de resolución
        //Variable para anotar si el comando es de cambio de resolución. Inicialmente no
        bool changeResCommand=false;
        //Si el comando tiene una longitud de 5...
        if(msg->text.length()==5)
        {
          //...si el comando comienza por /res...
          if(msg->text.substring(0,4)=="/res")
          {
            //...si el último carácter es numérico...
            String n=msg->text.substring(4);
            if(n>="0" && n<="8")
            {
              //Creamos tarea para el cambio de resolución y que informe a todos
              cameraAddTask(n[0]-48,"0",msg->from_name,defaultKeyb);
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
          outCommAddTask(0,"0",msg->from_name + " dijo: " + msg->text,defaultKeyb);
        }
      }
    }
    else //El autor no es suscriptor
    {
      //Si quiere suscribirse...
      if(msg->text.substring(0,11)=="/subscribe ")
      {
        //Si tenemos algún código válido...
        if(subscriptionCode)
        {
          //Si el código no ha caducado
          if(millis()<subscriptionCodeTimeout)
          {
            //Si el código indicado es correcto...incluimos el nuevo usuario como suscriptor
            if(msg->text.substring(11)==String(subscriptionCode)) subscribe(msg);
            //Si el código indicado no es el correcto...le informamos
            else outCommAddTask(0,msg->chat_id,"Código incorrecto","");
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
  //Temos terminado de procesar el mensaje
}

void webhookReceivedMsg(void *parameter)
{
  //Función a la que se llamará como thread cuando se reciba un mensaje de Telegram
  //El parámetro corresponde a un mensaje de Telegram

  //Recuperamos su tipo correcto
  TelegramMessage *msg=(TelegramMessage*)parameter;
  //Llamamos a la función que procesa los mensajes
  receivedMsgManager(msg);
  //Borramos el mensaje
  delete msg;
  //Borramos la tarea actual
  vTaskDelete(NULL);
}

void inCommManager(void *parameter)
{
  //Gestión de comunicaciones de entrada por consulta

  //Variable para guardar un mensaje entrante de Telegram
  TelegramMessage msg;
  //Tiempo de espera actual en milisegundos
  uint32_t currentWait=startWait;

  //Bucle infinito
  while(true)
  {
    //Si la gestión de mensajes de entrada está activa...
    if(inCommQueueRunning)
    {
      //Obtenemos el siguiente mensaje
      bot.getNextMessage(&msg);
      //Si tenemos algún mensaje...
      if(msg.text.length())
      {
        //Reseteamos el tiempo de espera
        currentWait=startWait;
        //Procesamos el mensaje
        receivedMsgManager(&msg);
      }
      //Aumentamos el tiempo de espera para el siguiente ciclo
      currentWait*=factorWait;
      //Si sobrepasamos el máximo...lo limitamos
      if(currentWait>maxWait) currentWait=maxWait;
      //Esperamos el tiempo calculado
      delay(currentWait);
    }
    //Si hos han pedido que paremos...
    if(inCommQueueStopRequest)
    {
      inCommQueueRunning=false; //Ya no procesaremos más tareas
      inCommQueueStopRequest=false; //Hemos procesado la petición de stop
    }
    delay(1); //Para evitar errores por WatchDog
  }
}

void webhookManager(void *parameter)
{
  //Gestión de comunicaciones de entrada por webhook

  //Bucle infinito para que continuamente llama al
  //refresco de webhook. Este método comprueba y 
  //gestiona nuevas conexiones cliente.
  //- Comprueba nuevas conexiónes cliente
  //- Envía cada una de ella a un thread, dentro de la misma clase
  //- Comprueba si contiene un mensaje válido
  //- Confirma recepción
  //- Envía el mensaje estructurado a la función de usuario

  //Bucle infinito
  while(true)
  {
    //Si la gestión de mensajes de entrada está activa...refrescamos webhook
    if(inCommQueueRunning) bot.webhookRefresh();
    //Si hos han pedido que paremos...
    if(inCommQueueStopRequest)
    {
      inCommQueueRunning=false; //Ya no procesaremos más tareas
      inCommQueueStopRequest=false; //Hemos procesado la petición de stop
    }
    delay(1); //Para evitar errores por WatchDog
  }
}

void interruptPIR()
{
  //Función de gestión de interrupciones del sensor de movimiento
  //Se ha detectado movimiento
  //Si el detector está activo...pedimos a la cámara que saque una foto para que se envíe a todos
  if(PIRenabled) cameraAddTask(9,"0","","");
}

void setup()
{
  Serial.begin(115200);
  delay(1000); //Damos tiempo a que se inicialice el puerto serie

  //Desactivamos los mensajes de error de "Brownout detector was triggered"
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  //Conectamos al punto de acceso wifi
  //Configuramos el wifi en modo de conexión a un punto de acceso para salir a Internet
  WiFi.mode(WIFI_STA);
  //Inicializamos conexión wifi con las credenciales actuales
  //Necesitamos convertir los Strings en arrays de char
  WiFi.begin(wifiClientSSID.c_str(),wifiClientPassword.c_str());
  Serial.print("Connecting.");
  //Esperamos a estar conectados
  while(WiFi.waitForConnectResult()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Ok : ");
  Serial.println(WiFi.localIP());

  //Inicializamos el sistema de archivos
  SPIFFS.begin();

  //Inicializamos la cámara
  Serial.print("Init camera ");
  byte errorCode=255;
  while(errorCode>0)
  {
    errorCode=camera.begin(pinCS);
    Serial.print(errorCode);
    delay(300);
  }
  Serial.println(" ok");

  //Recuperamos la última configuración
  readConfig();
  //Inicializamos el diccionario de suscriptores
  subscribers.begin("/subscribers.txt");
  //Inicializamos el bot con el token
  bot.begin(botToken);

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

  //Si utilizamos webhook como método para obtener nuevos mensajes...
  if(useWebhook)
  {
    //Activamos el gestor de comunicaciones de entrada por webhook
    //- webhookManager: nombre de la función
    //- "": nombre de la tarea (no importa)
    //- 7000: tamaño de pila
    //- NULL: puntero a parámetros
    //- 1: prioridad baja
    //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
    //- 1: core en el que se ejecuta
    xTaskCreatePinnedToCore(webhookManager,"",7000,NULL,1,NULL,1);
    //Activamos el servidor seguro para atender webhook
    byte errorCode=bot.webhookEnable((char*)secureServerCertificateFile.c_str(),(char*)secureServerPrivateKeyFile.c_str(),webhookReceivedMsg,webhookPort,4,3000);
    Serial.println("webhook server started with error code "+String(errorCode));
  }
  else //Si utilizamos polling como metodo para obtener nuevos mensajes...
  {
    //Activamos el gestor de comunicaciones de entrada por polling
    //- inCommManager: nombre de la función
    //- "": nombre de la tarea (no importa)
    //- 7000: tamaño de pila (5000 es insuficiente y 5500 es ok)
    //- NULL: puntero a parámetros
    //- 1: prioridad baja
    //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
    //- 1: core en el que se ejecuta
    xTaskCreatePinnedToCore(inCommManager,"",7000,NULL,1,NULL,1);
  }

  //Configuramos el pin del sensor de movimiento como entrada
  pinMode(pinPIR,INPUT);
  //Activamos las interrupciones para el pin del sensor de movimiento
  attachInterrupt(pinPIR,interruptPIR,RISING);
  //Creamos una nueva tarea para cambiar la resolución de la cámara
  //No es necesario que devuelva respuesta a nadie
  cameraAddTask(currentRes,"","","");
  //Informamos a todos los suscriptores que se ha reiniciado
  outCommAddTask(0,"0","Dispositivo reiniciado",defaultKeyb);
  //Inicializamos la semilla de números aleatorios
  randomSeed(millis());
}

void loop()
{
  //Nada especial que hacer aquí
  delay(1);
}
