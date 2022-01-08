/*
  Autor: Ramón Junquera
  Fecha: 20201219
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
    La grabación se mantendrá mientras se mantenga pulsado el botón, hasta un máximo de 4s.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos

//Contantes globales
const byte pinLed=22;
const byte pinMicSDA=32; //Datos I2S
const byte pinMICSCL=14; //Reloj I2S
const byte pinMICWS=15; //Reloj de canal I2S
const byte pinBuzzer=25;
//Las muestras se recogerán por bloques
//Cada bloque será 1Kb de datos
//Puesto que una muestra son 2 bytes, un bloque contiene 1024/2=512 muestras
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objeto de gestión
RoJoSwitch buttonRecord(33),buttonPlay(26);

//Variables globales
int16_t sampleBuffer[blocksMax*1024/2]; //Es la mitad de bloques, porque el tipo ocupa 2 bytes
uint32_t blocks=0; //Número actual de bloques

//Función de error. El led parpadea continuamente.
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
    blocks=0;
    size_t bytesRead;
    digitalWrite(pinLed,LOW); //Encendemos el led
    while(buttonRecord.pressing() && blocks<blocksMax) {
      //Leemos el bloque
      //Es muy importante la conversión de tipos en el puntero de buffer, porque el array
      //original es de int16_t. Si fuese de bytes no haría falta.
      //Convertimos en uint32_t el puntero del array del buffer para poder operar con él.
      //Y al resultado final le aplicamos el tipo que espera la función (void*).
      i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*1024),1024,&bytesRead,100);
      blocks++; //Hemos recibido un bloque más
    }
    digitalWrite(pinLed,HIGH); //Apagamos el led
    //Convirtiendo muestras de 16bits (int16_t) a 8bits (byte) con amplificación x32
    RoJoI2Sa_convert16to8(sampleBuffer,(byte*)sampleBuffer,blocks*512,25);
  }
  if(buttonPlay.pressed()) { //Si se ha pulsado el botón de reproducción...
    digitalWrite(pinLed,LOW); //Encendemos el led
    RoJoI2Sa_write(0,(byte*)sampleBuffer,blocks*512); //Reproducimos buffer en speaker
    digitalWrite(pinLed,HIGH); //Apagamos el led
  }
  delay(10);
}
