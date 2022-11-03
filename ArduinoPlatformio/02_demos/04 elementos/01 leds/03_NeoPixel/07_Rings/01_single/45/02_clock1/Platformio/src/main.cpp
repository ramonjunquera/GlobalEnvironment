/*
  Autor: Ramón Junquera
  Fecha: 20221103
  Tema: Anillos de leds NeoPixel

  Descripción:
    Reloj analógico con una anillo de 45 leds que marca horas, minutos y segundos.
    Los colores son:
    - horas    : R : rojo
    - minutos  : G : verde
    - segundos : B : azul
    Cuando se juntan dos colores en el mismo led, se muestra la combinación de ellos
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <RoJoNeoPixelRing.h> //Gestion de anillos NeoPixel
#include <WiFi.h> //Gestión de wifi para ESP32
#include <RoJoRTCinternal.h> //Gestión de RTC interno
#include <RoJoNTPclient.h> //Gestión conexión servidor NTP

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
RoJoNeoPixelRing ring; //Objeto de gestión del anillo
RoJoRTCinternal myClock; //Objeto de gestión del RTC interno

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
  Serial.begin(115200); delay(3000);
  leds.begin(45,13); //Tenemos un anillo de 45 en pin 13
  ring.begin(&leds,0,45); //Anillo de 45 leds que inicia en posición 0
  beginWiFi(); //Inicializamos y conectamos wifi

  RoJoNTPclient ntp; //Objeto de gestión de conexión con servidor NTP
  //Inicialización
  //  Servidor NTP: pool.ntp.org
  //  Huso horario: +1 (Madrid)
  //  Horario de verano: no
  ntp.begin("pool.ntp.org",1,false);
  Serial.print("Connecting NTP..");
  uint32_t secNow=ntp.get();
  if(secNow>0) { //Si la hora es válida...
    myClock.set1900(secNow); //...ponemos en hora el reloj interno
    Serial.println("ok");
  } else Serial.println("ko"); //Si no es válida, no hacemos nada
}

void loop() {
  RoJoDateTime r;
  myClock.get(&r); //Obtenemos hora del RTC interno
  for(byte i=0;i<45;i++) leds.videoMem[i]={0,0,0}; //Apagamos todos los leds
  for(uint16_t angle=0;angle<360;angle+=90) ring.set({3,3,3},angle); //En blanco los cuadrantes (12,3,6 & 9)
  
  //Segundos
  uint16_t ledIndex=ring.ledIndex((uint16_t)r.second*6); //a=s/60*360=s*6
  leds.videoMem[ledIndex].channel[2]=66; //Azul
  //Minutos
  ledIndex=ring.ledIndex((uint16_t)r.minute*6); //a=m/60*360=m*6
  leds.videoMem[ledIndex].channel[0]=66; //Verde
  //Horas
  ledIndex=ring.ledIndex((uint16_t)(r.hour%12)*30+(uint16_t)r.minute/2); //a=(h%12)/12*360+m/60*360/12=h*30+m/2
  leds.videoMem[ledIndex].channel[1]=66; //Rojo

  leds.show();
  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));
  delay(1000);
}