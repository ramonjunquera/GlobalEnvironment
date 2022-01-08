/*
  Autor: Ramón Junquera
  Fecha: 20210122
  Tema: Micrófono
  Objetivo: Demo de grabación de ADC y reproducción en DAC con I2S
  Material: M5Stack Fire

  Descripción:
  Dados unos parámetros iniciales:
  - Pines de micrófono y buzzer
  - Número de muestras
  - Frecuencia de muestreo
  Creamos un buffer para almacenar todas las muestras.
  Asignamos funionalidad a los dos primeros botones:
  - Botón A : grabación
    Configuración de micrófono
    Solicitamos la recuperación de muestras del micrófono.
    Desconfiguración del micrófono
    Convertimos muestras de 12 a 16 bits
  - Botón B : reproducción
    Configuración del speaker
    Solicitamos el envío de las muestras al buzzer
    Desconfiguración del speaker

  La grabación se mantendrá mientras se mantenga pulsado el botón, hasta un máximo de 4s.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Gestión de sprites color
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S

//Contantes globales
const byte pinMic=34;
const byte pinBuzzer=25;
//Las muestras se recogerán por bloques
//Cada bloque será 1Kb de datos
//Puesto que una muestra son 2 bytes, un bloque contiene 1024/2=512 muestras
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo

//Creamos objeto de gestión
RoJoILI9341 display;
RoJoSwitch buttonA(39),buttonB(38); //Botones en M5Stack Fire

//Variables globales
uint16_t sampleBuffer[blocksMax*1024/2]; //Es la mitad de bloques, porque el tipo ocupa 2 bytes
uint32_t blocks=0; //Número actual de bloques

//Configura el canal 0 I2S para recibir datos del micrófono
//Nota: Sólo se puede leer un ADC por el canal 0 de I2S!
void configMic() {
  i2s_config_t i2s_config={ //Definición de configuración de I2S para micrófono
    //Parámetros:
    //  I2S_MODE_MASTER : Generaremos nosotros mismos la señal de reloj
    //  I2S_MODE_RX : La señal será de recepción
    //  I2S_MODE_DAC_BUILT_IN : La señal se leerá de un ADC integrado
    //Nota: Los ADC integrados tienen una resolución de 12 bits
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = freq, //Frecuencia
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    //Enviamos todas las muestras como si fuesen del canal izquierdo 
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
    Serial.println("Error instalando driver I2S para micrófono");
    vTaskDelete(NULL);
  }
  switch (pinMic) { //Activamos el ADC que corresponde al pin del micrófono
    case 36:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_0);
      break;
    case 37:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_1);
      break;
    case 38:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_2);
      break;
    case 39:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_3);
      break;
    case 32:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_4);
      break;
    case 33:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_5);
      break;
    case 34:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_6);
      break;
    case 35:
      i2s_set_adc_mode(ADC_UNIT_1,ADC1_CHANNEL_7);
      break;
  }
  //No es imprescindible configurar el reloj...
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
    Serial.println("Error asignando reloj I2S para micrófono");
    vTaskDelete(NULL);
  }
}

//Configura el canal 0 I2S para enviar datos al speaker
//Nota: Sólo se puede escribir en un DAC por el canal 0 de I2S!
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
    Serial.println("Error instalando driver I2S para speaker");
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
    Serial.println("Error asignando reloj I2S para speaker");
    vTaskDelete(NULL);
  }
}

void setup() {
  Serial.begin(115200);
  //Inicializamos el display para M5Stack Fire
  //RES=33,DC=27,CS=14,BackLight=32
  display.begin(33,27,14,32);
  display.rotation(1); //Botonera abajo
  display.disk(65,225,10,{255,0,0}); //Label REC
  display.triangleFill(150,215,170,225,150,235,{0,255,0}); //Label PLAY
  display.printOver("/10x22.fon","Demo grabacion/reproduccion",{255,255,255},25);
}

void loop() {
  size_t bytesRead;
  if(buttonA.pressed()) { //Si se ha pulsado el botón de grabación...
    blocks=0;
    display.disk(160,120,60,{255,0,0}); //Mostramos icono REC
    configMic();
    while(buttonA.pressing() && blocks<blocksMax) {
      //Leemos el bloque en la posición que le corresponde
      i2s_read(I2S_NUM_0,(void*)&sampleBuffer[blocks*512],1024,&bytesRead,100);
      blocks++; //Hemos recibido un bloque más
    }
    display.block(100,60,220,180,{0,0,0}); //Borramos icono REC
    i2s_driver_uninstall(I2S_NUM_0); //Desinstalamos el driver I2S
    //Convertimos muestras de 12 a 16 bits
    for(uint16_t i=0;i<blocks*512;i++) sampleBuffer[i]<<=4; //Subimos los 4 bits de diferencia
  } else if (buttonB.pressed()) { //Si se ha pulsado el botón de reproducción...
    display.triangleFill(100,60,220,120,100,180,{0,255,0}); //Mostramos icono PLAY
    configSpeaker();
    i2s_write(I2S_NUM_0,(void*)sampleBuffer,blocks*1024,&bytesRead,100);
    display.block(100,60,220,180,{0,0,0}); //Borramos icono PLAY
    i2s_driver_uninstall(I2S_NUM_0); //Desinstalamos el driver I2S
    i2s_set_dac_mode(I2S_DAC_CHANNEL_DISABLE); //Desactivamos el DAC para evitar el ruido blanco
  }
  delay(10);
}
