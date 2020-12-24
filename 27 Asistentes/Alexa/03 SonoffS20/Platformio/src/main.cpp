/*
  Autor: Ramón Junquera
  Fecha: 20201223
  Tema: librería Espalexa
  Objetivo: Ejemplo de control de de interruptor con ESP8266

  Descripción:
    El objetivo es controlar con Alexa un enchufe inteligente Sonoff S20.
    Puesto que es incómodo para cargar programas, utilizaremos una placa
    más cómoda, como la WeMos D1 mini.
    Usaremos las mismas conexiones/pines.
    Una vez que el programa esté depurado y probado, podemos pasarlo
    al Sonoff S20.

    Puesto que aun no tenemos la posibilidad de configurar un dispositivo
    como un enchufe inteligente (simple interruptor), utilizamos la
    funcionalidad de luz monocroma con cambio de brillo.

    Implementamos la función de reponer el último estado tras un corte
    de suministro.
    Cada cambio de estado lo guardamos en un archivo en SPIFFS.
*/

#include <Arduino.h>
#include <Espalexa.h>
#include <RoJoSwitch.h>
#include <FS.h> //Librería para gestión de archivos

#ifdef ARDUINO_ESP8266_SONOFF_S20
  const byte D7=13;
  const byte D6=12;
  const byte D3=0;
#endif

//Constantes globales
const byte pinLed=D7; //D7=13 pin del led verde
const byte pinRelay=D6; //D6=12 pin del led azul y el relé
const byte pinSwitch=D3; //D3=0 pin del pulsador
const char* ssid="xxx";
const char* password="xxx";
const String statusRelayFile="/status.txt"; //Nombre del archivo de estado

//Variables globales
Espalexa alexa; //Objeto de gestión de Alexa
RoJoSwitch button(pinSwitch);
bool statusRelay; //Estado del relé

//Conecta a wifi
void wifiSetup() {
  pinMode(pinLed,OUTPUT); //Led verde de salida
  WiFi.mode(WIFI_STA); //Fijamos el modo de conexión a un punto de acceso
  WiFi.disconnect(); //Forzamos a su desconexión (por si estaba conectado anteriormente)
  delay(100);
  WiFi.begin(ssid,password); //Pedimos conexión al punto de acceso
  while (WiFi.status()!=WL_CONNECTED) {
    digitalWrite(pinLed,!digitalRead(pinLed)); //Cambiamos el estado del led
    delay(100);
  }
  digitalWrite(pinLed,HIGH); //Led apagado
}

//Lee el estado desde el archivo y lo aplica
void readStatus() {
  //Abrimos el archivo del estado del relé como sólo lectura
  File f=SPIFFS.open(statusRelayFile,"r");
  if(!f) { //Si no se pudo abrir...
    f=SPIFFS.open(statusRelayFile,"w"); //..se crea
    f.write(0); //Se guarda el estado "apagado", por defecto
    f.close(); //Se cierra
    f=SPIFFS.open(statusRelayFile,"r"); //Se vuelve a abrir como sólo lectura
  }
  //Tenemos el archivo abierto
  
  //Leemos el estado guardado
  statusRelay=f.read(); //Sólo puede ser 0=off o 1=on
  f.close(); //Cerramos el archivo
  digitalWrite(pinRelay,statusRelay); //Aplicamos el estado leido
}

//Aplica el estado actual (statusRelay) al relé y lo guarda
void setStatus() {
  digitalWrite(pinRelay,statusRelay); //Aplicamos estado al relé
  //Abrimos el archivo del estado del relé para escritura
  File f=SPIFFS.open(statusRelayFile,"w");
  f.write((int)statusRelay); //Guardamos el estado del led en el archivo
  f.close(); //Cerramos el archivo
}

//Función de gestión de eventos de Alexa
void alexaSwitch(byte b) {
  //Sólo tiene un parámetro con el brillo
  //Convertiremos el brillo a encendido o apagado y lo aplicaremos el pin del relé
  statusRelay=b>127;
  setStatus(); //Aplicamos el estado actual
}

void setup() {
  //SPIFFS.format();  //Sólo la primera vez!
  pinMode(pinRelay,OUTPUT); //Configuramos pin de relé
  digitalWrite(pinRelay,LOW); //Inicialmente apagado
  SPIFFS.begin(); //Inicializamos sistema de archivos
  readStatus();
  wifiSetup(); //Conectamos a wifi

  alexa.addDevice("caldera",alexaSwitch); //Creamos el dispositivo
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  if(button.pressed()) {
    statusRelay=!statusRelay;
    setStatus();
  }
  delay(1);
}
