/*
  Autor: Ramón Junquera
  Fecha: 20220108
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
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram
#include <RoJoArduCAM.h> //Librería de gestión de la cámara

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
const uint32_t checkingGap=1000; //Tiempo de espera en misisegundos para comprobación de nuevos mensajes
const byte pinLed=LED_BUILTIN; //Pin del led integrado en placa
const uint32_t maxWait=30000; //Tiempo máximo de espera = 30 segundos
const float factorWait=1.2; //Factor de la progresión geométrica para calcular el siguiente tiempo de espera

//Definición de variables globales
RoJoTelegramBot bot; //Creamos el objeto de gestión del bot
uint32_t startWait=1+1/(factorWait-1); //Tiempo de espera inicial
uint32_t currentWait=startWait; //Tiempo de espera actual
RoJoArduCAM camera; //Creamos el objeto de gestión de la cámara
//Pinout
#ifdef ESP8266 //Si es una ESP8266...
  const byte pinCS=D0;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinCS=4;
#endif

//Procesa todos los mensajes pendientes
void handleNewMessages() {
  TelegramMessage msg; //Creamos estructura de mensaje
  bot.getNextMessage(&msg); //Obtenemos el siguiente mensaje
  while(msg.text.length()) { //Mientras haya mensaje...
    if(msg.text=="/start") { //Si se envía el comando /start siempre debemos mostrar una mínima información
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
      bot.sendMessage(msg.chat_id,message); //Enviamos el mensaje
    }
    else if (msg.text == "/photo") { //Si es el comando /photo...
      camera.takePhoto(); //...solicitamos que se haga la foto...
      bot.sendChatAction(msg.chat_id,0); //...enviamos un mensaje de acción de "escribiendo"
      uint32_t resX,resY; //Resolución de la imagen guardada
      //...descargamos la foto a local...
      //Como nombre de archivo sólo el nombre. Sin barra inicial
      //La extensión la pone él
      byte errorCode=camera.savePhoto("photo",&resX,&resY);
      //Si hubo algún error...informamos
      if(errorCode) bot.sendMessage(msg.chat_id,"Fallo al enviar. Error " + String(errorCode));
      else { //No hubo errores
        //...enviamos un mensaje de acción de "enviado foto"
        bot.sendChatAction(msg.chat_id,1);
        //...enviamos la foto por Telegram
        bot.sendPhotoLocal(msg.chat_id,"/photo.jpg");
      }
    }
    else if(msg.text.length()==5) { //Si el comando tiene una longitud de 5...
      if(msg.text.substring(0,4)=="/res") { //...si el comando comienza por /res...
        //...si el último carácter es numérico...
        String n=msg.text.substring(4);
        if(n>="0" && n<="8") {
          byte res=n[0]-48; //...obtenemos el valor del código de la resolución
          camera.setResolution(res); //Aplicamos la resolución
          bot.sendMessage(msg.chat_id,"Cambiada a resolución " + n); //Informamos de la resolución aplicada
        }
      }
    }
    else bot.sendMessage(msg.chat_id,"Comando desconocido");
    
    currentWait=startWait; //Reseteamos el tiempo de espera
    //Hemos terminado de procesar el mensaje actual
    bot.getNextMessage(&msg); //Leemos el siguiente
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  bot.begin(botToken); //Inicializamos el bot
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100); //Esperamos un momento
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  while (WiFi.status() != WL_CONNECTED) { //Mientras no estemos conectados...
    //Cambiaremos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  }
  //Hemos conseguido conectar

  byte errorCode = camera.begin(pinCS); //Inicializamos la cámara
  if(errorCode) { //Si tenemos algún error en la inicialización...
    Serial.println("Error init camera "+String((int)errorCode));
    while(1) { //Hacemos parpadear el led, rápido y para siempre
      digitalWrite(pinLed,!digitalRead(pinLed));
      delay(30);
    };
  }
  //Hemos inicializado la cámara correctamente
  //Por defecto arranca en modo jpg con la resolución 2 = 320x240

  digitalWrite(pinLed,HIGH); //Nos aseguramos que el led esté apagado
}

void loop() {
  handleNewMessages(); //Procesa todos los nuevos mensajes
  delay(currentWait); //Esperamos
  currentWait*=factorWait; //Aumentamos el tiempo de espera para el siguiente ciclo
  if(currentWait>maxWait) currentWait=maxWait; //Si sobrepasamos el máximo...lo limitamos
}