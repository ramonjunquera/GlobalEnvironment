/*
  Autor: Ramón Junquera
  Fecha: 20201217
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción a través de I2S
  Material: M5Stack Atom Echo

  Descripción:
  El modelo Atom Echo tiene tanto el micrófono como el speaker conectados por I2C.
  Ambos son mono.
  No procesamos las muestras obtenidas del micrófono para enviarlas al speaker.
  Puesto que sólo tiene un botón, distinguimos si se trata de una grabación o una
  reproducción por el tiempo de pulsación:
  - Pulsación corta = grabación
  - Pulsación larga = reproducción
  Utilizaremos sólo el canal 0 de I2S. Lo configuraremos antes de cada uso como
  entrada (micro) o salida (speaker).
  Utilizaremos el led RGB:
  - Destello azul para indicar que se ha inicializado
  - Rojo = grabando
  - Verde = reproduciendo
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoNeoPixelAtomLite.h> //Gestión de led NeoPixel
#include <driver/i2s.h> //Protocolo I2S para leer micrófono

//Contantes globales
const byte pinMicData=23;
const byte pinMicClock=33;
const byte pinSpeakerData=22;
const byte pinSpeakerClock=19;
const byte pinSpeakerChannel=33;
const uint32_t samples=20000; //Número de muestras = 20000 -> 2s a 10KHz
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objetos de gestión
RoJoNeoPixelAtomLite led;
RoJoSwitch button(39);

//Variables globales
int16_t sampleBuffer[samples];

//Deja parpadeando el led en rojo
void error() {
  while(1) {
    led.draw({255,0,0}); delay(300);
    led.draw(); delay(300);
  }
}

//Configura el canal 0 I2S para recibir datos del micrófono
void configMic() {
  i2s_driver_uninstall(I2S_NUM_0);
  i2s_config_t i2s_config={ //Definición de configuración de I2S para micrófono
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será recepción = I2S_MODE_RX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Único tamaño permitido
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 128
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

//Configura el canal 0 I2S para enviar datos al speaker
void configSpeaker() {
  i2s_driver_uninstall(I2S_NUM_0);
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
    .bck_io_num = pinSpeakerClock, //Pin de reloj de speaker
    .ws_io_num = pinSpeakerChannel, //Pin de selector de canales del speaker
    .data_out_num = pinSpeakerData, //Pin de datos de salida
    .data_in_num = I2S_PIN_NO_CHANGE //No hay pin de datos de entrada (micro)
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) error(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) error(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) error(); //Error asignando reloj I2S
}

void setup() {
  led.begin(); //Inicialización de led
  //Indicamos que estamos preparados encendiendo el led en azul durante un momento
  led.draw({0,0,255}); //Led en azul
  delay(500);
  led.draw(); //Led apagado
}

void loop() {
  if(button.pressed()) { //Si se ha pulsado el botón...
    size_t bytesRead;
    uint32_t now=millis();
    uint32_t timeOut=now+500; //Dentro de medio segundo
    //Esperaremos hata que se suelte o caduque...
    while(now<timeOut && button.pressing()) now=millis();
    if(now<timeOut) { //Pulsación corta = grabación
      led.draw({255,0,0}); //Led en rojo
      configMic(); //Inicialización de micrófono
      i2s_read(I2S_NUM_0,sampleBuffer,samples*2,&bytesRead,100); //Leemos buffer completo
      led.draw();//Led apagado
    } else { //Pulsación larga = reproducción
      led.draw({0,255,0}); //Led en verde
      configSpeaker(); //Inicialización de speaker
      i2s_write(I2S_NUM_0,sampleBuffer,samples*2,&bytesRead,100); //Escribimos buffer completo
      led.draw();//Led apagado
    }
  }
}
