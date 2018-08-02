/*
  Autor: Ramón Junquera
  Tema: Wifi
  Fecha: 20180802
  Objetivo: Mostrar intensidad de señal
  Material: ESP32 OLED TTGO 16Mb

  Descripción:
  La intensidad de señal es uno de los valores que se recogen cuando se hace un scanner de
  puntos de acceso wifi.
  Para ello es suficiente con activar wifi. No se necesita ni conectar con un punto ni crear
  un punto de acceso.
  El tiempo necesario para hacer un scanner es de unos 3 segundos.
  El programa hace scanner continuo y envía por el puerto serie los resultados con el formato:
  RSSI SSID (intensidad y nombre del punto de acceso)

  Recordemos que la intensidad de señal se mide en dB negativos.
  Por lo tanto -50 es una señal más fuerte que -90.
  El punto de acceso con una señal de -50 estará más cerca que el de -90.

  Resultado:
  Se muestra el listado de puntos de acceso próximos y su intensidad.
*/

#include <Arduino.h>
#include <WiFi.h>

void setup()
{
  Serial.begin(115200);
  WiFi.begin(); //Activamos wifi
}

void loop()
{
  //Solicitamos un nuevo scanner y obtenemos el número de puntos de acceso disponibles
  byte available_networks = WiFi.scanNetworks();
  //Recorremos todos los puntos de acceso
  for (byte network = 0; network < available_networks; network++)
  {
    Serial.print(WiFi.RSSI(network));
    Serial.print(" ");
    Serial.println(WiFi.SSID(network));
  }
  Serial.println();
}
