/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Sincronización NTP
  Material adicional: breadboard, placa ESP8266, cables, módulo "Tiny RTC I2C"

  Descripción:
  Aprovechando que las placas ESP8266 tienen wifi incluido, desarrollamos un
  ejemplo de cómo conectar con un servidor NTP en Internet, obtener la hora,
  y asignarla al RTC.

  Muestra los pasos y resultado final por el monitor serie.
  
  Resultado:
  Sincroniza el RTC con un servior NTP de Internet.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include "RoJoRTCclock.h" //Gestión del reloj del módulo Tiny RTC

//Creamos objeto de gestión del reloj
RoJoRTCclock myClock;

//Definimos como constantes el nombre del punto de acceso y su contraseña
const char *wifiSSID = "RoJo";
const char *wifiPassword = "ZAMPABOLLOS5";

//Zona horaria = GMT+1
const int8_t timeZone = 1;
//Horario de verano?
const bool summerTime=true; //Sí

void setup()
{
  Serial.begin(115200);
  //Si el reloj no está presente...
  if(!myClock.check()) Serial.println(F("Reloj no detectado"));
  else Serial.println(F("Reloj detectado"));

  //Ponemos el wifi en modo cliente que conecta a punto de acceso
  WiFi.mode(WIFI_STA);
  //Conecta con punto de acceso
  WiFi.begin(wifiSSID, wifiPassword);
  //Esperamos hasta que conecte
  Serial.print(F("Conectando wifi."));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(400);
    Serial.print(F("."));
  }
  Serial.println(F("ok"));
  
  //Definimos la hora de Internet
  //A la UTC le debemos añadir tantas horas como la zona horaria en la que estemos
  configTime((int32_t)(timeZone+summerTime) * 3600, 0, "pool.ntp.org", "time.nist.gov");

  //Obtenemos la hora del servidor NTP
  time_t realTime;
  Serial.print(F("Obteniendo hora."));
  do
  {
    realTime=time(nullptr);
    Serial.print(F("."));
    delay(400);
  }
  while(!realTime); //Repetimos mientras no sea una hora válida
  Serial.println(F("ok"));
  
  //Asignamos la hora obtenida al reloj
  myClock.set1900(realTime);
}

void loop()
{
  //Creamos una variable para guardar la fecha y hora actuales
  datetime currentTime=myClock.get();
  //Si hemos podido leer la hora del módulo conectado a I2C...
  if (currentTime.second < 128)
  {
    //Ya tenemos el tiempo actual guardado en la variable currentTime
    Serial.print(currentTime.day);
    Serial.print(F("/"));
    Serial.print(currentTime.month);
    Serial.print(F("/"));
    Serial.print(currentTime.year);
    Serial.print(F(" "));
    Serial.print(currentTime.hour);
    Serial.print(F(":"));
    Serial.print(currentTime.minute);
    Serial.print(F(":"));
    Serial.print(currentTime.second);
    Serial.print(F(" weekDay="));
    Serial.println(currentTime.weekDay);
  }
  else //No hemos podido leer la hora!
  {
    //Mostramos 4 guiones en la pantalla para indicar que algo no funciona
    Serial.println(F("----"));
  }
  //Esperamos un segundo
  delay(1000);
}
