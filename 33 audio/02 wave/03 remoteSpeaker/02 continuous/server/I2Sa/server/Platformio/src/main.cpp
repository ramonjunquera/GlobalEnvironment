/*
  Autor: Ramón Junquera
  Fecha: 20201227
  Tema: Remote Speaker
  Objetivo: Demo de reproducción continua de audio remota (servidor)
  Material: M5Stack Fire

  Descripción:
  Crea un punto de acceso wifi.
  Crea un servidor en el puerto 8266/tcp.
  Se separan las tareas de recepción y reproducción. Se asigna cada tarea a un core distinto.
  Las tareas se sincronizan a través de una cola de mensajes.
  - Recepción: comprueba si hay algún cliente, recupera el bloque de muestras y se lo entrega
    a la cola de mensajes.
  - Reproducción: recupera mensaje de la cola, obtiene el bloque de muestras y las reproduce
    por el speaker.

  Nota:
  Se evita utilizar los leds mientras se lanzan otras tareas.
  La librería de control de leds no está preparada para multitarea.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

//Contantes globales
const char* wifiSSID = "ESPserver";
const char* wifiPassword = "12345678";
const byte pinBuzzer=25; //Buzzer interno
const uint32_t freq=16000; //Frecuencia de muestreo 16KHz
const uint16_t samplesPerBlock=512;
const byte maxQueueMessages=5; //Número máximo de mensajes de la cola

//Variables globales
WiFiServer server(8266); //Servidor en puerto 8266
RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
QueueHandle_t q=xQueueCreate(maxQueueMessages,samplesPerBlock); //Cola que guarda bloques

//Tarea de recepción
void taskReceiving(void *param) {
  WiFiClient client;
  byte buffer[samplesPerBlock]; //Buffer de muestras
  while(1) { //Bucle infinito
    client=server.available(); //Anotamos si hay algún cliente
    if(client) { //Si detectamos algún cliente...
      //Nota. La recepción no es instantánea. La información recibida se guarda inicialmente en
      //el buffer de recepción wifi que tiene unas 15Kb.
      //Si la información esperada es mayor que el tamaño del buffer de recepción se perderá.
      //Para evitarlo, necesitamos leer este buffer tantas veces como sea necesario (para descargarlo)
      //y guardarlo en la posición correcta de nuestro buffer de muestras.
      uint16_t samplesReceived=0; //Aun no hemmos recibido ninguna muestra
      //Mientras no tengamos todas las muestras...seguiremos leyendo el buffer de entrada del wifi
      while(samplesReceived<samplesPerBlock && client.connected()) {
        //Cuando no se reciben datos, client.read() devuelve -1
        int bytesReceived=client.read(&buffer[samplesReceived],samplesPerBlock-samplesReceived);
        //Si se han recibido datos...se tienen en cuenta
        if(bytesReceived>0) samplesReceived+=bytesReceived;
      }
      client.stop(); //Desconectamos
      xQueueSend(q,buffer,0); //Añadimos el bloque a la cola. Si no hay sitio, se pierde.
    }
  }
}

//Tarea de reproducción
void taskPlaying(void *param) {
  byte buffer[samplesPerBlock]; //Buffer de muestras
  while(1) { //Bucle infinito
    //Recuperamos siguiente mensaje de la lista y copiamos las muestras al buffer.
    //Si no hay mensajes, esperaremos.
    xQueueReceive(q,buffer,portMAX_DELAY);
    //Reproducimos buffer en speaker
    //No silenciamos el speaker al terminar para evitar el click que produce
    RoJoI2Sa_write(0,buffer,samplesPerBlock,false);
    //Si no tenemos pendiente más bloques...apagamos el speaker
    //Esto evita que el speaker que quede encendido y generando ruido.
    if(uxQueueMessagesWaiting(q)==0) dacWrite(pinBuzzer,0);
  }
}

void setup() {
  //Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
  leds.v->clear({50,50,50}); leds.draw(); //Leds en blanco. Config begin
  RoJoI2Sa_begin(0,pinBuzzer,freq); //Innicializamos buzzer
  WiFi.mode(WIFI_AP); //Se creará un punto de acceso wifi
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña del punto de acceso
  server.begin(); //Arrancamos el servidor
  leds.v->clear({0,5,0}); leds.draw(); //Leds en verde. Running
  xTaskCreatePinnedToCore(taskPlaying,"",6000,NULL,1,NULL,0); //Tarea de reproducción en core 0
  xTaskCreatePinnedToCore(taskReceiving,"",6000,NULL,1,NULL,1); //Tarea de recepción en core 1
  vTaskDelete(NULL); //Borramos la tarea del programa principal. Nunca se ejecutará loop()
}

void loop() {
  //Nada que hacer aquí
}
