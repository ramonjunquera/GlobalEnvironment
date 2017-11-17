/*
 Autor: Ramón Junquera
 Descripción: Servidor FTP para ESP v20170912
*/

#ifndef EspFtpServer_h
#define EspFtpServer_h

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h> //Para crear el punto de acceso
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClient.h>
#if defined(ESP32)
  #include <SPIFFS.h> //Librería de gestión de SPIFFS para ESP32
#else
  #include <FS.h> //Librería de gestión de SPIFFS para ESP8266
#endif

#define FTP_SERVER_VERSION "FTP v1.0 20170912" //Versión
#define FTP_CTRL_PORT 21 //Puerto de escucha
#define FTP_DATA_PORT_PASV 50009 //Puerto de datos para el modo pasivo

#define FTP_TIME_OUT 5 //Tiempo de desconexión del cliente por inactividad en minutos
#define FTP_CMD_SIZE 255 + 8 //Máximo tamaño de un comando
#define FTP_CWD_SIZE 255 + 8 //Máximo tamaño del nombre de una carpeta
#define FTP_FIL_SIZE 255 //Máximo tamaño del nombre de un archivo
#define FTP_BUF_SIZE 1024 //Tamaño del buffer de lectura/escritura

class FtpServer
{
public:
  void begin(String uname, String pword);
  void handleFTP();
private:
  void iniVariables();
  void clientConnected();
  void disconnectClient();
  bool userIdentity();
  bool userPassword();
  bool processCommand();
  bool dataConnect();
  bool doRetrieve();
  bool doStore();
  void closeTransfer();
  void abortTransfer();
  bool makePath(char *fullname);
  bool makePath(char *fullName,char *param);
  byte getDateTime(uint16_t *pyear,byte *pmonth,byte *pday,byte *phour,byte *pminute,byte *second);
  char* makeDateTimeStr(char *tstr,uint16_t date,uint16_t time);
  int8_t readChar();

  IPAddress dataIp; //Dirección ip del cliente para datos
  WiFiClient client;
  WiFiClient data;
  
  File file;
  
  boolean dataPassiveConn;
  uint16_t dataPort;
  char buf[ FTP_BUF_SIZE ]; //Buffer de datos para transferencias
  char cmdLine[ FTP_CMD_SIZE ]; //Línea de comandos recibida
  char cwdName[ FTP_CWD_SIZE ]; //Nombre de la carpeta actual
  char command[ 5 ]; //Comando recibido
  boolean rnfrCmd; //El comando anterior era RNFR ?
  char *parameters; //Puntero de inicio de parámetros recibido
  uint16_t iCL; //Puntero al siguiente carácter de la línea de comandos (cmdLine)
  byte cmdStatus; //Estado de la conexión ftp
  byte transferStatus; //Estado de la transmisión de datos
  uint32_t millisTimeOut; //Tiempo de desconexión por inactividad
  uint32_t millisDelay,millisEndConnection; 
  uint32_t millisBeginTrans; //Momento de inicio de la transacción
  uint32_t bytesTransfered;
  String _FTP_USER;
  String _FTP_PASS;
};

#endif // ESP32FtpServer_h
