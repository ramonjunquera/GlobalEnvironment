/*
  Tema: Librería RMT
  Objetivo: Receptor de pulso simple con RMT
  Fecha: 20220504
  Autor: Ramón Junquera

  Descripción:
    Utilizamos la librería RMT para la recepción de pulsos simples.
    Sabemos que el pulso a recibir tiene una duración de 100ms.
    Calculamos clk_div & periodo para que nos coincida con el emisor.
    Los valores son:
      clk_div = 245
      duration = 32653
    Aplicamos la configuración de recepción con el clk_div calculado y lo ponemos en marcha.
    En el loop principal, comprobaremos continuamente si hay algún dato en el buffer circular de entrada.
    Si encontramos alguno, descargamos todos los items que contenga y lños mostramos por el
    puerto serie.
    Finalmente liberamos los items leidos del buffer circular.
  
  Notas:
  - Puesto que sabemos que los pulsos durarán, aproximadamente 32653 ticks, necesitamos indicar
    que en el tiempo de espera de un pulso es de al menos este valor (rx_config.idle_threshold).
  - Si un estado dura más de este número de ticks, se devolverá como duración 0. por esta razón
    el valor duration1 siempre es 0.
  - Destacar que los niveles de los pulsos están invertidos debido al receptor.
  - Hemos activado el filtro de ruido para que los pulsos inferiores a 100 ticks no se tengan en
    cuenta. Esta activación es opcional.
  
  Resultado:
    A cada recepción de pulso, se muestran sus detalles.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=32;
const float periodo=100000; //100ms=0.1s

rmt_config_t configRMT;
RingbufHandle_t rb=NULL; //Buffer circular de entrada

//Calcula los parámetros RMT para un periodo determinado
void calcParamsRMT(float periodo, byte &clk_div, uint32_t &duration) {
  clk_div=ceil(periodo*80/32767);
  duration=periodo*80/clk_div;
}

void setup() {
  Serial.begin(115200);

  //Calcularemos los parámetros
  byte clk_div;
  uint32_t duration;
  calcParamsRMT(periodo,clk_div,duration);
  Serial.printf("clk_div = %u\n",clk_div);
  Serial.printf("duration = %u\n",duration);

  //Definición de la configuración de RMT
  configRMT.rmt_mode=RMT_MODE_RX; //Recibiremos
  configRMT.channel=RMT_CHANNEL_0; //Seleccionamos el canal
  configRMT.gpio_num=(gpio_num_t)pinLed;
  configRMT.clk_div=clk_div; //Valor calculado
  configRMT.mem_block_num=1; //Sólo un item
  configRMT.rx_config.filter_en=true; //Activamos el filtro de entrada de ruido
  configRMT.rx_config.filter_ticks_thresh=100; //Los pulsos menores de 100 ticks se descartan
  //Esperamos un pulso de 32653 ticks
  //Necesitamos esperar al menos ese tiempo para que se reciba el pulso completo
  //Por eso lo subimos hasta 33000 ticks
  configRMT.rx_config.idle_threshold=33000; 

  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  //Activamos el canal
  //Definimos el tamaño del buffer de recepción
  //Un item de pulso ocupa 11 bytes
  //Necesitaremos un buffer de al menos ese tamaño
  //Si ponemos menos, el buffer se desbordará dando un error.
  //Es 0 final es porque no utilizaremos flags especiales
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,11,0));
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