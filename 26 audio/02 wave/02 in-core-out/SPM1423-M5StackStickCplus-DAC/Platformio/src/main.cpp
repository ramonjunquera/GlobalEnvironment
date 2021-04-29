/*
  Autor: Ramón Junquera
  Fecha: 20201228
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción con librería RoJoI2Sa
  Material: M5Stack StickC+

  Descripción:
  Los modelos StickC y StickC+ de M5Stack incluyen un micrófono SPM1423.
  La recuperación de datos de este dispositivo se hace a través del protocolo I2S.
  Puesto que el micrófono es mono, se ha eliminado el pin del selector de canal
  de audio en su conexión interna, quedando sólo el de datos y el de reloj.
  Las muestras recibidas por I2S están en formato int16_t, con una precisión de 12bits.

  En estos mismos modelos, el buzzer interno está conectado a un pin que no coincide
  con la salida de DAC, por lo tanto no puede ser utilizado para la reproducción de ondas.
  Sólo podemos conectar un buzzer/speaker externo.

  Utilizaremos la librería RoJoI2Sa para gestionar el speaker/buzzer y simplificar
  el código.

  Se define el número de muestras, el tamaño del buffer y la frecuencia.
  Se inicializa el display para poder mostrar mensajes de error.
  Se crea la configuración I2S y sus pines.
  Se instala el driver I2S, se le asigna la configuración de pines y la de reloj.
  Se crea el buffer.
  Se configura el pin del buzzer externo como salida.
  Se muestran los elementos estáticos de pantalla.

  La grabación se mantendrá mientras se mantenga pulsado el botón, hasta un máximo de 4s.
*/

#include <Arduino.h>
#include <RoJoST7789V2.h> //Librería de gestión del display RoJoST7789V2
#include <RoJoSwitch.h> //Gestión de interruptores
#include <driver/i2s.h> //Protocolo I2S para leer micrófono
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos

//Contantes globales
const byte pinMicSDA=34;
const byte pinMICSCL=0;
const byte pinBuzzer=26; //Buzzer pasivo externo
//Las muestras se recogerán por bloques
//Cada bloque será 1Kb de datos
//Puesto que una muestra son 2 bytes, un bloque contiene 1024/2=512 muestras
const uint32_t blocksMax=80; //Número máximo de bloques. 80 = 80Kb = 80*1024=81920bytes = 80*512=40960muestras
const uint32_t freq=10000; //Frecuencia de muestreo = 10KHz

//Creamos objeto de gestión
RoJoST7789V2 display;
RoJoSwitch buttonM5(37),buttonSide(39); //Botones M5 y lateral

//Variables globales
int16_t sampleBuffer[blocksMax*1024/2]; //Es la mitad de bloques, porque el tipo ocupa 2 bytes
uint32_t blocks=0; //Número actual de bloques

void setup() {
  display.begin();
  display.rotation(1); //Botón M5 a la derecha
  
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
    .bck_io_num = I2S_PIN_NO_CHANGE, //No tenemos pin para reloj de canales (el micro es mono)
    .ws_io_num = pinMICSCL, //Pin de reloj de micro
    .data_out_num = I2S_PIN_NO_CHANGE, //No hay pin de datos de salida
    .data_in_num = pinMicSDA //Pin de datos de micro
  };
  if(i2s_driver_install(I2S_NUM_0,&i2s_config,0,NULL)) {
    display.printOver("/7x11.fon","ERROR instalando driver I2S",{255,255,255});
    vTaskDelete(NULL); //Eliminamos la tarea del programa principal. Fin
  }
  if(i2s_set_pin(I2S_NUM_0,&pin_config)) {
    display.printOver("/7x11.fon","ERROR asignando pines I2S",{255,255,255});
    vTaskDelete(NULL); //Eliminamos la tarea del programa principal. Fin
  }
  if(i2s_set_clk(I2S_NUM_0,freq,I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_MONO)) {
    display.printOver("/7x11.fon","ERROR asignando reloj I2S",{255,255,255});
    vTaskDelete(NULL); //Eliminamos la tarea del programa principal. Fin
  }

  //Gestionaremos el speaker a través del driver RoJoI2Sa para simplificar la
  //programación y no tener que trabajar con timers.
  //Inficamos timer, pin analógico de salida y frecuencia.
  RoJoI2Sa_begin(0,pinBuzzer,freq);
  
  display.disk(225,67,10,{255,0,0}); //Label REC
  display.triangleFill(105,5,125,15,105,25,{0,255,0}); //Label PLAY
  display.printOver("/7x11.fon","Demo grabacion/reproduccion",{255,255,255},0,40);
}

void loop() {
  if(buttonM5.pressed()) { //Si se ha pulsado el botón M5...
    blocks=0;
    size_t bytesRead;
    display.disk(115,95,35,{255,0,0}); //Disco rojo
    while(buttonM5.pressing() && blocks<blocksMax) {
      //Leemos el bloque
      //Es muy importante la conversión de tipos en el puntero de buffer, porque el array
      //original es de int16_t. Si fuese de bytes no haría falta.
      //Convertimos en uint32_t el puntero del array del buffer para poder operar con él.
      //Y al resultado final le aplicamos el tipo que espera la función (void*).
      i2s_read(I2S_NUM_0,(void*)((uint32_t)sampleBuffer+blocks*1024),1024,&bytesRead,100);
      blocks++; //Hemos recibido un bloque más
    }
    display.block(80,60,150,130,{0,0,0}); //Borramos disco rojo
    //Convirtiendo muestras de 16bits (int16_t) a 8bits (byte) con amplificación x32
    RoJoI2Sa_convert16to8(sampleBuffer,(byte*)sampleBuffer,blocks*512,32);
  }
  if(buttonSide.pressed() && blocks>0) { //Si se ha pulsado el botón lateral...
    display.triangleFill(80,60,150,95,80,130,{0,255,0}); //Triángulo verde
    RoJoI2Sa_write(0,(byte*)sampleBuffer,blocks*512); //Reproducimos buffer en speaker
    display.block(80,60,150,130,{0,0,0}); //Borramos triángulo verde
  }
  delay(10);
}
