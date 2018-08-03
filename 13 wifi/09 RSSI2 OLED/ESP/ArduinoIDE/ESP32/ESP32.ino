/*
  Autor: Ramón Junquera
  Tema: Wifi
  Fecha: 20180802
  Objetivo: Mostrar intensidad de dos señales wifi en display
  Material: ESP32 OLED TTGO 16Mb

  Descripción:
  Al inicio del programa se definen como constantes los nombres de los puntos
  de acceso monitorizar.
  Se mostrarán contínuamente las intensidades en el display OLED I2C integrado.
  La intensidad del primer punto de aceso se mostrará en la parte superior.
  La segunda intensidad en la parte inferior.

  Análisis.
  El objetivo de este desarrollo es comprobar si dos dispositivos idénticos que
  crean puntos de acceso con distintos nombre y que se sitúan uno al lado de otro,
  emiten la misma intensidad de señal wifi.
  El resultado es que NO. Es parecida pero no coincide.
  Además la intensidad varía de manera muy errática.
  Si hacemos la tabla de la intensidad de señal respecto a la distancia, comprobaremos
  que además no es lineal.
  Estos son unos datos de ejemplo:

  distancia RSSI1 RSSI2
      0      -37   -37
     50      -70   -65
    100      -81   -70
    150      -62   -78
    200      -64   -87
    250      -72   -75
    300      -70   -72
    350      -68   -74
    400      -70   -75
    450      -84   -73
    500      -73   -83

  Teniendo en cuenta que:
  - El tiempo que necesita para escanear las redes disponibles es muy alto (unos 3 segundos)
  - Los resultados son muy erráticos
  - No podemos establecer una relación entre la intensidad de señal y la distancia
  - La intensidad depende del dispositivo que crea el punto de acceso
  - Dos dispositivos iguales no da la misma señal
  - Las intensidad de señal es interferida por el cuerpo humano
  Deducimos que:
  - No podemos utilizar el valor de intensidad de señal wifi para un cálculo de posición en
    interiores por trilateración.

  Resultado:
  Se muestran las intensidades de dos puntos de acceso wifi determinados.
*/

#include <Arduino.h>
#include <WiFi.h>
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos
#include "RoJoABC.h" //Gestión de fuentes

//Constantes globales
const String monSSID1 = "ESP1"; //Nombre del punto de acceso a monitorizar
const String monSSID2 = "ESP2"; //Nombre del punto de acceso a monitorizar

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
  //Limpiamos pantalla
  display.videoMem->clear();
  //Recorremos todos los puntos de acceso
  for (byte currentSSID=0;currentSSID<totalSSID;currentSSID++)
  {
    //Tomamos nota nombre actual del punto de acceso
    String nameSSID=WiFi.SSID(currentSSID);
    //Si el nombre del punto de acceso procesado coincide con alguno de los buscados...
    if (nameSSID==monSSID1 || nameSSID==monSSID2)
    {
      //Creamos el sprite de texto
      font.print("/21x33d.fon",String(WiFi.RSSI(currentSSID)),&textSprite);
      //Lo mostramos
      display.videoMem->drawSpritePage(0,nameSSID==monSSID1?0:4,&textSprite,4);
    }
  }
  //Refrescamos pantalla
  display.show();
}
