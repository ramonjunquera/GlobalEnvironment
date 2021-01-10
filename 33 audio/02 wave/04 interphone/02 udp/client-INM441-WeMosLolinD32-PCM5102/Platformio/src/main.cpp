/*
  Autor: Ramón Junquera
  Fecha: 20210108
  Tema: Remote Speaker
  Objetivo: Demo de reproducción continua de audio remota (cliente)
  Material: Wemos Lolin D32, micrófono INM441, PCM5102
*/

#include <Arduino.h>
#include <driver/i2s.h> //Protocolo I2S
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h> //Protocolo mDNS

//Contantes globales
const char* wifiSSID = "ESPwifi"; //Nompre del punto de acceso wifi
const char* wifiPassword = "12345678"; //Contraseña del punto de acceso wifi
const char* hostnameLocal="interClientLolin"; //Nombre del dispositivo local
const char* hostnameRemote="interClientEcho"; //Nombre del dispositivo remoto
const byte pinI2SchannelMic=15;
const byte pinI2SdataMic=32;
const byte pinI2SclockMic=14;
const byte pinI2SchannelSpeaker=25;
const byte pinI2SdataSpeaker=23;
const byte pinI2SclockSpeaker=26;
const byte pinLed=22;
const uint32_t freq=16000; //Frecuencia de muestreo 16KHz
const uint16_t samplesPerBlock=256;
const byte maxQueueMessages=5; //Número máximo de mensajes en las colas
const uint16_t bytesPerBlock=samplesPerBlock*2; //Usaremos muestras de 16bits (int16_t)

//Variables globales
//Definimos cola alimentada por bloques recibidos para ser reproducida
QueueHandle_t qPlaying=xQueueCreate(maxQueueMessages,bytesPerBlock);
//Definimos cola alimentada por bloques grabados para ser enviados
QueueHandle_t qSending=xQueueCreate(maxQueueMessages,bytesPerBlock);
IPAddress ipRemote; //Dirección IP del otro dispositivo. Inicialmente no tenemos = 0.0.0.0
bool wifiConnected;

//Deja parpadeando el led integrado
void error() {
  while(1) {
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(200);
  }
}

//Comprueba conexión wifi
//Si no hay, lo intenta varias veces
//Devuelve true si consigue conectar
bool checkWifi() {
  if(WiFi.status()==WL_CONNECTED) return true; //Si está conectado...devolvemos true
  WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
  byte counter=255;
  do {
    counter--;
    delay(50);
  } while(WiFi.status()!=WL_CONNECTED && counter>0);
  return counter>0;
}

//Configura el canal 0 I2S para recibir datos del micrófono
void configMic() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para micrófono
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será recepción = I2S_MODE_RX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Único tamaño permitido
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //Sólo leeremos el canal izquierdo
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num=pinI2SclockMic, //Pin de reloj de bits
    .ws_io_num=pinI2SchannelMic, //Pin de reloj de canal
    .data_out_num=I2S_PIN_NO_CHANGE, //Pin de datos de speaker
    .data_in_num=pinI2SdataMic //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) error(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) error(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) error(); //Error asignando reloj I2S
}

//Configura el canal 1 I2S para enviar datos al speaker
void configSpeaker() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para speaker
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será envío = I2S_MODE_TX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Único tamaño permitido
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 128,
    .use_apll=false,
    .tx_desc_auto_clear=true
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = pinI2SclockSpeaker, //Pin de reloj de bits
    .ws_io_num = pinI2SchannelSpeaker, //Pin de reloj de canal
    .data_out_num = pinI2SdataSpeaker, //Pin de datos de speaker
    .data_in_num = I2S_PIN_NO_CHANGE //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_1,&i2s_config,0,NULL)) error(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_1,&pin_config)) error(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_1,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) error(); //Error asignando reloj I2S
}

//Tarea de recepción
void taskReceiving(void *param) {
  byte buffer[bytesPerBlock]; //Buffer de muestras. Realmente es int16_t
  WiFiUDP udp;
  udp.begin(8266); //Preparado para recibir paquetes udp por este puerto
  while(1) { //Bucle infinito
    int packetSize=udp.parsePacket(); //Comprobamos si hay paquetes nuevos
    if(packetSize>=0) { //Si hay algún paquete nuevo...
      if(packetSize==bytesPerBlock) { //Si tiene un tamaño correcto...
        uint16_t bytesReceived=0; //Aun no hemos recibido nada
        uint32_t usTimeout=micros()+3000; //No esperaremos más de 3 ms
        //Mientras no tengamos todas los bytes...seguiremos leyendo el buffer de entrada del wifi
        while(bytesReceived<bytesPerBlock && micros()<usTimeout) {
          //Cuando no se reciben datos, client.read() devuelve -1
          int bytesReceivedNow=udp.read(&buffer[bytesReceived],bytesPerBlock-bytesReceived);
          //Si se han recibido datos...se tienen en cuenta
          if(bytesReceivedNow>0) bytesReceived+=bytesReceivedNow;
        }
        //Si hemos recibido el bloque completo..lo añadimos a la cola de reproducción
        //Si no hay sitio en la cola, se pierde
        if(bytesReceived==bytesPerBlock) xQueueSend(qPlaying,buffer,0);
        udp.flush(); //Descartamos el posible contenido del buffer de recepción
      };
      udp.flush(); //Vaciamos el buffer para este paquete
    }
    delay(1); //Refresca WatchDogTimer
  }
}

//Tarea de reproducción
void taskPlaying(void *param) {
  byte buffer[bytesPerBlock];
  size_t bytesRead;
  while(1) { //Bucle infinito
    //Recuperamos siguiente mensaje de la lista y copiamos las muestras al buffer.
    //Si no hay mensajes, esperaremos.
    xQueueReceive(qPlaying,buffer,portMAX_DELAY);
    //Reproducimos buffer en speaker
    i2s_write(I2S_NUM_1,buffer,bytesPerBlock,&bytesRead,100);
  }
}

//Tarea de grabación
void taskRecording(void *param) {
  byte buffer[bytesPerBlock]; //Buffer de muestras
  size_t bytesRead;
  while(1) { //Bucle infinito
    i2s_read(I2S_NUM_0,buffer,bytesPerBlock,&bytesRead,100);
    //Añadimos el bloque a la cola de envío. Si no hay sitio, se pierde.
    xQueueSend(qSending,buffer,0);
  }
}

//Tarea de envío
void taskSending(void *param) {
  byte buffer[bytesPerBlock];
  WiFiUDP udp;
  while(1) { //Bucle infinito
    //Recuperamos siguiente mensaje de la lista y copiamos las muestras al buffer.
    //Si no hay mensajes, esperaremos.
    xQueueReceive(qSending,buffer,portMAX_DELAY);
    if(wifiConnected && ipRemote!=0) { //Si tenemos conexión wifi e identificado al otro dispositivo...
      udp.beginPacket(ipRemote,8266); //Configuración del paquete udp
      udp.write(buffer,bytesPerBlock); //Contenido del paquete
      udp.endPacket(); //Finalización y envío
    }
    //Si no tenemos conexión wifi, o no tenemos identificada la dirección ip del otro dispositivo,
    //perdemos el bloque de muestras
  }
}

void setup() {
  pinMode(pinLed,OUTPUT); //Inicialización de led integrado
  digitalWrite(pinLed,HIGH); //Led apagado
  configSpeaker();
  configMic();
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  wifiConnected=checkWifi(); //Conectar wifi ahora para no perder tiempo después
  MDNS.begin(hostnameLocal); //Publicamos el nombre local
  xTaskCreatePinnedToCore(taskPlaying,"",6000,NULL,1,NULL,0); //Tarea de reproducción en core 0
  xTaskCreatePinnedToCore(taskReceiving,"",6000,NULL,1,NULL,0); //Tarea de recepción en core 0
  xTaskCreatePinnedToCore(taskSending,"",6000,NULL,1,NULL,1); //Tarea de envío en core 1
  xTaskCreatePinnedToCore(taskRecording,"",6000,NULL,1,NULL,1); //Tarea de grabación en core 1
}

void loop() {
  bool anyChange=false;
  bool wifiConnectedNow=checkWifi();
  if(wifiConnected!=wifiConnectedNow) { //Si ha cambiado el estado...
    anyChange=true;
    wifiConnected=wifiConnectedNow;
  }
  if(wifiConnected) { //Si no hay conexión wifi...
    //Pedimos conversión de hostname a ip del dispositivo remoto
    IPAddress ip=MDNS.queryHost(hostnameRemote);
    if(ip!=ipRemote) { //Si la dirección obtenida es distinta a la que teníamos...
      anyChange=true;
      ipRemote=ip; //La actualizamos
    }
  }
  if(anyChange) digitalWrite(pinLed,!(wifiConnected && ipRemote!=0));
  delay(3000);
}
