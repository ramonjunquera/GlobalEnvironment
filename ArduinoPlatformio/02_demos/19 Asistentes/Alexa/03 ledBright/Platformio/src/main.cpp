/*
  Tema: librería Espalexa
  Fecha: 20230108
  Autor: Ramón Junquera
  Objetivo: Ejemplo básico de control de brillo

  Descripción del programa:
    Se utiliza un M5Stack Fire.
    Se definirá un dispositivo llamado bombilla.
    Se controlan los leds de NeoPixel.
    Se pueden apagar, encender o cambiar de brillo.
    No se pude cambiar el color. Siempre son blancos.
  
  Descripción de espalexa:
    Espalexa emula el comportamiento de una bombilla inteligente Philips HUE.
    Por eso cuando el dispositivo está conectado, vemos en sus propiedades que el fabricante
    es "Royal Philips Electronics smart device".
    Por lo tanto las propiedades de los dispositivos que creemos están limitados a los de la bombilla:
    - Encender/apagar
    - Nivel de brillo
    - ...

  Detección por Alexa:
    Para que Alexa detecte el nuevo dispositivo podemos pedirle a un echo que busque nuevos
    dispositivos, o en la aplicación de Alexa vamoa a la sección dispositivos, pulsamos el +
    para añadir, Añadir dispositivo, Otro, Wi-Fi y esperamos unos segundos.
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <WiFi.h>
#include <Espalexa.h>

const char* ssid="xxx";
const char* password="xxx";

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
Espalexa alexa; //Objeto de gestión de Alexa

//Fija color para todos los leds NeoPixel. Por defecto negro
void setColor(uint32_t color=0) {
  for(byte i=0;i<10;i++) leds.videoMem[i].setColor(color);
  leds.show();
}

//Conecta a wifi
void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) delay(100);
}

//Función de gestión de eventos de luz
//Sólo tiene un parámetro con el brillo
void alexaLuz(byte b) {
  setColor((b<<16)+(b<<8)+b); //Aplicamos el brillo a todos los canales de color
}

void setup() {
  //Serial.begin(115200); //DEBUG

  //Configuración NeoPixel para M5Stack Fire en pin 15
  //Serán 10 leds
  leds.begin(10,15);
  setColor(0x330000); //Leds en rojo mientras inicializa
  delay(500);
  setColor(0x333300); //Leds en amarillo mientras conecta wifi
  wifiSetup(); //Conectamos a wifi
  setColor(0x003300); //Leds en verde. Todo Ok
  delay(500);
  setColor(); //Apagados

  //Creamos el dispositivo. Le asignamos un nombre y una función de control
  alexa.addDevice("bombilla",alexaLuz); 
  alexa.begin(); //Iniciamos la gestión de dispositivos
}

void loop() {
  alexa.loop();
  delay(1);
}
