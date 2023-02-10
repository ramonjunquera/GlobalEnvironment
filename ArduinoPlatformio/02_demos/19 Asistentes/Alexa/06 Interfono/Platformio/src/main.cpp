//Tema: AutoOpen para interfono
//Fecha: 20230125
//Autor: Ramón Junquera

//Nota. millis() devuelve un uint32_t que puede almacenar valores de 49 días.
//Es posible que el dispositivo está más tiempo encendido.
//Tendremos que tener cuidado cuando hagamos comparaciones de tiempos asumiendo que 
//haya oicurrido un overflow.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Espalexa.h>

const char* ssid="xxx";
const char* password="xxx";

const byte pinButtonOpen=0;
const byte pinLed=1;
const byte pinButtonDisplay=2;
const byte pinCalling=3;

Espalexa alexa; //Objeto de gestión de Alexa
EspalexaDevice *alexaDevice=NULL; //Dispositivo Alexa
bool calling=false; //Inicialmente no estamos recibiendo una llamada
bool autoOpen=false; //Inicialmente no abrimos automáticamente
uint32_t displayTimeOut=0; //Momento en el que la pantalla estará encendida
uint32_t callingTimeOut=0; //Momento en el que finalizará la llamada
uint32_t autoOpenTimeOut=0; //Momento en el que se desactivará el autoOpen
const uint32_t maxTimeOut=-1-1000*60*60*24; //Un día antes del máximo
byte status=0; //Estado. 0=reposo,1=esperando pantalla,2=esperando fin de llamada

//Conecta a wifi
void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  //Hace parpadear el led mientras lo intenta
  while (WiFi.status()!=WL_CONNECTED) {
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  }
  digitalWrite(pinLed,LOW); //Led apagado
}

//Función de control de Alexa
//El parámetro es la intensidad de luz
void alexaFunc(byte b) {
  autoOpen=b; //Anotamos nuevo estado
  //Si nos piden activar...calculamos el momento en el que se desactivará
  if(autoOpen) autoOpenTimeOut=millis()+10000; //Estará activo durante 10 segundos
  digitalWrite(pinLed,autoOpen); //Led representa el estado de autoOpen
}

//Apaga autoOpen
void autoOpenOff() {
  alexaFunc(0); //Desactivamos autoOpen y apagamos el led
  alexaDevice->setValue(0); //Informamos al servidor Alexa
}

//Pulsación corta del botón indicado
void pressButton(byte pin) {
  digitalWrite(pin,LOW); //Pulsamos
  delay(200);
  digitalWrite(pin,HIGH); //Soltamos
}

void setup() {
  pinMode(pinLed,OUTPUT);
  pinMode(pinCalling,INPUT); //No necesitamos resistencias pull

  pinMode(pinButtonOpen,OUTPUT);
  digitalWrite(pinButtonOpen,HIGH); //Botón no pulsado

  pinMode(pinButtonDisplay,OUTPUT);
  digitalWrite(pinButtonDisplay,HIGH); //Botón no pulsado

  wifiSetup(); //Conectamos a wifi
  
  alexa.addDevice("interfono",alexaFunc); //Añadimos dispositivo Alexa con índice 0
  alexaDevice=alexa.getDevice(0); //Recuperamos estructura de dispositivo Alexa de índice 0
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

//Bucle principal para detectar llamadas y lanzar la secuencia a apertura
void loop() {
  alexa.loop(); //Gestión de peticiones de Alexa
  if(calling) { //Si estamos recibiendo una llamada...
    //Si hemos superado el tiempo de fin de llamada...ya no hay llamada
    if(callingTimeOut-millis()>maxTimeOut) calling=false;
  }
  if(!digitalRead(pinCalling)) { //Si se detecta una llamada...
    callingTimeOut=millis()+2000; //Anotamos que la llamada finalizará en 2 segundos
    calling=true; //Nos están llamando
  }
  if(autoOpen) { //Si tenemos activo el autoOpen...
    //Si hemos superado el tiempo de autoOpen...lo apagamos
    if(autoOpenTimeOut-millis()>maxTimeOut) autoOpenOff();
  }
  switch(status) {
    case 0: //En reposo
      if(calling & autoOpen) { //Si están llamando y debemos descolgar...
        pressButton(pinButtonDisplay); //Descolgamos
        autoOpenOff(); //Apagamos autoOpen
        displayTimeOut=millis()+7000; //Tenemos que esperar 7 segundos hasta que se encienda la pantalla
        status=1; //Esperando a que se encienda la pantalla
      }
      break;
    case 1: //Esperando que se encienda la pantalla
      if(displayTimeOut-millis()>maxTimeOut) { //Si ya se ha encendido la pantalla...
        pressButton(pinButtonOpen); //Abrimos
        status=2; //Esperando a que finalice la llamada
      }
      break;
    case 2: //Esperando que finalice la llamada
      if(!calling) status=0; //Si la llamada ha finalizado...volvemos a reposo
      else { //Si la llamada no ha finalizado...
        if(autoOpen) { //Si recibimos nueva petición de apertura...
          //...no la tendremos en cuenta, porque no se puede abrir de nuevo hasta que la pantalla
          //no se apague y finalice esta llamada.
          autoOpenOff(); //Apagamos autoOpen
        }
      }
      break;
  }
  delay(1);
}
