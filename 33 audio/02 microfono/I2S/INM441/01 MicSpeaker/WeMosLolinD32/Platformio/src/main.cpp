/*
  Autor: Ramón Junquera
  Fecha: 20201216
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción con librería RoJoI2Sa
  Material: ESP32 WeMos Lolin D32, micrófono INM441, buzzer pasivo.

  Descripción:
    Utilizaremos el micrófono I2S INM441 para recoger una colección de muestras y
    un simple buzzer para reproducirlas.
    Incluimos dos interruptores de pulsador. Uno para grabar y otro para reproducir.
    Aprovechamos el led integrado de la placa para mostrar cuandpo está ocupado,
    ya sea grabando o reproduciendo.
    Para el micrófono, usaremos los pines sugeridos para ESP32.
    Para el buzzer utilizaremos uno de los dos pines DAC que disponemos.
    Dejamos sin conectar el pin L/R para recoger sólo las muestras del canal izquierdo.
    Configuramos I2S para utilizar muestras de 16bits que se reciben en formato int16_t.
    Antes de reproducirlo en el buzzer, debemos convertirlas a formato byte, que es
    lo único que permite un pin DAC.
    Es importante que el buzzer sea pasivo, para evitar ruido adicional.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos

//Contantes globales
const byte pinLed=22;
const byte pinMicSDA=32;
const byte pinMICSCL=14;
const byte pinMICWS=15;
const byte pinBuzzer=25;
const uint32_t samples=20000; //Número de muestras = 20000 -> 2s a 10KHz
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objeto de gestión
RoJoSwitch buttonRecord(33),buttonPlay(26);

//Variables globales
int16_t sampleBuffer[samples]; //Las muestras serán de 16bits

//Función de eroor. El led parpadea continuamente.
void blinkLed() {
  while(1) {
    digitalWrite(pinLed,!digitalRead(pinLed));
    delay(300);
  }
}

void setup() {
  pinMode(pinLed,OUTPUT);
  digitalWrite(pinLed,HIGH); //led apagado

  i2s_config_t i2s_config={ //Definición de configuración de I2S para micrófono
    //Pedimos que se encargue de generar la señal de reloj = I2S_MODE_MASTER
    //La dirección de la comunicación será recepción = I2S_MODE_RX
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Utilizaremos muestras de 16bits
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //Sólo leeremos el canal izquierdo
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = pinMICSCL, //Pin de reloj de micro
    .ws_io_num = pinMICWS, //Pin de selección de canal
    .data_out_num = I2S_PIN_NO_CHANGE, //No hay pin de datos de salida
    .data_in_num = pinMicSDA //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) blinkLed(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) blinkLed(); //Error asignando pines I2S

  //Gestionaremos el speaker a través del driver RoJoI2Sa para simplificar la
  //programación y no tener que trabajar con timers.
  //Indicamos timer, pin analógico de salida y frecuencia.
  RoJoI2Sa_begin(0,pinBuzzer,freq);
}

void loop() {
  if(buttonRecord.pressed()) { //Si se ha pulsado el botón de grabación...
    digitalWrite(pinLed,LOW); //Encendemos el led
    size_t bytesRead;
    i2s_read(I2S_NUM_0,sampleBuffer,samples*2,&bytesRead,100); //Leemos buffer completo
    digitalWrite(pinLed,HIGH); //Apagamos el led
    //Convirtiendo muestras de 16bits (int16_t) a 8bits (byte) con amplificación x25
    RoJoI2Sa_convert16to8(sampleBuffer,(byte*)sampleBuffer,samples,25);
  }
  if(buttonPlay.pressed()) { //Si se ha pulsado el botón de reproducción...
    digitalWrite(pinLed,LOW); //Encendemos el led
    RoJoI2Sa_write(0,(byte*)sampleBuffer,samples); //Reproducimos buffer en speaker
    digitalWrite(pinLed,HIGH); //Apagamos el led
  }
  delay(10);
}
