/*
  Autor: Ramón Junquera
  Tema: Wifi
  Fecha: 20180802
  Objetivo: Mostrar intensidad de una señal en display
  Material: ESP32 OLED TTGO 16Mb

  Descripción:
  Al inicio del programa se define como constante el nombre del punto de acceso
  monitorizar.
  Se mostrará contínuamente la intensidad en el display OLED I2C integrado.
  Si después conectamos a la placa una alimentación por batería, tendremos un 
  dispositivo que nos permitirá hacer mediciones precisas para correlacionar
  intensidad de señal con distancia.
  Este es un ejercicio previo al proyecto de trilateración.

  Resultado:
  Se muestra la intensidad de un punto de acceso determinado
*/

#include <Arduino.h>
#include <WiFi.h>
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos
#include "RoJoABC.h" //Gestión de fuentes

//Constantes globales
const String monSSID = "ESP"; //Nombre del punto de acceso a monitorizar

//Variables globales
RoJoSSD1306 display; //Creamos objeto de gestión del display
RoJoSprite textSprite; //Creamos el sprite que contendrá el texto
RoJoABC font; //Creamos objeto de gestión de fuentes

void setup()
{
  //Activamos wifi
  WiFi.begin();
  //Inicializamos el display con los pines en los que está conectado la placa
  //En este modelo no son seleccionables y no coinciden con los estandar:
  //void begin(byte pinCS_SD,byte pinSDA,byte pinSCL,byte pinRST);
  //Nota es este ejemplo no se utiliza una tarjeta SD para almacenar los archivos
  //por lo tanto el pinCS_SD no se tiene en cuenta. Podríamos escribir cualquier valor
  display.begin(SS,4,15,16);
}

void loop()
{
  //Solicitamos un nuevo scanner y obtenemos el número de puntos de acceso disponibles
  byte totalSSID=WiFi.scanNetworks();
  //Recorremos todos los puntos de acceso
  for (byte currentSSID=0;currentSSID<totalSSID;currentSSID++)
  {
    //Si el nombre del punto de acceso procesado coincide con el que queremos procesar...
    if (WiFi.SSID(currentSSID)==monSSID)
    {
      //Limpiamos pantalla
      display.videoMem->clear();
      //Creamos el sprite de texto
      font.print("/21x33d.fon",String(WiFi.RSSI(currentSSID)),&textSprite);
      //Lo mostramos
      display.videoMem->drawSpritePage(0,0,&textSprite,4);
      //Refrescamos pantalla
      display.show();
      //Hemos terminado. No seguimos mirando más
      return;
    }
  }
  //Hemos recorrido todos los puntos de acceso disponibles y no hemos encontrado el que buscamos

  //Limpiamos pantalla
  display.videoMem->clear();
  //Refrescamos pantalla
  display.show();
}
