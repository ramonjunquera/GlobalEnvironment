/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: Librería para gestión de bots en Telegram
  Objetivo: Demo de uso de webhook con ayuda de la librería

  Requerimientos:
  - Bot creado
  - Certificado y clave pública generados con el hostname público y en LittleFS
  - Enrutación del puerto del router a la IP fija del servidor

  Dispositivos compatibles:
    Para utilizar webhook, necesitamos crear un servidor seguro en nuestro dispositivo.
    Sólo la familia ESP32 permite hacerlo.
    Si la librería detecta este dispositivo, permitirá acceder a las funciones de gestión
    de webhook.

  Descripción:
    Tomamos como base el ejemplo del control remoto del led integrado.
*/
#include <Arduino.h>
#include <WiFi.h> //Librería para gestión de wifi para ESP32
#include <RoJoTelegramBot.h> //Librería para gestión de bots de Telegram

//Definición de constantes globales
const char ssid[]="xxx"; //Nombre del punto de acceso (SSID)
const char password[]="xxx"; //Contraseña
const String botToken="xxx"; //Token del bot
#ifdef ARDUINO_LOLIN32LITE
  const byte pinLed=22;
#else
  const byte pinLed=LED_BUILTIN;
#endif
const char* secureServerCertificateFile="/autocert.crt"; //Archivo de certificado
const char* secureServerPrivateKeyFile="/private.key"; //Archivo de clave privada
const String localHostname="alira.synology.me";
const IPAddress fixIP=IPAddress(192,168,1,190); //IP fija
const uint16_t port=8443;

//Definición de variables globales
RoJoTelegramBot bot; //Creamos el objeto de gestión del bot

//Función de gestión de mensajes recibidos por webhook
void handleWebhookMessages(TelegramMessage *msg) {
  Serial.println("Recibido mensaje:  "+msg->text);
  //Si se envía el comando /start siempre debemos mostrar una mínima información
  if(msg->text=="/start") {
    //Componemos el mensaje a enviar en una sola cadena (es más rápido)
    String message = "RoJo Telegram Bot library\n\n";
    message += "/on : to switch the led ON\n";
    message += "/off : to switch the led OFF\n";
    message += "/status : returns current led status\n";
    //Enviamos el mensaje
    bot.sendMessage(msg->chat_id,message);
  } else if (msg->text == "/on") { //Si es el comando /on...
    digitalWrite(pinLed,LOW); //...encendemos el led
    bot.sendMessage(msg->chat_id,"Led is ON"); //Enviamos mensaje informado
  } else if(msg->text=="/off") { //Si el comando es /off...
    digitalWrite(pinLed,HIGH); //...apagamos el led
    bot.sendMessage(msg->chat_id,"Led is OFF"); //Enviamos mensaje informado
  } else if(msg->text=="/status") { //Si el comando es /status...
    //Obtenemos el texto del estado
    String statusText=digitalRead(pinLed)?"OFF":"ON";
    //Enviamos mensaje informado del estado
    bot.sendMessage(msg->chat_id,"Led is "+statusText);
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);
  bot.begin(botToken); //Inicializamos el bot
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led como salida
  digitalWrite(pinLed,LOW); //Encendemos el led
  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100); //Esperamos un momento a que desconecte
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  while (WiFi.status() != WL_CONNECTED) delay(100); //Esperamos hasta que conecte el wifi
  WiFi.config(fixIP,WiFi.gatewayIP(),WiFi.subnetMask(),WiFi.dnsIP()); //Asignamos la dirección IP fija
  byte errorCode=bot.webhookEnable(secureServerCertificateFile,secureServerPrivateKeyFile,handleWebhookMessages,port);
  if(errorCode) Serial.println("Servidor local WebHook activado con error "+String((int)errorCode));
  else { //Si el servidor local se ha activado correctamente...
    errorCode=bot.webhookCheckTelegram(); //Comprobamos el estado actual de webhook en Telegram
    if(errorCode==2) Serial.println("Error al conectar con Telegram");
    else { //Si hemos podido conectar con Telegram...
      if(errorCode==0) { //Si webhook no está activado en Telegram...
        errorCode=bot.webhookEnableTelegram(secureServerCertificateFile,localHostname,port);
        if(errorCode) Serial.println("Servicio webHook en Telegram activado con error "+String((int)errorCode));
      } else { //El servicio de webhook ya estaba activado en Telegram
        Serial.println("El servicio de webhook ya estaba activado en Telegram");
        errorCode=0;
      }
    }
  }
  if(!errorCode) { //Si no hubo errores...
    digitalWrite(pinLed,HIGH); //Apagamos el led
    Serial.print("Started on "); Serial.println(WiFi.localIP());
  }
}

void loop() {
  //Refrescamos continuamente el webhook para que gestiones nuevas conexiones
  bot.webhookRefresh();
  delay(1);
}