/*
  Autor: Ramón Junquera
  Fecha: 20201225
  Tema: Remote Speaker
  Objetivo: Demo de reproducción de audio remota (servidor)
  Material: M5Stack Fire

  Descripción:
  Crea un punto de acceso wifi.
  Crea un servidor en el puerto 8266/tcp.
  Comprueba contínuamente si hay algún cliente.
  Si lo hay recibe su información y la reproduce por el speaker.

  Formato de los datos recibidos.
  Primero se recibe un byte con el número de bloques.
  Cada bloque son 512 bytes (o muestras).
  Las muestras ya vienen en formato byte y preparadas para ser reproducidas en el speaker.
  
  Dejamos de recibir muestras si llegamos al total de muestras esperadas o perdemos
  la comunicación con el cliente.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

//Contantes globales
const char* wifiSSID = "ESPserver";
const char* wifiPassword = "12345678";
const uint32_t freq=10000; //Frecuencia de muestreo
const byte pinBuzzer=25; //Buzzer interno
byte *sampleBuffer; //Buffer de muestras

//Variables globales
float period_s=1.0/(float)freq; //Periodo en segundos
WiFiServer server(8266); //Servidor en puerto 8266
RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel

void setup() {
  Serial.begin(115200);
  //Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
  RoJoI2Sa_begin(0,pinBuzzer,freq); //Innicializamos buzzer
  //Creamos el punto de acceso
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor
  leds.v->clear({0,0,50}); leds.draw(); leds.draw(); //Leds en azul (ready)
}

void loop() {
  WiFiClient client=server.available(); //Anotamos si hay algún cliente
  if(client) { //Si detectamos algún cliente...
    leds.v->clear({50,0,0}); leds.draw(); //Leds en rojo (recibiendo)
    uint32_t samples=512*client.read(); //Leemos el número de bloques y calculamos número de muestras
    Serial.println("samples="+String(samples));
    sampleBuffer=new byte[samples]; //Dimensionamos buffer
    //Nota. La recepción no es instantánea. La información recibida se guarda inicialmente en
    //el buffer de recepción wifi que tiene unas 15Kb.
    //Si la información esperada es mayor que el tamaño del buffer de recepción se perderá.
    //Para evitarlo, necesitamos leer este buffer tantas veces como sea necesario (para descargarlo)
    //y guardarlo en la posición correcta de nuestro buffer de muestras.
    uint32_t samplesReceived=0; //Aun no hemmos recibido ninguna muestra
    //Mientras no tengamos todas las muestras...seguiremos leyendo el buffer de entrada del wifi
    while(samplesReceived<samples && client.connected()) {
      //Cuando no se reciben datos, client.read() devuelve -1
      int bytesReceived=client.read(&sampleBuffer[samplesReceived],samples-samplesReceived);
      //Si se han recibido datos...se tienen en cuenta
      if(bytesReceived>0) samplesReceived+=bytesReceived;
    }
    client.stop(); //Desconectamos
    Serial.println("samplesReceived="+String(samplesReceived));
    leds.v->clear({0,50,0}); leds.draw(); //Leds en verde (reproduciendo)
    RoJoI2Sa_write(0,sampleBuffer,samplesReceived); //Reproducimos buffer en speaker
    leds.v->clear({0,0,50}); leds.draw(); //Leds en azul (ready)
    delete[] sampleBuffer; //Liberamos memoria del buffer
  }
  delay(10);
}
