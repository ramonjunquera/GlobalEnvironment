/*
  Autor: Ramón Junquera
  Fecha: 20191127
  Tema: Librería de gestión de RTC Tiny
  Objetivo: Ejemplo de lectura de hora desde un servidor NTP

  Descripción:
    Se obtiene la hora de un servidor NTP, se fija en el RTC y se envía por el puerto serie cada segundo
*/

#include <Arduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h> //Gestión de wifi
#elif defined(ESP32)
  #include <WiFi.h> //Gestión de wifi
#endif
#include <RoJoRTCtiny.h> //Gestión de RTC Tiny
#ifdef __arm__
  #include <RoJoNTPclientRPi.h> //Gestión conexión servidor NTP para RPi
#else
  #include <RoJoNTPclientESP.h> //Gestión conexión servidor NTP para ESP
#endif

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi

//Definición de objetos globales
RoJoRTCtiny myClock; //Objeto de gestión del RTC Tiny

#if !defined(__arm__) //RPi no necesita inicializar conectividad
//Inicializa y conecta wifi
void beginWiFi() {
  WiFi.mode(WIFI_STA); //Ponemos el wifi en modo cliente que conecta a punto de acceso
  WiFi.begin(wifiSSID,wifiPassword); //Conecta con punto de acceso
  Serial.print(F("Conectando wifi."));
  while (WiFi.status() != WL_CONNECTED) { //Esperamos hasta que conecte
    delay(400);
    Serial.print(".");
  }
  Serial.println(F("ok"));
}
#endif

void setup() {
  Serial.begin(115200); delay(3000);
  Serial.print("\nInicializando RTC..");
  Serial.println(myClock.begin()?"ok":"ko");

  #if defined(ESP32) || defined(ESP8266) //Si es un ESP
    beginWiFi(); //Inicializamos y conectamos wifi
  #endif

  RoJoNTPclient ntp; //Objeto de gestión de conexión con servidor NTP
  //Inicialización
  //  Servidor NTP: pool.ntp.org
  //  Huso horario: +1 (Madrid)
  //  Horario de verano: no
  ntp.begin("pool.ntp.org",1,false);
  Serial.print("Connecting NTP..");
  uint32_t secNow=ntp.get();
  if(secNow>0) { //Si la hora es válida...
    myClock.set1900(secNow); //...la asignamos
    Serial.println("ok");
  } else Serial.println("ko"); //Si no es válida, no hacemos nada
}

void loop() {
  RoJoDateTime r;
  //Obtenemos hora del RTC
  myClock.get(&r);
  //Formateamos fecha y hora y la enviamos por Serial
  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));
  delay(1000);
}
