/*
  Nombre de la librería: RoJoTelegramBot.h
  Versión: 20190910
  Autor: Ramón Junquera
  Descripción:
    Librería de gestión de bots de Telegram para ESP
*/

#ifndef RoJoTelegramBot_h
#define RoJoTelegramBot_h

//Comprobamos que la placa es compatible
#if !defined(ESP32) && !defined(ESP8266)
  #error Library RoJoTelegramBot is only compatible with ESP32 & ESP8266 family devices
#endif

#include <Arduino.h>
//Cargamos la librería de gestión de archivos SPIFFS dependiendo de la placa
#if defined(ESP32)
  #include <SPIFFS.h> 
#elif defined(ESP8266)
  #include <FS.h>
#endif
#include <WiFiClientSecure.h> //Librería para gestión de conexiones https

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
    String _lastUpdateId=""; //Identificador del último mensaje recibido
    String _actionCodes[9]={"typing","upload_photo","record_video","record_audio","upload_audio","upload_document","find_location","record_video_note","upload_video_note"};
    String _clienParseInt(WiFiClientSecure *client,uint16_t timeOut);
    bool _clientFindString(WiFiClientSecure *client,uint16_t timeOut,String findText,String endText="");
    String _clientReadString(WiFiClientSecure *client,uint16_t timeOut,String border);
    String _clientFieldInt(WiFiClientSecure *client,uint16_t timeOut,String fieldName,String endText="");
    String _clientFieldString(WiFiClientSecure *client,uint16_t timeOut,String fieldName,String endText="");
    String _sendFileLocal(String chat_id,String fileName,String ext);
    String _sendFileRemote(bool isPhoto,String chat_id,String file_id,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
  public:
    void begin(String botToken); //Inicializa la clase
    void getNextMessage(TelegramMessage* msg); //Obtiene el siguiente mensaje
    bool sendMessage(String chat_id,String text,String keyboard="",bool resize=false,bool oneTime=false); //Envía un mensaje a un chat
    String sendPhotoLocal(String chat_id,String fileName); //Envía una foto local
    String sendPhotoRemote(String chat_id,String photo,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
    String sendFileLocal(String chat_id,String fileName); //Envía un archivo local
    String sendFileRemote(String chat_id,String file_id,String caption = "",bool disable_notification = false,String reply_to_message_id = "0");
    bool sendChatAction(String chat_id,byte actionCode); //Envía un mensaje de acción
};

#endif
