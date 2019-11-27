/*
  Autor: Ramón Junquera
  Fecha: 20191127
  Tema: Librería de gestión de RTC BM8563
  Objetivo: Mostrar reloj sincronizado por Internet en display
  Material: M5Stick-C

  Descripción:
    Muestra la fecha y hora actuales en el display.
    Sincroniza la hora con un servidor NTP de Internet periodicamente.
    Muestra el estado de la última conexión al servidor NTP en forma de icono.
*/

#include <Arduino.h>
#include <WiFi.h> //Gestión de wifi
#include <RoJoRTCBM8563.h> //Gestión de RTC BM8563
#include <RoJoNTPclientESP.h> //Gestión conexión servidor NTP para ESP
#include <RoJoST7735S.h> //Gestión de display

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi
const uint32_t periodUpdateNTP=10; //Periodo de actualización de hora de Internet en segundos

//Definición de objetos globales
RoJoRTCBM8563 myClock; //Objeto de gestión del RTC BM8563 del M5Stick-C
RoJoST7735S display; //Gestión de display
RoJoNTPclient ntp; //Gestión de conexión a servidor NTP

//Definición de variables globales
uint32_t nextUpdateNTP=0; //Próxima actualización de hora de Internet
bool connected=false; //El último intento de actualización de hora de Internet fue correcto?
bool lastConnected=false; //Estado de la conexión del último refresco
byte lastDay=0; //Día del último refresco

//Inicializa y conecta wifi
void beginWiFi() {
  WiFi.mode(WIFI_STA); //Ponemos el wifi en modo cliente que conecta a punto de acceso
  WiFi.begin(wifiSSID,wifiPassword); //Conecta con punto de acceso
  while (WiFi.status() != WL_CONNECTED) { //Esperamos hasta que conecte
    delay(400);
  }
}

void setup() {
  display.begin(); //Inicialización del display
  display.rotation(3); //Rotamos el display 90 grados
  display.clear(); //Limpiamos el display
  display.printOver("/10x22.fon","Init RTC...",0xFFFF);

  //Nota. M5Stick-C tiene un RTC BM8563 con identificador I2C: 0x51
  //Conectado a los pines SDA=21 y SCL=22
  String msg="Init RTC...";
  if(myClock.begin(21,22)) {
    msg+="ok";
    display.printOver("/10x22.fon",msg,0xFFFF);
  } else {
    msg+"ko";
    display.printOver("/10x22.fon",msg,0xFFFF);
    delay(2000);
  }

  //Inicializamos y conectamos wifi
  display.printOver("/10x22.fon","Init wifi...",0xFFFF,0,23);
  beginWiFi();

  //Por defecto icono de wifi off
  display.clear();
  display.drawSprite("/wifioff.spr");

  //Inicialización conexión NTP
  //  Servidor NTP: pool.ntp.org
  //  Huso horario: +1 (Madrid)
  //  Horario de verano: no
  ntp.begin("pool.ntp.org",1,false);
}

//Convierte el valor en una cadena de 2 dígitos, rellenando con 0 por la izquierda
String twoDigits(uint16_t v) {
  String txt="0"+String(v);
  return txt.substring(txt.length()-2);
}

void loop() {
  RoJoDateTime r;
  myClock.get(&r); //Obtenemos la hora del RTC
  uint32_t secNow=myClock.datetime2seconds(&r); //La pasamos a segundos desde 1900
  if(secNow>nextUpdateNTP) { //Si tenemos que refrescar la hora desde Internet...
    nextUpdateNTP=secNow+periodUpdateNTP; //Calculamos la nueva hora límite
    secNow=ntp.get(); //Obtenemos hora desde Internet
    connected=false; //Inicialmente somos pesimistas. No conseguiremos conectar
    if(secNow>0) { //Si la hora de Internet es válida...
      connected=true; //Hemos podido conectar!
      myClock.set1900(secNow); //Actualizamos la hora del RTC
      myClock.get(&r); //Obtenemos la hora del RTC
      secNow=myClock.datetime2seconds(&r); //La pasamos a segundos desde 1900
      nextUpdateNTP=secNow+periodUpdateNTP; //Calculamos la nueva hora límite
    }
  }

  //Refrescamos el display
  RoJoSprite spr; //Preparamos un sprite
  String txt; //Prepareamos uaa cadena para almecenar el texto a mostrar
  //Si el estado del refrescon anterior es distinto al actual...
  if(lastConnected!=connected) {
    //...refrescaremos el icono de la conexión
    //Si estamos conectados...mostramos icono wifi on
    if(connected) display.drawSprite("/wifion.spr");
    //Sino...mostramos icono wifi OFF
    else display.drawSprite("/wifioff.spr");
    //El estado anterior será el actual
    lastConnected=connected;
  }
  //Refrecaremos la fecha (si es necesario) y la hora a través de un sprite
  //Si el día del refresco anterior es distinto al actual...
  if(lastDay!=r.day) {
    //...refrescamos la fecha
    spr.setSize(display.xMax(),22); //Usaremos una fuente de 22 puntos de altura
    //Componemos el texto de fecha
    txt=twoDigits(r.day)+"/"+twoDigits(r.month)+"/"+String(r.year)+" "+myClock.dayName[r.weekDay];
    //Dibujamos en el sprite
    spr.printOver("/10x22.fon",txt,0xFFFF,5);
    //Dibujamos el sprite en el display
    display.drawSprite(&spr,0,24);
    //El día del último refresco será el actual
    lastDay=r.day;
  }
  //Refrecamos la hora
  spr.setSize(display.xMax(),31); //Usaremos una fuente de 31 puntos de altura
  txt=twoDigits(r.hour)+":"+twoDigits(r.minute)+":"+twoDigits(r.second);
  spr.printOver("/21x31d.fon",txt,0xFFFF,5);
  display.drawSprite(&spr,0,46);
  //Hemos terminado de trabajar con el sprite
  spr.end();

  delay(1000);
}
