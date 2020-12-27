/*
  Autor: Ramón Junquera
  Fecha: 20201227
  Tema: Remote Speaker
  Objetivo: Demo de reproducción continua de audio remota (cliente)
  Material: M5Stack Atom Echo

  Descripción:
  No se necesitan botones. Graba continuamente muestras que envía al servidor.
  Se separan las tareas de grabación y envío. Se asigna cada tarea a un core distinto.
  Las tareas se sincronizan con una cola de mensajes.
  - Grabación: recupera un bloque de muestras del micrófono y se lo entrega a la cola de
    mensajes
  - Envío: recupera mensaje de la cola, obtiene las muestras del micrófono, las convierte
    a 8bits y las envía al servidor
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en Atom
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <WiFi.h> //Para transferir muestras por wifi

//Contantes globales
const char* wifiSSID = "ESPserver"; //Nombre del punto de acceso
const char* wifiPassword = "12345678"; //Contraseña del punto de acceso
const char* host = "192.168.4.1"; //Nombre o IP del host
const byte pinMicData=23;
const byte pinMicClock=33;
const uint32_t freq=16000; //Frecuencia de muestreo = 16KHz
const uint16_t samplesPerBlock=512;
const byte maxQueueMessages=5; //Número máximo de mensajes de la cola

//Variables globales
RoJoNeoPixelAtomLite led;
QueueHandle_t q=xQueueCreate(maxQueueMessages,samplesPerBlock*2);

//Deja parpadeando el led en azul
void error() {
  while(1) {
    led.draw({0,0,255}); delay(300);
    led.draw(); delay(300);
  }
}

//Aseguramos conexión wifi con el punto de acceso
void checkWifi() {
  if(WiFi.status() != WL_CONNECTED) { //Si no está conectado al servidor...
    WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
    while (WiFi.status() != WL_CONNECTED) delay(50); //Esperamos mientras no esté conectado
  }
}

//Configura el canal 0 I2S para recibir datos del micrófono
void configMic() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para micrófono
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será recepción = I2S_MODE_RX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Único tamaño permitido
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 6,
    .dma_buf_len = 60
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = I2S_PIN_NO_CHANGE, //No tenemos pin para selector de canales (el micro es mono)
    .ws_io_num = pinMicClock, //Pin de reloj de micro
    .data_out_num = I2S_PIN_NO_CHANGE, //No hay pin de datos de salida
    .data_in_num = pinMicData //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) error(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) error(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) error(); //Error asignando reloj I2S
}

//Tarea de conversión y envío
void taskSending(void *param) {
  int16_t buffer16[samplesPerBlock]; //Buffer de muestras de micrófono
  byte buffer8[samplesPerBlock]; //Buffer de muestras convertidas
  WiFiClient client;
  while(1) { //Bucle infinito
    //Recuperamos siguiente mensaje de la lista y el buffer de micrófono
    //No no hay mensaje, esperamos hasta que lo haya
    xQueueReceive(q,(byte*)buffer16,portMAX_DELAY);
    //Convertimos muestras de int16_t a byte sin añadir ninguna ganancia
    for(uint16_t i=0;i<samplesPerBlock;i++) buffer8[i]=constrain((float)buffer16[i]/256.0+127.0,0,255);
    checkWifi(); //Nos aseguramos que tenemos conexión wifi con el punto de acceso
    if(client.connect(host,8266)) { //Si conseguimos conectar con el servidor...
      client.write(buffer8,samplesPerBlock); //Enviamos el bloque de muestras convertidas
      client.stop(); //Terminamos la conexión
    }
    //Si no hemos conseguido conectar con el servidor, no enviamos este bloque y lo perdemos.
  }
}

//Tarea de grabación
void taskRecording(void *param) {
  size_t bytesRead;
  int16_t buffer16[samplesPerBlock]; //Buffer de muestras de micrófono
  while(1) { //Bucle infinito
    i2s_read(I2S_NUM_0,(void*)buffer16,samplesPerBlock*2,&bytesRead,100); //Leemos bloque de muestras
    //Añadimos un nuevo mensaje a la cola con una copia del buffer recogido del micrófono
    //Si la cola está llena, perderemos el bloque
    xQueueSend(q,buffer16,0);
  }
}

void setup() {
  led.begin(); //Inicialización de led
  led.draw({200,200,200}); //Led en blanco mientras inicializa
  configMic(); //Inicialización I2C de micrófono
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
  checkWifi(); //Conectar wifi ahora para no perder tiempo después
  led.draw({0,50,0}); //Led verde. Running
  xTaskCreatePinnedToCore(taskSending,"",6000,NULL,1,NULL,0); //Tarea de envío en core 0
  xTaskCreatePinnedToCore(taskRecording,"",6000,NULL,1,NULL,1); //Tarea de grabación en core 1
  vTaskDelete(NULL); //Borramos la tarea del programa principal. Nunca se ejecutará loop()
}

void loop() {
  //Nada que hacer aquí
}
