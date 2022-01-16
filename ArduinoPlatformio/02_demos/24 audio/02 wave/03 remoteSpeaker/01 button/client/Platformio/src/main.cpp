/*
  Autor: Ramón Junquera
  Fecha: 20201225
  Tema: Remote Speaker
  Objetivo: Demo de reproducción de audio remota (cliente)
  Material: M5Stack Atom Echo

  Descripción:
  Tenemos un único botón para grabar y reproducir.
  Para diferenciar la función, tendremos en cuenta la duración de la pulsación.
  Una pulsación larga activará la grabación.
  La grabación se mantendrá mientras se mantenga pulsado el botón o hasta que se
  consuma toda la memoria dispoible (buffer).
  Atención: la grabación no comienza cuando se pulsa el botón, sino unas décimas
  de segundo después después. Necesita tiempo para distinguir si se trata de una
  pulsación corta de reproducción o larga de grabación.
  Puesto que sólo tenemos un dispositivo I2S (micrófono), definiremos la configuración
  I2C en el setup() y no la modificaremos ni desactivaremos a lo largo de la ejecución.
  Una vez obtenidas las muestras del micrófono en el buffer, las convertiremos de
  int16_t a byte que es lo máximo que admite el speaker de destino (en el Fire).
  Además ocupan la mitad y tardaremos menos en transferirlas.

  Puesto que podemos tener desconexiones del servidor, cada vez nos soliciten
  reproducir, comprobaremos si existe conexión. Si no la hay, la estableceremos en
  ese mismo momento.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en Atom
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <WiFi.h> //Para transferir muestras por wifi

//Contantes globales
const char* wifiSSID = "ESPserver"; //Nombre del punto de acceso
const char* wifiPassword = "12345678"; //Contraseña del punto de acceso
const char* host = "192.168.4.1"; //Nombre o IP del host
const byte pinMicData=23;
const byte pinMicClock=33;
//Las muestras se recogerán por bloques
//Cada bloque será 1Kb de datos
//Puesto que una muestra son 2 bytes, un bloque contiene 1024/2=512 muestras
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz
//40960 muestras a una frecuencia de 10KHz son: 40960/10000=4s

//Creamos objetos de gestión
RoJoNeoPixelAtomLite led;
RoJoSwitch button(39);
WiFiClient client;

//Variables globales
int16_t sampleBuffer[blocksMax*1024/2]; //Es la mitad de bloques, porque el tipo ocupa 2 bytes
uint32_t blocks=0; //Número actual de bloques

//Deja parpadeando el led en azul
void error() {
  while(1) {
    led.draw({0,0,255}); delay(300);
    led.draw(); delay(300);
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

void setup() {
  led.begin(); //Inicialización de led
  configMic(); //Inicialización I2C de micrófono
  WiFi.mode(WIFI_STA); //Conectado como cliente a un punto de acceso
}

void loop() {
  uint32_t now=millis();
  if(button.pressed()) { //Si se ha pulsado el botón...
    button.released(); //Borramos el estado de release del botón
    uint32_t recordingMillis=now+200;
    size_t bytesRead;
    //Esperamos hasta que se suelte el botón (reproducir) o se llegue al timeout (grabar)
    while(!button.released() && now<recordingMillis) now=millis();
    if(now<recordingMillis) { //Reproducción..
      if(blocks>0) { //Si hay algo grabado...
        if(WiFi.status() != WL_CONNECTED) { //Si no está conectado al servidor...
          WiFi.begin(wifiSSID,wifiPassword); //Solicitamos conexión
          while (WiFi.status() != WL_CONNECTED) delay(50); //Esperamos mientras no esté conectado
        } //Seguro que estamos conectados al servidor
        if(client.connect(host,8266)) { //Si podemos establecer conexión con el servidor...
          led.draw({0,255,0}); //Led en verde
          client.write(blocks); //Enviamos el número de bloques
          client.write((byte*)sampleBuffer,blocks*512); //Enviamos las muestras recogidas
          client.stop(); //Terminamos la conexión
          led.draw(); led.draw(); //Led apagado
        }
      }
    } else { //Grabación...
      blocks=0;
      led.draw({255,0,0}); //Led en rojo
      while(button.pressing() && blocks<blocksMax) {
        //Leemos el bloque
        //Es muy importante la conversión de tipos en el puntero de buffer, porque el array
        //original es de int16_t. Si fuese de bytes no haría falta.
        //Convertimos en uint32_t el puntero del array del buffer para poder operar con él.
        //Y al resultado final le aplicamos el tipo que espera la función (void*).
        i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*1024),1024,&bytesRead,100);
        blocks++; //Hemos recibido un bloque más
      }
      led.draw();//Led apagado
      //Convertimos muestras a 8 bits
      for(uint32_t i=0;i<blocks*512;i++) ((byte*)sampleBuffer)[i]=constrain((float)sampleBuffer[i]/256.0*16.0+127.0,0,255);
    }
  }
  delay(10);
}
