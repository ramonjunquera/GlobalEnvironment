/*
  Autor: Ramón Junquera
  Fecha: 20191127
  Tema: Librería de gestión de RTC BM8563
  Objetivo: Ejemplo de lectura de hora desde un servidor NTP
  Material: M5Stick-C

  Descripción:
    Se fija una hora y se envía por el puerto serie la hora leida cada segundo
  Nota.
    Es posible que se tenga que reiniciar dos veces el dispositivo para que conecte con
    el punto de acceso.
*/

#include <Arduino.h>
#include <WiFi.h> //Gestión de wifi
#include <RoJoRTCBM8563.h> //Gestión de RTC BM8563
#include <RoJoNTPclientESP.h> //Gestión conexión servidor NTP para ESP

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi

//Definición de objetos globales
RoJoRTCBM8563 myClock; //Objeto de gestión del RTC BM8563 del M5Stick-C

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

void setup() {
  Serial.begin(115200);

  //Encendemos el led para saber que está funcionando
  pinMode(10,OUTPUT); digitalWrite(10,LOW); //DEBUG

  //Nota. M5Stick-C tiene un RTC BM8563 con identificador I2C: 0x51
  //Conectado a los pines SDA=21 y SCL=22
  Serial.print("\nInicializando RTC..");
  Serial.println(myClock.begin(21,22)?"ok":"ko");

  //Inicializamos y conectamos wifi
  beginWiFi();

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
