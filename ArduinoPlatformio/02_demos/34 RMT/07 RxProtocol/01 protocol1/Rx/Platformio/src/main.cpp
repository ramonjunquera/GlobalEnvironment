/*
  Tema: Librería RMT
  Objetivo: Receptor de mensaje con protocolo
  Fecha: 20220505
  Autor: Ramón Junquera

  Descripción:
    Se recibe un mensaje codificado con un protocolo.
    El mensaje consiste en un array de 3 bytes.
    Se toman los mismos valores de clk_dic & pulseTicks que en programa del emisor.
  Resultado:
    Se muestra el resultado recibido si tiene un formato correcto.
  Notas:
  - La luz ambiental afecta al resultado. Se debe reducir en lo posible.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=32;
const byte clk_div=160;
const uint16_t pulseTicks=3;
const uint32_t normTicks=1; //Corrección de error de ticks para normalización
const byte msgSize=3; //Tamaño del mensaje
byte msg[msgSize]; //Mensaje a recibir

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
  //La máxima duración de un pulso es de 3 ticks
  //Necesitamos esperar al menos ese tiempo para que se reciba el pulso completo
  configRMT.rx_config.idle_threshold=8; 

  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  //Activamos el canal
  //Definimos el tamaño del buffer de recepción
  //Un item de pulso ocupa 11 bytes
  //Pretendemos recibir 3 bytes de 8 bits cada uno. Total 3*8=24 items.
  //Necesitaremos un buffer de al menos 24*11=264 bytes. Lo sobredimensionaremos a 300
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,300,0));
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
    if(rx_size/4==8*msgSize) { //Si hemos recibido el mensaje completo...
      byte i=0; //Index
      byte currentByte=0;
      while(i<msgSize*8) {
        if(items[i].level0==0 && items[i].level1==1) { //Si los niveles de pulso son correctos...
          uint32_t ticks0=(items[i].duration0+normTicks)/pulseTicks; //Ticks normalizados HIGH
          uint32_t ticks1=(items[i].duration1+normTicks)/pulseTicks; //Ticks normalizados LOW
          currentByte=(currentByte<<1)+ticks0-1; //Desplazamos y añadimos el bit
          if(ticks1!=1) { //Si no es un separador de bits (separador de bytes o fin de mensaje)...
            msg[i/8]=currentByte; //Añadimos el byte al mensaje
            if(ticks1==2) currentByte=0; //Si es un separador de bytes...lo reseteamos
          }
        }
        i++;
      }
      for(byte i=0;i<msgSize;i++) {
        Serial.printf("  byte=%u : value=%u\n",i,msg[i]);
      }
    }
    vRingbufferReturnItem(rb,(void*)items); //Liberamos los items leidos del buffer circular
  }
}