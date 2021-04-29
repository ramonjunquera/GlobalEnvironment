/*
  Autor: Ramón Junquera
  Fecha: 20201222
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción I2S
  Material: ESP32 WeMos Lolin D32, micrófono INM441, DAC I2S (PCM5120 o Max98357A)

  Descripción:
    Ejemplo de grabación y reproducción de audio.
    Tenemos dos pulsadores. Uno para grabar y otro para reproducir.
    La grabación se mantendrá mientras se mantenga pulsado el botón, hasta un máximo de 4s.
    Aprovechamos el led integrado de la placa para mostrar cuando está ocupado,
    ya sea grabando o reproduciendo.

    Tanto el micrófono como el DAC son I2S.
    Utilizaremos como DAC (decodificador digital analógico) un PCM5120 (DAC) o un
    Max98357A (DAC + amplificador).
    PCM5120 es mono y Max98357A stereo.
    Puesto que las muestras obtenidas del micrófono son mono, en ambos casos configuraremos
    la salida como mono.
    No necesitaremos hacer ninguna conversión de formatos.
    Utilizaremos la misma configuración y canal I2S para ambos.
    Compartiremos los pines de reloj y selección de canal. El circuito será más simple
    Crearemos la configuración I2S y la activaremos en la función setup().

    Después de la reproducción, puede que quede algo de información en la memoria
    dma de I2S y lo reproduzca cíclicamente.
    Para evitarlo tenemos 3 opciones:
    1 - Configurar e instalar el driver I2S (i2s_driver_install) justo antes de utilizarlo
        y desinstalarlo (i2s_driver_uninstall) después.
    2 - Activar el driver I2S (i2s_start) justo antes de utilizarlo y pararlo (i2s_stop)
        después.
    3 - Limpiar la memoria dma (i2s_zero_dma_buffer) tras la reproducción.
    Elección:
    1 - La instalación del driver I2S lleva unos instantes. Es importante que tras la
        pulsación del botón de grabación no se tarde mucho en comenzar, para no perder
        las muestras iniciales. Opción descartada.
    2 - La activación/desactivación son muy rápidas, pero al activar el driver I2S se
        produce una distorsión que dura un par de segundos que queda grabada e deteriora
        la calidad del sonido. Opción descartada.
    3 - Puesto que el driver se instala y activa en el setup(), no queda grabada la 
        interferencia inicial. El vaciado de la dma es muy rápido. Opción seleccionada.
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para leer micrófono

//Contantes globales
const byte pinLed=22;
const byte pinMicData=32;
const byte pinI2Sclock=26;
const byte pinI2Schannel=25;
const byte pinSpeakerData=23;
//Las muestras se recogerán por bloques
//Cada bloque será 1Kb de datos
//Puesto que una muestra son 2 bytes, un bloque contiene 1024/2=512 muestras
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz
//Nota: 40960 muestras a 10KHz son 4.1 segundos (aprox).

//Creamos objeto de gestión
RoJoSwitch buttonRecord(33),buttonPlay(19);

//Variables globales
int16_t sampleBuffer[blocksMax*1024/2]; //Es la mitad de bloques, porque el tipo ocupa 2 bytes
uint32_t blocks=0; //Número actual de bloques
size_t bytesRead;

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
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //Utilizaremos muestras de 16bits
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, //Sólo leeremos el canal izquierdo
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128,
  };
  i2s_pin_config_t pin_config={ //Definición de configuración de pines
    .bck_io_num = pinI2Sclock, //Pin de reloj de micro
    .ws_io_num = pinI2Schannel, //Pin de selección de canal
    .data_out_num = pinSpeakerData, //No hay pin de datos de salida
    .data_in_num = pinMicData //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) blinkLed(); //Error instalando driver I2S
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) blinkLed(); //Error asignando pines I2S
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) blinkLed(); //Error asignando reloj I2S
}

void loop() {
  if(buttonRecord.pressed()) { //Si se ha pulsado el botón de grabación...
    blocks=0;
    digitalWrite(pinLed,LOW); //Encendemos el led
    //Nunca utilizaremos más de la mitad del buffer, porque después tenemos que convertir
    //las muestras en stereo
    while(buttonRecord.pressing() && blocks<blocksMax/2) {
      //Leemos el bloque
      //Es muy importante la conversión de tipos en el puntero de buffer, porque el array
      //original es de int16_t. Si fuese de bytes no haría falta.
      //Convertimos en uint32_t el puntero del array del buffer para poder operar con él.
      //Y al resultado final le aplicamos el tipo que espera la función (void*).
      i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*1024),1024,&bytesRead,100);
      blocks++; //Hemos recibido un bloque más
    }
    digitalWrite(pinLed,HIGH); //Apagamos el led
  }
  if(buttonPlay.pressed()) { //Si se ha pulsado el botón de reproducción...
    digitalWrite(pinLed,LOW); //Encendemos el led
    i2s_write(I2S_NUM_0,(void*)sampleBuffer,blocks*1024,&bytesRead,100); //Escribimos buffer completo
    i2s_zero_dma_buffer(I2S_NUM_0);
    digitalWrite(pinLed,HIGH); //Apagamos el led
  }
  delay(10);
}
