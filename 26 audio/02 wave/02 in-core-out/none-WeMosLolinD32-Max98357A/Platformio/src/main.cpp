/*
  Autor: Ramón Junquera
  Fecha: 20210216
  Tema: Buzzer
  Objetivo: Demo de reproducción
  Material: WeMos Lolin D32, Max98357A

  Descripción:
  Al pulsar el botón se reproducirá enviará el contenido del buffer al Max98357A.
  Previamente llenaremos el buffer con el resultado de una función senoidal.
  Utilizaremos un buffer con muestras de 16bits (int16_t).
  Solo generaremos muestras para un canal.
  Para que suene por ambos, indicaremos que el speaker es mono.

  El Max98357A se configura para que tenga una ganancia de 12db y para que sólo
  reproduzca muestras del canal izquierdo.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S

//Contantes globales
const byte pinSpeakerChannel=25;
const byte pinSpeakerClock=26;
const byte pinSpeakerData=23;
const uint32_t samples=20000; //Número de muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz
//Nota: 20000 muestras a 10KHz son 2 segundos

//Creamos objeto de gestión
RoJoSwitch button(33);

//Variables globales
int16_t sampleBuffer[samples];

//Configura el canal 0 I2S para enviar datos al speaker
void configSpeaker() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para speaker
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será envío = I2S_MODE_TX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //Enviamos todas las muestras como si fuesen del canal izquierdo
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = pinSpeakerClock, //Pin de reloj de speaker
    .ws_io_num = pinSpeakerChannel, //Pin de selector de canales del speaker
    .data_out_num = pinSpeakerData, //Pin de datos de salida
    .data_in_num = I2S_PIN_NO_CHANGE //No hay pin de datos de entrada (micro)
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
    Serial.println("Error instalando driver I2S");
    vTaskDelete(NULL);
  }
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) {
    Serial.println("Error asignando pines I2S");
    vTaskDelete(NULL);
  }
  //Puesto que Max98357A sólo puede controlar un speaker, enviaremos en mono.
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
    Serial.println("Error asignando reloj I2S");
    vTaskDelete(NULL);
  }
}

void setup() {
  Serial.begin(115200);
  //Llenamos el buffer con una onda senoidal
  for(uint32_t sample=0;sample<samples;sample++) sampleBuffer[sample]=32767.0*sin((double)sample/8);
}

void loop() {
  if (button.pressed()) {
    Serial.print("Playing...");
    configSpeaker();
    size_t bytesRead;
    i2s_write(I2S_NUM_0,(void*)sampleBuffer,samples*2,&bytesRead,100);
    i2s_driver_uninstall(I2S_NUM_0);
    Serial.println("Stop");
  }
  delay(10);
}
