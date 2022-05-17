/*
  Tema: Librería RMT
  Objetivo: Emisor de mensaje con protocolo
  Fecha: 20220505
  Autor: Ramón Junquera

Definición del protocolo de comunicación:
- Cada bit se enviará como un pulso HIGH
- Un bit 0 durará 1*pulseTicks ticks
- Un bit 1 durará 2*pulseTicks ticks
- Entre los pulsos de información (HIGH) se insertan pulsos de separación LOW
- Un pulso de separación de bits durará 1*pulseTicks ticks LOW
- Un pulso de separación de bytes durará 2*pulseTicks ticks LOW
- Un pulso de fin de envío durará al menos 3*pulseTicks ticks LOW

Descripción:
  Cada vez que se pulsa el botón se envía un mensaje, un array de 3 bytes.
  Se siguen las reglas del protocolo especificado.
  Se toman valores óptimos para clk_div & pulseTicks.
*/

#include <Arduino.h>
#include <driver/rmt.h>
#include <RoJoSwitch.h>

const byte pinLed=33;
const byte clk_div=160;
const uint16_t pulseTicks=3;
const byte msgSize=3; //Tamaño del mensaje
const byte msg[msgSize]={33,111,222}; //Mensaje a enviar
RoJoSwitch button(32);
rmt_config_t configRMT;
rmt_item32_t pulseItems[8*msgSize]; //8 items/bits por byte de mensaje

void setup() {
  Serial.begin(115200);

  //Definición de la configuración de RMT
  configRMT.rmt_mode=RMT_MODE_TX; //Transmitiremos
  configRMT.channel=RMT_CHANNEL_0; //Seleccionamos el canal
  configRMT.gpio_num=(gpio_num_t)pinLed;
  configRMT.clk_div=clk_div; //Valor calculado
  configRMT.mem_block_num=1; //Sólo un item
  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,0,0)); //Activamos canal

  //Definimos los items de pulso
  for(byte i=0;i<msgSize;i++) { //Recorremos todas las posiciones del mensaje
    byte currentByte=msg[i]; //Obtenemos el byte actual
    Serial.printf("index %u : value %u\n",i,currentByte);
    for(byte b=0;b<8;b++) { //Recorremos todos los bits de la posición
      byte currentBit=currentByte & 128; //Obtenemos el bit actual
      pulseItems[i*8+b].duration0=currentBit>0?2*pulseTicks:1*pulseTicks; //Duración de pulso HIGH depende del bit
      pulseItems[i*8+b].level0=1; //Nivel de pulso HIGH
      pulseItems[i*8+b].duration1=1*pulseTicks; //Duración de pulso LOW siempre igual
      pulseItems[i*8+b].level1=0; //Nivel de pulso LOW
      Serial.printf("  bit %u : duration0 %u\n",b,pulseItems[i*8+b].duration0);
      currentByte<<=1; //Siguiente bit
    }
    pulseItems[i*8+7].duration1=2*pulseTicks; //Duración de pulso LOW de fin de byte
  }
  pulseItems[msgSize*8-1].duration1=3*pulseTicks; //Duración de pulso LOW de fin de mensaje
}

void loop() {
  //Si se pulsa el botón...enviamos los items de pulso y esperamos a que se complete
  if(button.pressed()) rmt_write_items(configRMT.channel,pulseItems,msgSize*8,true);
}