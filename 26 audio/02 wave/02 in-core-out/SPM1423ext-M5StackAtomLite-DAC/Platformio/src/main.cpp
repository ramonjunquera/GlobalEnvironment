/*
  Autor: Ramón Junquera
  Fecha: 20201230
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción con librería RoJoI2Sa
  Material: M5Stack Atom Lite, micrófono SPM1423 externo, buzzer pasivo externo

  Descripción:
  Utilizamos el micrófono SPM1423 externo y lo conectamos al puerto groove.
  El buzzer pasivo lo conectamos al puerto corto que está debajo, que contiene el pin
  25 que es uno de los dos asociados a DAC.

  Utilizaremos la librería RoJoI2Sa para gestionar el speaker/buzzer y simplificar
  el código.

  Una pulsación larga permite grabar y una corta reproducir.
  La grabación se mantendrá mientras se pulse el botón, o hasta que se graben todos los bloques.
*/

#include <Arduino.h>
#include <RoJoNeoPixelAtomLite.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos

//Contantes globales
const byte pinI2SdataMic=26;
const byte pinI2SchannelMic=32;
const byte pinBuzzer=25; //Buzzer pasivo externo
const uint32_t samplesPerBlock=512;
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objeto de gestión
RoJoNeoPixelAtomLite led;
RoJoSwitch button(39);

//Variables globales
int16_t sampleBuffer[blocksMax*samplesPerBlock];
uint32_t blocks=0; //Número actual de bloques

//Deja parpadeando el led en azul
void error() {
  while(1) {
    led.draw({0,0,255}); delay(300);
    led.draw(); delay(300);
  }
}

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
    .dma_buf_count = 2,
    .dma_buf_len = 128
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = I2S_PIN_NO_CHANGE, //No tenemos pin para reloj de bits
    .ws_io_num = pinI2SchannelMic, //Pin de reloj de canales de micrófono
    .data_out_num = I2S_PIN_NO_CHANGE, //No hay pin de datos de salida
    .data_in_num = pinI2SdataMic //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) error(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) error(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) error(); //Error asignando reloj I2S
}

void setup() {
  led.begin();
  configMic(); //Inicialización de micrófono
  RoJoI2Sa_begin(0,pinBuzzer,freq); //Inicialización de speaker
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
      led.draw({0,255,0}); //Led en verde
      if(blocks>0) RoJoI2Sa_write(0,(byte*)sampleBuffer,blocks*samplesPerBlock); //Reproducimos buffer en speaker
      led.draw();//Led apagado
    } else { //Grabación...
      blocks=0;
      led.draw({255,0,0}); //Led en rojo
      while(button.pressing() && blocks<blocksMax) {
        i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*samplesPerBlock*2),samplesPerBlock*2,&bytesRead,100);
        blocks++; //Hemos recibido un bloque más
      }
      led.draw();//Led apagado
      //Convirtiendo muestras de 16bits (int16_t) a 8bits (byte) con amplificación x32
      RoJoI2Sa_convert16to8(sampleBuffer,(byte*)sampleBuffer,blocks*samplesPerBlock,32);
    }
  }
  delay(10);
}
