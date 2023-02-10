/*
  Tema: Librería Espalexa
  Fecha: 20230123
  Autor: Ramón Junquera
  Objetivo: Informar de cambio de estado a servidor Alexa
  Descripción:
    El programa crea un dispositivo Alexa asociado al led integrado.
    Cuando se pide encender (cualquier brillo superior a cero), se enciende el
    led interno, pero calcula el momento en el que debería apagarse (3 segundos
    más tarde).
    En el loop principal, comprueba continuamente si el led está encendido.
    Si lo está comprueba si ha llegado el momento de apagarse.
    Si es el momento hace dos cosas:
    - Llamar a la función de callback que ajusta el led integrado al nuevo estado
    - Llamar al método .setValue() de la estructura que guarda el dispositivo Alexa
      para que el nuevo valor sea informado al servidor Alexa.
    Con esto conseguimos sincronizar el cambio de estado originado por el propio
    programa, tanto a nivel local (led) como a nivel externo (servidor de Alexa).
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Espalexa.h>

const char* ssid="xxx";
const char* password="xxx";

Espalexa alexa; //Objeto de gestión de Alexa
const byte pinLed=LED_BUILTIN;
uint32_t timeOver=0; //Momento en el que debe apagarse solo
bool status=false; //Estado del led
const uint32_t timeOverFlow=-1-1000*60*60*24; //Un día antes del máximo
EspalexaDevice *alexaDevice=NULL; //Puntero a dispositivo Alexa

//Conecta a wifi
void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  //Hace parpadear el led mientras lo intenta
  while (WiFi.status()!=WL_CONNECTED) {
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(100);
  } 
}

//Función de gestión de eventos de luz
//Sólo tiene el brillo como parámetro
void alexaLuz(byte bright) {
  status=bright;
  if(status) { //Encender...
    timeOver=millis()+3000; //Se apagará dentro de 3 segundos
  }
  digitalWrite(pinLed,!status); //El estado del led está invertido respecto de su voltaje
}

void setup() {
  pinMode(pinLed,OUTPUT);
  wifiSetup(); //Conectamos a wifi
  digitalWrite(pinLed,HIGH); //Led apagado

  //Creamos el dispositivo. Le asignamos un nombre y una función de control
  //La librería nos permite crear varios dispositivos Alexa en el mismo programa.
  //Cada uno de ellos independiente de los demás.
  //Se guardarán en un array interno en el mismo orden que se crean comenzando
  //por el 0.
  //El método .addDevice devuelve el índice del dispositivo creado, mas 1.
  //Si devuelve 0, consideraremos que ha ocurrido un error.
  //En este ejemplo, como sólo hay un dispositivo Alexa, ya sabemos que tendrá
  //como índice el 0, pero lo guardamos en alexaIndex.
  byte alexaIndex=alexa.addDevice("bombilla",alexaLuz)-1;
  //Obtenemos el puntero de la estructura del dispositivo Alexa que acabamos de crear.
  alexaDevice=alexa.getDevice(alexaIndex);
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  if(status) { //Si está encendido...
    if(timeOver-millis()>timeOverFlow) { //Si es hora de apagar...
      alexaLuz(0); //Llamamos a la función de callback para que se apaque el led
      //La llamada anterior sólo actua a nivel local.
      //Necesitamos informar al servidor de Alexa del nuevo estado llamando al método
      //setValue() de la estructura del dispositivo Alexa que guardamos al principio.
      alexaDevice->setValue(0);
    }
  }
  delay(1);
}
