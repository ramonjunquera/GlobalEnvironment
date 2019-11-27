/*
  Autor: Ramón Junquera
  Fecha: 20191127
  Tema: Librería de gestión de RTC Tiny
  Objetivo: Mostrar reloj sincronizado por Internet en display

  Descripción:
    Muestra la fecha y hora actuales en un display Catalex.
    Sincroniza la hora con un servidor NTP de Internet periodicamente.
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
#include <RoJoTM1637.h> //Gestión de display Catalex

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi
const uint32_t periodUpdateNTP=10; //Periodo de actualización de hora de Internet en segundos
#ifdef ESP8266
  const byte pinCLKdisplay=D5;
  const byte pinDIOdisplay=D7;
#elif defined(ESP32)
  const byte pinCLKdisplay=14;
  const byte pinDIOdisplay=13;
#elif defined(__arm__)
  const byte pinCLKdisplay=21;
  const byte pinDIOdisplay=20;
#endif

//Definición de objetos globales
RoJoRTCtiny myClock; //Objeto de gestión del RTC BM8563 del M5Stick-C
RoJoTM1637 display; //Gestión de display
RoJoNTPclient ntp; //Gestión de conexión a servidor NTP

//Definición de variables globales
uint32_t nextUpdateNTP=0; //Próxima actualización de hora de Internet

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
  display.begin(pinCLKdisplay,pinDIOdisplay); //Inicialización del display
  display.setBrightness(4);

  Serial.print("Init RTC...");
  if(myClock.begin()) Serial.println("ok");
  else Serial.println("ko");

  #if defined(ESP32) || defined(ESP8266) //Si es un ESP
    beginWiFi(); //Inicializamos y conectamos wifi
  #endif

  //Inicialización conexión NTP
  //  Servidor NTP: pool.ntp.org
  //  Huso horario: +1 (Madrid)
  //  Horario de verano: no
  ntp.begin("pool.ntp.org",1,false);
}

void loop() {
  RoJoDateTime r;
  myClock.get(&r); //Obtenemos la hora del RTC
  uint32_t secNow=myClock.datetime2seconds(&r); //La pasamos a segundos desde 1900
  if(secNow>nextUpdateNTP) { //Si tenemos que refrescar la hora desde Internet...
    Serial.print("Connecting NTP..");
    nextUpdateNTP=secNow+periodUpdateNTP; //Calculamos la nueva hora límite
    secNow=ntp.get(); //Obtenemos hora desde Internet
    if(secNow>0) { //Si la hora de Internet es válida...
      Serial.println("ok");
      myClock.set1900(secNow); //Actualizamos la hora del RTC
      nextUpdateNTP=secNow+periodUpdateNTP; //Calculamos la nueva hora límite
    } else Serial.println("ko");
  }

  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));

  //Refrescamos el display
  display.set2DigitValue(0,r.minute);
  display.set2DigitValue(2,r.second);

  delay(1000);
}
