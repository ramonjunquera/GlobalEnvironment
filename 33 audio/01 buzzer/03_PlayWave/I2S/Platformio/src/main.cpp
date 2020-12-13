/*
  Autor: Ramón Junquera
  Fecha: 20201208
  Tema: Buzzer
  Objetivo: Demo de reproducción en speaker I2S
  Material: M5Stack Atom Echo

  Descripción:
  Al pulsar el botón, vamos a reproducir por el buzzer el contenido de un
  buffer de memoria.
  Previamente llenaremos el buffer con el resultado de una función senoidal.
  Utilizaremos una conexión I2S a un speaker para transmitir los datos.
  Configuraremos tdos los detalles de la conexión I2S y enviaremos.
  Después desinstalaremos el driver.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para escribir en speaker

//Contantes globales
const uint32_t sampleSize=20000; //Número máximo de muestras
const uint32_t freq=10000; //Frecuencia de muestreo
const byte pinSpeakerLRCK=33;
const byte pinSpeakerBCLK=19;
const byte pinSpeakerDATA=22;

//Objetos globales
RoJoSwitch buttonA(39);

//Variables globales
byte *sampleBuffer=nullptr; //Puntero a buffer
size_t byteswrite; //Bytes escritos en I2S

void setup() {
  Serial.begin(115200); delay(2000);
  
  sampleBuffer=new byte[sampleSize];
  if(sampleBuffer==nullptr) Serial.println("Error reservando memoria");
  else Serial.println("Reserva de memoria OK");

  //Llenamos el buffer con una onda senoidal
  for(uint32_t sample=0;sample<sampleSize;sample++) {
    sampleBuffer[sample]=127+127.0*sin((double)sample/8);
  }
  Serial.println("Ready!");
}

void loop() {
  if (buttonA.pressed()) {
    Serial.print("Playing..");
    i2s_config_t i2s_config = { //Definición de configuración de I2S para speaker
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = freq, //Frecuencia
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Único tamaño permitido
      .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 2,
      .dma_buf_len = 128,
    };
    i2s_pin_config_t pin_config={ //Definición de configuración de pines
      .bck_io_num = pinSpeakerBCLK,
      .ws_io_num = pinSpeakerLRCK,
      .data_out_num = pinSpeakerDATA,
      .data_in_num = I2S_PIN_NO_CHANGE //No hay pin de datos de entrada
    };
    if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
      Serial.println("Error instalando driver I2S");
    }
    if(i2s_set_pin(I2S_NUM_0,&pin_config)) {
      Serial.println("Error asignando pines I2S");
    }
    if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
      Serial.println("Error asignando reloj I2S");
    }
    i2s_write(I2S_NUM_0, sampleBuffer, sampleSize, &byteswrite, portMAX_DELAY);
    i2s_driver_uninstall(I2S_NUM_0); //Desinstalamos el driver I2S
    Serial.println("Stop");
  }
  delay(10);
}
