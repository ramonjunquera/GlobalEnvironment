/*
  Autor: Ramón Junquera
  Fecha: 20181007
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Obtener una foto remota
  Material: placa ESP, ArduCAM-Mini-2MP
  Descripción:
    Utilizamos las librerías desarrolladas para ArduCAM.
    Atendemos comandos para cambiar la resolución y tomar una foto.
    El ejemplo nos permite tomar una foto a petición y a distancia
    mediante comandos en Telegram.
    El bot nos entrega la imagen obtenida.
 */
#include <Arduino.h>
#ifdef ESP8266 //Si es una ESP8266...
  #include <ESP8266WiFi.h> //Librería para gestión de wifi para ESP8266
#elif defined(ESP32)
  #include <WiFi.h> //Librería para gestión de wifi para ESP32
#endif
#include "RoJoTelegramBot.h" //Librería para gestión de bots de Telegram
#include "RoJoArduCAM.h" //Librería de gestión de la cámara

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
//Creamos el objeto de gestión del bot
RoJoTelegramBot bot;
//Tiempo de espera inicial
uint32_t startWait=1+1/(factorWait-1);
//Tiempo de espera actual
uint32_t currentWait=startWait;
//Creamos el objeto de gestión de la cámara
RoJoArduCAM camera;
//Definimos pin CS
#ifdef ESP8266 //Si es una ESP8266...
  const byte pinCS=D0;
#elif defined(ESP32) //Si es un ESP32
  const byte pinCS=4;
#endif

void handleNewMessages()
{
  //Procesa todos los mensajes pendientes
  
  //Creamos estructura de mensaje y obtenemos el siguiente mensaje
  TelegramMessage msg;
  bot.getNextMessage(&msg);
  //Mientras haya mensaje...
  while(msg.text.length())
  {
    //Si se envía el comando /start siempre debemos mostrar una mínima información
    if(msg.text=="/start")
    {
      //Componemos el mensaje a enviar en una sola cadena (es más rápido)
      String message = "RoJo Telegram Bot library\n\n";
      message += "/photo : toma foto\n";
      message += "/resX : fija resolución\n";
      message += "donde X = [0,8]\n";
      message += "X = 0 -> 160x120\n";
      message += "X = 1 -> 176x144\n";
      message += "X = 2 -> 320x240\n";
      message += "X = 3 -> 352x288\n";
      message += "X = 4 -> 640x480\n";
      message += "X = 5 -> 800x600\n";
      message += "X = 6 -> 1024x768\n";
      message += "X = 7 -> 1280x1024\n";
      message += "X = 8 -> 1600x1200\n";     
      //Enviamos el mensaje
      bot.sendMessage(msg.chat_id,message);
    }
    //Si es el comando /photo...
    else if (msg.text == "/photo")
    {
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
        bot.sendMessage(msg.chat_id,"Fallo al enviar. Error " + String(errorCode));
      }
      else //No hubo errores
      {
        //...enviamos un mensaje de acción de "enviado foto"
        bot.sendChatAction(msg.chat_id,1);
        //...enviamos la foto por Telegram
        bot.sendPhotoLocal(msg.chat_id,"/photo.jpg");
      }
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
          //...obtenemos el valor del código de la resolución
          byte res=n[0]-48;
          //Aplicamos la resolución
          camera.setResolution(res);
          //Informamos de la resolución aplicada
          bot.sendMessage(msg.chat_id,"Cambiada a resolución " + n);
        }
      }
    }
    else //Comando desconocido
      bot.sendMessage(msg.chat_id,"Comando desconocido");
    
    //Reseteamos el tiempo de espera
    currentWait=startWait;
    //Hemos terminado de procesar el mensaje actual
    //Leemos el siguiente
    bot.getNextMessage(&msg);
  }
}

void setup()
{
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

  //Inicializamos la cámara con el pin CS conectado al D0
  byte errorCode = camera.begin(pinCS);
  //Si tenemos algún error en la inicialización...
  if(errorCode)
  {
    //Hacemos parpadear el led, rápido y para siempre
    while(1)
    {
      digitalWrite(pinLed,!digitalRead(pinLed));
      delay(30);
      yield();
    };
  }
  //Hemos inicializado la cámara correctamente
  //Por defecto arranca en modo jpg con la resolución 2 = 320x240

  //Nos aseguramos que el led esté apagado
  digitalWrite(pinLed,HIGH);
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
}
