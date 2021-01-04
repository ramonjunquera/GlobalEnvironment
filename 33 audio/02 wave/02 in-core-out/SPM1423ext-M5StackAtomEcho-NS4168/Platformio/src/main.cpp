/*
  Autor: Ramón Junquera
  Fecha: 20201231
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción a través de I2S
  Material: M5Stack Atom Echo

  Descripción:
  En vez de utilizar el micrófono interno, conectaremos uno externo.
  Esto nos permitirá utilizar los dos canales I2S disponibles.
  Asignaremos el 0 al micrófono y el 1 al speaker.
  Puesto que no se interfieren, declararemos la configuración una sola vez
  en el setup() y no la modificaremos en la ejecución.

  No procesamos las muestras obtenidas del micrófono para enviarlas al speaker.
  Puesto que sólo tiene un botón, distinguimos si se trata de una grabación o una
  reproducción por el tiempo de pulsación:
  - Pulsación corta = reproducción
  - Pulsación larga = grabación
  La grabación se mantendrá mientras se mantenga pulsado el botón o hasta que se
  consuma toda la memoria dispoible (buffer).
  Atención: la grabación no comienza cuando se pulsa el botón, sino unas décimas
  de segundo después después. Necesita tiempo para distinguir si se trata de una
  pulsación corta de reproducción o larga de grabación.
  Utilizaremos el led RGB:
  - Azul parpadeando = error
  - Destello blanco para indicar que se ha inicializado
  - Rojo = grabando
  - Verde = reproduciendo
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel en Atom
#include <driver/i2s.h> //Protocolo I2S para leer micrófono

//Contantes globales
const byte pinI2SchannelMic=32; //Micrófono externo
const byte pinI2SdataMic=26; //Micrófono externo
const byte pinI2SchannelSpeaker=33; //Speaker interno
const byte pinI2SdataSpeaker=22; //Speaker interno
const byte pinI2SclockSpeaker=19; //Speaker interno
const uint32_t bytesPerBlock=1024;
const uint32_t blocksMax=80;
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objetos de gestión
RoJoNeoPixelAtomLite led;
RoJoSwitch button(39);

//Variables globales
byte sampleBuffer[blocksMax*bytesPerBlock];
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
    .bck_io_num = I2S_PIN_NO_CHANGE, //Pin de reloj de bits
    .ws_io_num = pinI2SchannelMic, //Pin de reloj de canal
    .data_out_num = I2S_PIN_NO_CHANGE, //Pin de datos de speaker
    .data_in_num = pinI2SdataMic //Pin de datos de micro
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

void setup() {
  Serial.begin(115200);
  led.begin(); //Inicialización de led
  configSpeaker(); //Inicialización de speaker
  configMic(); //Inicialización de micrófono
  //Encendemos el led en blanco un instante para indicar que está inicializado
  led.draw({255,255,255});
  delay(500);
  led.draw();
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
      Serial.print("reproduciendo...");
      led.draw({0,255,0}); //Led en verde
      i2s_write(I2S_NUM_1,sampleBuffer,blocks*bytesPerBlock,&bytesRead,100); //Escribimos buffer completo
      led.draw();//Led apagado
    } else { //Grabación...
      Serial.print("grabando...");
      blocks=0;
      led.draw({255,0,0}); //Led en rojo
      while(button.pressing() && blocks<blocksMax) {
        //Leemos el bloque
        //Es muy importante la conversión de tipos en el puntero de buffer, porque el array
        //original es de int16_t. Si fuese de bytes no haría falta.
        //Convertimos en uint32_t el puntero del array del buffer para poder operar con él.
        //Y al resultado final le aplicamos el tipo que espera la función (void*).
        i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*bytesPerBlock),bytesPerBlock,&bytesRead,100);
        blocks++; //Hemos recibido un bloque más
      }
      led.draw();//Led apagado
      Serial.print(String(blocks)+"blocks...");
    }
    Serial.println("Ok");
  }
}
