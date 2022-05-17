/*
  Tema: Librería RMT
  Objetivo: Receptor de mensaje con protocolo con paridad
  Fecha: 20220509
  Autor: Ramón Junquera

  Descripción:
    Se recibe un mensaje codificado con un protocolo.
    El mensaje consiste en un array de 3 bytes.
    Se toman los mismos valores de clk_div & pulseTicks que en programa del emisor.
  Resultado:
    Se muestra el resultado recibido si tiene un formato correcto.
  Notas:
  - La luz ambiental afecta al resultado. Se debe reducir en lo posible.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=32;
//const byte clk_div=160;
const byte clk_div=255;
const uint16_t minPulseTicksRx0=3;
const uint16_t minPulseTicksRx1=1;
const byte msgSize=3; //Tamaño del mensaje

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
  //El pulso más largo sería el de paridad =3*pulseTicks=3*3=9;
  //Necesitamos esperar al menos ese tiempo para que se reciba el pulso completo
  //Lo sobredimensionamos a 11
  configRMT.rx_config.idle_threshold=30; 

  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  //Activamos el canal
  //Definimos el tamaño del buffer de recepción
  //Un item de pulso ocupa 11 bytes
  //Un byte necesita 5 items de pulso
  //Pretendemos recibir 3 bytes
  //Necesitaremos un total de 5*3=15 items
  //Necesitaremos un buffer de al menos 15*11=165 bytes
  //Lo sobredimensionaremos a 200
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,200,0));
  rmt_get_ringbuf_handle(configRMT.channel,&rb); //Obtenemos el puntero del buffer circular de entrada
  ESP_ERROR_CHECK(rmt_rx_start(configRMT.channel,1)); //Activamos la recepción reseteando al canal
  Serial.println("config end");
}

//Decodifica y normaliza los valores de un item de pulso según protocolo
void decodeValuesFromItems(rmt_item32_t *pulseItem,byte *value0,byte *value1) {
  *value0=pulseItem->duration0-minPulseTicksRx0;
  *value1=pulseItem->duration1;
  if(*value1) *value1-=minPulseTicksRx1;
  Serial.printf("* %u - %u : %u - %u\n",pulseItem->duration0,pulseItem->duration1,*value0,*value1);
}

//Decodifica los items de pulso
//Parámetros:
//- res: puntero a array de bytes donde se almacenará el resultado decodificado
//- totalBytes: númeo de bytes decodificados
//- pulseItemsBase: puntero a array de items de pulso
//- totalItems: número de items de pulso
//Devuelve true si se han detectado errores
bool decodeItems(byte *res,byte *totalBytes,rmt_item32_t *pulseItemsBase,size_t totalItems) {
  bool errors=false;
  byte currentByte=0;
  size_t indexItem=0;
  byte indexItemInByte=0;
  byte paridad=0;
  byte v0,v1; //Valores decodificados de un item de pulso
  *totalBytes=0; //Inicialmente no se ha decidificado ningún byte
  while(indexItem<totalItems) {
    decodeValuesFromItems(&pulseItemsBase[indexItem],&v0,&v1);
    if(v0>=2 && v0<=3 && v1==0 && indexItemInByte==4 && (paridad+v0-2)%2==0) { //Si es un item de paridad correcto...
      res[(*totalBytes)++]=currentByte; //Guardamos el byte recibido
      currentByte=0;
      indexItemInByte=0;
      paridad=0;
    } else if(v0<2 && v1<2 && indexItemInByte<4){ //Si es un item de información correcto...
      currentByte<<=1;
      if(v0) { //Si es 1...
        currentByte++;
        paridad++;
      }
      currentByte<<=1;
      if(v1) { //Si es 1...
        currentByte++;
        paridad++;
      }
      indexItemInByte++;
    } else { //Si es un item corrupto...
      currentByte=0;
      indexItemInByte=0;
      paridad=0;
      errors=true;
    }
    indexItem++;
  }
  return errors;
}

void loop() {
  byte msg[msgSize];
  byte totalBytes;
  size_t rx_size=0; //Número de items recibidos
  //Leemos del buffer circular de entrada los items que contiene ahora mismo. No esperamos.
  rmt_item32_t* items=(rmt_item32_t*)xRingbufferReceive(rb,&rx_size,0);
  if(items) { //Si hemos podido leer algún item...
    rx_size/=4; //Número de items de pulso recibidos
    Serial.printf("%u Recibidos %u items\n",millis(),rx_size);
    if(rx_size>2 && rx_size<=5*msgSize) {
      bool error=decodeItems(msg,&totalBytes,items,rx_size);
      Serial.printf("%u decodificados",totalBytes);
      if(error) Serial.println(" con error");
      else Serial.println(" sin error");
      for(byte i=0;i<totalBytes;i++) Serial.printf("  %u : %u\n",i,msg[i]);
    }
    vRingbufferReturnItem(rb,(void*)items); //Liberamos los items leidos del buffer circular
  }
}