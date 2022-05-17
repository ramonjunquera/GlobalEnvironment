/*
  Tema: Librería RMT
  Objetivo: Receptor de varios pulsos iguales con RMT
  Fecha: 20220503
  Autor: Ramón Junquera

  Descripción:
    Se recibirán 3 pulsos iguales y seguidos de 30ms, espaciados por 20ms.
    Utilizarmos un clk_div fijo a 80 para que cada tick represente 1us.
  Resultado:
    Recibimos los 3 pulsos seguidos correctamente.
    La duración no es exácta, pero sí muy aproximada:
      - pulso alto 30001us
      - pulso bajo 19997us
  Notas
  - El pulso bajo del último item tiene una duración de 0 porque supera cualquier timeout. Es correcto.
  - Se recomienda reducir al máximo la luz ambiental para evitar interferencias.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=32;
const byte clk_div=80; //1 tick = 1us

rmt_config_t configRMT;
RingbufHandle_t rb=NULL; //Buffer circular de entrada

void setup() {
  Serial.begin(115200);

  //Definición de la configuración de RMT
  configRMT.rmt_mode=RMT_MODE_RX; //Recibiremos
  configRMT.channel=RMT_CHANNEL_0; //Seleccionamos el canal
  configRMT.gpio_num=(gpio_num_t)pinLed;
  configRMT.clk_div=clk_div; //Valor calculado
  configRMT.mem_block_num=1; //Sólo un item
  configRMT.rx_config.filter_en=true; //Activamos el filtro de entrada de ruido
  configRMT.rx_config.filter_ticks_thresh=100; //Los pulsos menores de 100 ticks se descartan
  //La máxima duración de un pulso es de 30000 ticks
  //Necesitamos esperar al menos ese tiempo para que se reciba el pulso completo
  //Por eso lo subimos hasta 31000 ticks
  configRMT.rx_config.idle_threshold=31000; 

  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  //Activamos el canal
  //Definimos el tamaño del buffer de recepción
  //Un item de pulso ocupa 11 bytes
  //Pretendemos recibir 3 seguidos
  //Necesitaremos un buffer de al menos 3*11=33 bytes
  //Si ponemos menos, el buffer se desbordará dando un error.
  //Es 0 final es porque no utilizaremos flags especiales
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,33,0));
  rmt_get_ringbuf_handle(configRMT.channel,&rb); //Obtenemos el puntero del buffer circular de entrada
  ESP_ERROR_CHECK(rmt_rx_start(configRMT.channel,1)); //Activamos la recepción reseteando al canal
  Serial.println("config end");
}

void loop() {
  size_t rx_size=0; //Número de items recibidos
  //Leemos del buffer circular de entrada los items que contiene ahora mismo. No esperamos.
  rmt_item32_t* items=(rmt_item32_t*)xRingbufferReceive(rb,&rx_size,0); 
  if(items) { //Si hemos podido leer algún item...
    //rx_size contiene los elementos de los items de pulso leidos
    //Como un item contiene 4 elementos (level0,duration0,level1,duration1), el total de items
    //leidos es la cuarta parte de rx_size.
    printf("%u : received %i items\n",millis(),rx_size/4);
    for (size_t i=0;i<rx_size/4;i++) { //Recorremos todos los items leidos
      //Mostramos el contenido de cada uno de los items
      printf("  level0 %i, duration0 %i, level1 %i, duration1 %i\n",items[i].level0,items[i].duration0,items[i].level1,items[i].duration1);
    }
    vRingbufferReturnItem(rb,(void*)items); //Liberamos los items leidos del buffer circular
  }
}