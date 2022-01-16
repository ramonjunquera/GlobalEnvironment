/*
  Autor: Ramón Junquera
  Fecha: 20210121
  Tema: Buzzer
  Objetivo: Demo de reproducción
  Material: M5Stack Fire, StickC, StickC+, Speaker Hat (opcional)

  Descripción:
  Al pulsar el primero de los botones se enviará el contenido del buffer al pin
  DAC integrado a través del driver I2S.
  Previamente llenaremos el buffer con el resultado de una función senoidal.
  Utilizaremos un buffer con muestras de 16bits (int16_t).
  Solo generaremos muestras para un canal.

  Nota:
  - Aunque los DAC integrados tienen una resolución de 8 bits, el driver I2S no
    permite utilizar muestras de 8 bits. El tamaño más pequeño tiene 16 bits.
    Sólo se tienen en cuenta los primeros 8 bits de cada muestra (MSB)
  - En el modelo StickC+, el buzzer interno está conectado al pin 2, pero los dos circutos
    DAC están asociados a los pines 25 y 26. Por lo tanto, NO podemos utilizar el buzzer
    interno para reproducir ondas. Es la razón por la que utilizamos un buzzer pasivo
    externo conectado al pin 26 o el hat de speaker. Esta solución es válida para los
    modelos StickC y StickC+.
  - En el modelo StickC y StickC+ se puede acoplar el hat de speaker.
    Este detalle se indicará en el archivo Platformio.ini
*/

#include <Arduino.h>
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S
#ifdef SPEAKERHAT
  #include <Wire.h> //Para gestión de chip de alimentación AXP192
#endif

//Contantes globales
const uint32_t samples=20000; //Número de muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz
//Nota: 20000 muestras a 10KHz son 2 segundos

#ifdef ARDUINO_M5STACK_FIRE
  const byte pinBuzzer=25;
  RoJoSwitch buttonA(39); //Primer botón
#elif ARDUINO_M5Stick_Watch
  const byte pinBuzzer=25;
  RoJoSwitch buttonA(35);
#elif ARDUINO_M5Stick_C
  const byte pinBuzzer=26; //Pin del buzzer o del speaker hat
  RoJoSwitch buttonA(37); //Botón M5
#endif

//Variables globales
uint16_t sampleBuffer[samples];

//Configura el canal 0 I2S para enviar datos al speaker
void configSpeaker() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para speaker
    //Parámetros:
    //  I2S_MODE_MASTER : Generaremos nosotros mismos la señal de reloj
    //  I2S_MODE_TX : La señal será de envío
    //  I2S_MODE_DAC_BUILT_IN : La señal se enviará a los DAC integrados
    //Nota: Como los DAC integrados tienen una resolución de 8 bits, no se tiene en cuenta
    //el tamaño de la muestra. Sólo se entregan al DAC los 8 primeros bits y el resto
    //se descartan.
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = freq, //Frecuencia
    //No se admiten muestras de 8 bits :-( 16 bits es el tamaño de muestra más pequeño
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    //Enviamos todas las muestras como si fuesen del canal izquierdo 
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
    //Imprescindible indicar que sólo se tendrán en cuenta los primeros bits de la muestra
    //con valor I2S_COMM_FORMAT_I2S_MSB. Si indicamos que se tienen en cuenta todos con
    //I2S_COMM_FORMAT_I2S, no se enviará nada.
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
    Serial.println("Error instalando driver I2S");
    vTaskDelete(NULL);
  }
  switch (pinBuzzer) { //Activamos el DAC que corresponde al pin del buzzer
    case 25:
      i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
      break;
    case 26:
      i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN);
      break;
  }
  //No es imprescindible configurar el reloj...
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
    Serial.println("Error asignando reloj I2S");
    vTaskDelete(NULL);
  }
}

void setup() {
  Serial.begin(115200);
  #ifdef SPEAKERHAT
    //Activamos la alimentación del speaker hat
    Wire.begin(21,22);
    Wire.beginTransmission(0x34);
    Wire.write(0x12);  
    Wire.write(0x4D); //Enable DC-DC1, OLED_VDD, 5B V_EXT
    Wire.endTransmission();
  #endif
  //Llenamos el buffer con una onda senoidal
  //Utilizamos samples de tipo uint16_t
  //Tomamos como amplitud 0 la mitad del valor máximo (32768).
  //Tomamos como amplitud máxima su complemento hasta el valor máximo (32768+32767=65535)
  //Recordemos que por restricciones del DAC integrado, sólo se tendrán en cuenta los 8 bits
  //de mayor peso (MSB).
  for(uint32_t sample=0;sample<samples;sample++) sampleBuffer[sample]=32768.0+32767.0*sin((double)sample/8);
  Serial.println("Ready!");
}

void loop() {
  if (buttonA.pressed()) {
    Serial.print("Playing...");
    configSpeaker();
    size_t bytesRead;
    //Puesto que las muestras son de 16 bits, debemos multiplicar por 2 para convertir en bytes
    i2s_write(I2S_NUM_0,(void*)sampleBuffer,samples*2,&bytesRead,100);
    i2s_driver_uninstall(I2S_NUM_0); //Desinstalamos el driver I2S
    i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE); //Desactivamos el DAC para evitar el ruido blanco
    Serial.println("Stop");
  }
  delay(10);
}
