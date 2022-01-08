/*
  Nombre de la librería: RoJoWiFiServerSecure32.h
  Autor: Ramón Junquera
  Fecha: 20211026
  Descripción:
    Servidor seguro (SSL)
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiServerSecure32_H
#define RoJoWiFiServerSecure32_H

#include <Arduino.h>
#include <SPIFFS.h> //Para acceso a archivos de certificado y clave
#include <WiFi.h>
#include <lwip/sockets.h>
#include <openssl/ssl.h>
#include <RoJoWiFiClientSecure32.h> //Cliente seguro

class RoJoWiFiServerSecure32 {
  private:
    byte _maxClients; //Máximo número de clientes
    int _socket; //Identificador del socket
    uint16_t _port; //Puerto de escucha
    bool _listening; //El socket está escuchando?
    byte _loadFile2Mem(const char *path,byte **pMem,uint16_t *fileLength); //Carga un archivo en memoria
    SSL_CTX *_SSLcontext; //Contexto SSL
    struct timeval _timeoutClient; //Timeout para conexiones cliente
  public:
    RoJoWiFiServerSecure32(uint16_t port=443,byte maxClients=1,uint16_t timeoutClient=1); //Constructor
    void end(); //Cierra el servidor
    ~RoJoWiFiServerSecure32(); //Destructor
    byte begin(const char* secureServerCertificateFile,const char* secureServerPrivateKeyFile);
    RoJoWiFiClientSecure32* available(); //Se ha conectado algún nuevo cliente?
};

#endif
