/*
  Autor: Ramón Junquera
  Fecha: 20201227
  Tema: Remote Speaker
  Objetivo: Demo de reproducción continua de audio remota (servidor)
  Material: M5Stack Fire

  Descripción:
  Crea un punto de acceso wifi.
  Crea un servidor en el puerto 8266/tcp.
  Comprueba contínuamente si hay algún cliente.
  Si lo hay recibe su información y la reproduce por el speaker.
  El tamaño de muestra es fijo.

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

//Variables globales
float period_s=1.0/(float)freq; //Periodo en segundos
WiFiServer server(8266); //Servidor en puerto 8266
RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
QueueHandle_t q=xQueueCreate(7,sizeof(byte*)); //Cola con 7 nodos máximo que guarda punteros

//Tarea de recepción
void taskReceiving(void *param) {
  WiFiClient client;
  while(1) { //Bucle infinito
    client=server.available(); //Anotamos si hay algún cliente
    if(client) { //Si detectamos algún cliente...
      byte *buffer=new byte[samplesPerBlock]; //Dimensionamos y reservamos memoria para el buffer
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
      //Si no se han recibido todos los datos...rellenamos el resto de 127 (silencio)
      if(samplesReceived<samplesPerBlock) for(uint16_t i=samplesReceived;i<samplesPerBlock;i++) buffer[i]=127;
      //Añadimos un nodo a la cola (si no hemos llegado al máximo) con el puntero del buffer
      //Una cola copia el contenido de la dirección indicada, por lo tanto debemos indicar el
      //puntero del puntero del buffer!
      xQueueSend(q,&buffer,portMAX_DELAY);
    }
  }
}

//Tarea de reproducción
void taskPlaying(void *param) {
  byte *buffer; //Puntero a buffer de muestras recibidas
  while(1) { //Bucle infinito
    if(uxQueueMessagesWaiting(q)==0) dacWrite(pinBuzzer,0);
    //Recuperamos siguiente nodo de la lista (si existe) y tomamos el puntero al buffer de muestras
    xQueueReceive(q,&buffer,portMAX_DELAY);
    //Reproducimos buffer en speaker
    //No silenciamos el epeaker al terminar para evitar el click que produce
    RoJoI2Sa_write(0,buffer,samplesPerBlock,false);
    //Si no tenemos pendiente más reproducciones...apagamos el speaker
    //Esto evita que el speaker que quede encendido y generando ruido.
    if(uxQueueMessagesWaiting(q)==0) dacWrite(pinBuzzer,0);
    delete[] buffer; //Ya no necesitamos el buffer. Liberamos su memoria
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
  leds.v->clear(); leds.draw(); leds.draw(); //Apagamos leds. Config end
  xTaskCreatePinnedToCore(taskPlaying,"",2000,NULL,1,NULL,0); //Tarea de reproducción en core 0
  xTaskCreatePinnedToCore(taskReceiving,"",2000,NULL,1,NULL,1); //Tarea de recepción en core 1
  vTaskDelete(NULL); //Borramos la tarea del programa principal. Nunca se ejecutará loop()
}

void loop() {
  //Nada que hacer aquí
}
