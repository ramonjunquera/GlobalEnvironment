/*
  Autor: Ramón Junquera
  Fecha: 20210122
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
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel
#include <driver/i2s.h> //Protocolo I2S

//Contantes globales
const char* wifiSSID = "ESPserver";
const char* wifiPassword = "12345678";
const byte pinBuzzer=25; //Buzzer interno
const uint32_t freq=16000; //Frecuencia de muestreo 16KHz
const byte maxQueueMessages=5; //Número máximo de mensajes de la cola
const uint16_t samplesPerBlock=512;
const uint16_t bytesPerBlock=samplesPerBlock*2;

//Variables globales
WiFiServer server(8266); //Servidor en puerto 8266
RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
QueueHandle_t q=xQueueCreate(maxQueueMessages,bytesPerBlock); //Cola que guarda bloques

//Configura el canal 0 I2S para enviar datos al speaker
void configSpeaker() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para speaker
    //Parámetros:
    //  I2S_MODE_MASTER : Generaremos nosotros mismos la señal de reloj
    //  I2S_MODE_TX : La señal será de envío
    //  I2S_MODE_DAC_BUILT_IN : La señal se enviará a los DAC integrados
    //Nota: Como los DAC integrados tienen una resolución de 8 bits, no se tiene en cuenta
    //el tamaño de la muestra. Sólo se entregan al DAC los 8 primeros bits y el resto
    //se descartan.
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = freq, //Frecuencia
    //No se admiten muestras de 8 bits :-( 16 bits es el tamaño de muestra más pequeño
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    //Enviamos todas las muestras como si fuesen del canal izquierdo 
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
    //Imprescindible indicar que sólo se tendrán en cuenta los primeros bits de la muestra
    //con valor I2S_COMM_FORMAT_I2S_MSB. Si indicamos que se tienen en cuenta todos con
    //I2S_COMM_FORMAT_I2S, no se enviará nada.
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
    Serial.println("Error instalando driver I2S");
    vTaskDelete(NULL);
  }
  switch (pinBuzzer) { //Activamos el DAC que corresponde al pin del buzzer
    case 25:
      i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
      break;
    case 26:
      i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);
      break;
  }
  //No es imprescindible configurar el reloj...
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
    Serial.println("Error asignando reloj I2S");
    vTaskDelete(NULL);
  }
}

//Tarea de recepción
void taskReceiving(void *param) {
  WiFiClient client;
  byte buffer[bytesPerBlock]; //Buffer de muestras
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
      //Hemos recibido el bloque completo
      //Cada sample es de 1 byte
      //Debemos convertirlos a 16 bits para que sea reproducido por I2S sobre el DAC
      //Sólo se tienen en cuenta los 8 bits de mayor peso (MSB)
      //Recorremos todos los samples en orden inverso
      //Convertimos de 8 a 16 bits y guardamos en array como si fuese de uint16_t
      for(uint16_t i=samplesPerBlock;i>0;i--) ((uint16_t*)buffer)[i-1]=buffer[i-1]<<8;
      xQueueSend(q,buffer,0); //Añadimos el bloque a la cola. Si no hay sitio, se pierde.
    }
  }
}

//Tarea de reproducción
void taskPlaying(void *param) {
  byte buffer[bytesPerBlock]; //Buffer de muestras
  while(1) { //Bucle infinito
    //Recuperamos siguiente mensaje de la lista y copiamos las muestras al buffer.
    //Si no hay mensajes, esperaremos.
    xQueueReceive(q,buffer,portMAX_DELAY);
    //Reproducimos buffer en speaker
    size_t bytesRead;
    i2s_write(I2S_NUM_0,(void*)buffer,bytesPerBlock,&bytesRead,100);
  }
}

void setup() {
  Serial.begin(115200);
  //Configuración NeoPixel para M5Stack Fire. 5x2 en pin 15
  //Cada fila representa un lado: fila 0 = lado derecho, fila 1 = lado izquierdo
  //Las columnas representan el led.
  //En la derecha las columnas se cuentan de arriba a abajo
  //En el lado izquierdo se cuentan de abajo arriba.
  leds.begin(5,2,15);
  leds.v->clear({50,50,50}); leds.draw(); //Leds en blanco. Config begin
  configSpeaker(); //Inicializamos buzzer
  WiFi.mode(WIFI_AP); //Se creará un punto de acceso wifi
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña del punto de acceso
  server.begin(); //Arrancamos el servidor
  leds.v->clear({0,5,0}); leds.draw(); //Leds en verde. Running
  xTaskCreatePinnedToCore(taskPlaying,"",6000,NULL,1,NULL,0); //Tarea de reproducción en core 0
  xTaskCreatePinnedToCore(taskReceiving,"",6000,NULL,1,NULL,1); //Tarea de recepción en core 1
  Serial.println("Ready!");
  vTaskDelete(NULL); //Borramos la tarea del programa principal. Nunca se ejecutará loop()
}

void loop() {
  //Nada que hacer aquí
}
