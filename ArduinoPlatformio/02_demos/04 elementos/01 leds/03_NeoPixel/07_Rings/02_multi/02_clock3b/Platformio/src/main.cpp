/*
  Autor: Ramón Junquera
  Fecha: 20221103
  Tema: Anillos de leds NeoPixel

  Descripción:
    Reloj analógico de 3 anillos.
    Las agujas de hora, minuto y segundo se muestran en colores distintos y atraviesan los anillos interiores.
    Se muestra la referencia de los 4 cuadrantes.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <RoJoNeoPixelRing.h> //Gestión de anillos NeoPixel
#include <WiFi.h> //Gestión de wifi para ESP32
#include <RoJoRTCinternal.h> //Gestión de RTC interno
#include <RoJoNTPclient.h> //Gestión conexión servidor NTP

//Definición de constantes globales
const char *wifiSSID = "xxx"; //Nombre del punto de acceso wifi
const char *wifiPassword = "xxx"; //Contraseña del punto de acceso wifi

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
RoJoNeoPixelRing ringH,ringM,ringS; //Distintos anillos para horas, minutos y segundos
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
  leds.begin(104,13); //Tenemos 3 anillos de: 45+35+24=104 leds en pin 13
  ringS.begin(&leds,0,45); //Anillo de segundos (exterior). 45 leds. Inicia en 0
  ringM.begin(&leds,45,35); //Anillo de minutos (medio). 35 leds. Inicia en 45
  ringH.begin(&leds,80,24); //Anillo de horas (interior). 24 leds. Inicia en 45+35=80
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
  for(byte i=0;i<104;i++) leds.videoMem[i]={0,0,0}; //Apagamos todos los leds
  for(uint16_t angle=0;angle<360;angle+=90) { //En blanco los cuadrantes (12,3,6 & 9)
    ringH.set({3,3,3},angle);
    ringM.set({3,3,3},angle); 
    ringS.set({3,3,3},angle); 
  }

  //Segundos
  int16_t angle=(int16_t)r.second*6; //a=s/60*360=s*6
  uint16_t ledIndex=ringH.ledIndex(angle);
  leds.videoMem[ledIndex].channel[2]=66; //Azul en anillo interior
  ledIndex=ringM.ledIndex(angle);
  leds.videoMem[ledIndex].channel[2]=66; //Azul en anillo medio
  ledIndex=ringS.ledIndex(angle);
  leds.videoMem[ledIndex].channel[2]=66; //Azul en anillo exterior
  //Minutos
  angle=(int16_t)r.minute*6; //a=m/60*360=m*6
  ledIndex=ringH.ledIndex(angle);
  leds.videoMem[ledIndex].channel[0]=66; //Verde en anillo interior
  ledIndex=ringM.ledIndex(angle);
  leds.videoMem[ledIndex].channel[0]=66; //Verde en anillo medio
  //Horas
  angle=(int16_t)(r.hour%12)*30+(int16_t)r.minute/2; //a=(h%12)/12*360+m/60*360/12=h*30+m/2
  ledIndex=ringH.ledIndex(angle);
  leds.videoMem[ledIndex].channel[1]=66; //Rojo en anillo interior

  leds.show();
  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));
  delay(1000);
}