/*
  Nombre de la librería: RoJoWiFiServerSecure32.h
  Autor: Ramón Junquera
  Fecha: 20211107
  Descripción:
    Cliente seguro (SSL)
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiClientSecure32_H
#define RoJoWiFiClientSecure32_H

#include <Arduino.h>
#include <SPIFFS.h> //Para acceso a archivos de certificado y clave
#include <WiFi.h>
#include <lwip/sockets.h>
#include <openssl/ssl.h>

class RoJoWiFiClientSecure32 {
  private:
    int _fd; //Socket del cliente
    SSL *_SSLconn; //Conexión SSL
  public:
    RoJoWiFiClientSecure32(int fd,SSL_CTX *SSLcontext); //Constructor
    ~RoJoWiFiClientSecure32(); //Destructor
    bool connected(); //Está conectado?
    void stop();
    int read(byte *buf, size_t size); //Lee datos del buffer de entrada
    bool read(byte *b); //Lee el siguiente byte del buffer de entrada
    String readString(String endText); //Lee texto hasta cierta marca
    String readInt(); //Lee un número entero
    bool readFindString(String findText,String endText=""); //Busca un texto
    void write(String s); //Envía una cadena al cliente
    int available(); //Devuelve el número de bytes pendientes de leer
    String readFindFieldInt(String fieldName,String endText=""); //Lee campo numérico JSON
    String readFindFieldString(String fieldName,String endText=""); //Lee campo de texto JSON
};

#endif