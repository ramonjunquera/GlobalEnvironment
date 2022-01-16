/*
  Nombre de la librería: RoJoWiFiServerSecure.h
  Autor: Ramón Junquera
  Fecha: 20211107
  Descripción:
    Clase heredada de WiFiServerSecure con funciones de gestión de datos recibidos.
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiClientSecure_H
#define RoJoWiFiClientSecure_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

class RoJoWiFiClientSecure:public WiFiClientSecure { //Hereda de WiFiClientSecure
  public:
    String readString(String endText="",uint16_t timeOut=1500); //Lee texto hasta cierta marca
    bool readFindString(String findText,String endText="",uint16_t timeOut=1500); //Busca un texto
    String readFindFieldString(String fieldName,String endText="",uint16_t timeOut=1500); //Lee campo de texto JSON
    String readInt(uint16_t timeOut=1500); //Lee un número entero
    String readFindFieldInt(String fieldName,String endText="",uint16_t timeOut=1500); //Lee campo numérico JSON
};

#endif