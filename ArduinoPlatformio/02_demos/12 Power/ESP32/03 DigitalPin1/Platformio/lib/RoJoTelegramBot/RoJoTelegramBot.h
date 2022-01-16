/*
  Nombre de la librería: RoJoTelegramBot.h
  Versión: 20211107
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión de bots de Telegram para ESP32
*/

#ifndef RoJoTelegramBot_h
#define RoJoTelegramBot_h

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266)
  #error Library RoJoTelegramBot is only compatible with ESP devices
#endif  

#include <Arduino.h>
#include <RoJoWiFiClientSecure.h> //Gestión de conexiones seguras con gestión de datos recibidos

#ifdef ESP32 //Si es un ESP32, además podremos utilizar webhooks
  #include <SPIFFS.h> //Gestión de archivos SPIFFS para ESP32
  #include <RoJoWiFiServerSecure32.h> //Gestión de servidor seguro
#else //Si es ESP8266
  #include <FS.h> //Gestión de archivos SPIFFS para ESP8266
#endif

struct TelegramMessage {
  String update_id;
  String message_id;
  String from_id;
  String from_name;
  String chat_id;
  String date;
  String text;
};

class RoJoTelegramBot {
  private:
    String _botToken; //Token del bot
    const String _telegramHost="api.telegram.org"; //Host de Telegram
    const uint16_t _telegramPort=443; //Puerto de Telegram (https)
    const String boundary = "-RoJo2017-Smile!";
    const String bodyEnd="\r\n--"+boundary+"--\r\n"; //Componemos el final del cuerpo
    String _lastUpdateId=""; //Identificador del último mensaje recibido
    String _actionCodes[9]={"typing","upload_photo","record_video","record_audio","upload_audio","upload_document","find_location","record_video_note","upload_video_note"};
    String _sendFileLocal(String chat_id,String fileName,String ext);
    String _sendFileRemote(bool isPhoto,String chat_id,String file_id,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
    byte _sendHTML(WiFiClientSecure *client,String headers,String body="",String filename=""); //Conecta y envía HTML
  public:
    void begin(String botToken); //Inicializa la clase
    void getNextMessage(TelegramMessage* msg); //Obtiene el siguiente mensaje
    bool sendMessage(String chat_id,String text,String keyboard="",bool resize=false,bool oneTime=false); //Envía un mensaje a un chat
    String sendPhotoLocal(String chat_id,String fileName); //Envía una foto local
    String sendPhotoRemote(String chat_id,String photo,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
    String sendFileLocal(String chat_id,String fileName); //Envía un archivo local
    String sendFileRemote(String chat_id,String file_id,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
    bool sendChatAction(String chat_id,byte actionCode); //Envía un mensaje de acción
    byte webhookEnableTelegram(const char *secureServerCertificateFile,String localHostname,uint16_t port=443,byte maxClients=1); //Activa webhook en Telegram
    byte webhookCheckTelegram(); //Consulta en Telegram si el bot tiene activado el webhook
    bool webhookDisableTelegram(); //Desactiva webhook en Telegram

  #ifdef ESP32 //Funcionalidades webhook
    private:
      RoJoWiFiServerSecure32 *_webhookServer=NULL; //Servidor seguro para webhook
      void (*_customFunction)(TelegramMessage *msg); //Función de envío de mensajes recibidos por webhook
    public:
      byte webhookEnable(const char *secureServerCertificateFile,const char *secureServerPrivateKeyFile,void (*customFunction)(TelegramMessage *msg),uint16_t port=443,byte maxClients=1,uint16_t timeoutClient=2500); //Activa webhook
      bool webhookDisable(); //Desactiva webhook
      void webhookRefresh(); //Detecta nuevos mensajes recibidos por webhook
  #endif
};

#endif
