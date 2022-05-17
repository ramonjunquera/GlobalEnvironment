/*
  Tema: Librería RMT
  Objetivo: Emisor de mensaje con protocolo con paridad
  Fecha: 20220509
  Autor: Ramón Junquera

Definición del protocolo de comunicación:
- La información se agrupa en paquetes de bytes (8 bits).
- La transmisión de 1 byte incluye una verificación de paridad para descartar los bytes erroneos
- Un paquete de 1 byte + verificación de paridad consta de 5 items de pulso
- Los 4 primeros paquetes de pulso contienen la información del byte a transmitir
- Los bits se codifican por parejas. 2 por items de pulso, comenzando siempre por un pulso HIGH.
- El valor del bit se codifica en la duración del pulso (sea HIGH o LOW).
- Un bit 0 tendrá una duración de 1 tick
- Un bit 1 tendrá una duración de 1*pulseTicks
- El quinto item de pulso contiene la información de paridad
- La paridad será siempre par
- El pulso HIGH (el primero) codifica la paridad en la duración del pulso
- Un bit de paridad 0 se codifica con una duración de 2*pulseTicks
- Un bit de paridad 1 se codifica con una duración de 3*pulseTicks
- El pulso LOW (el segundo) siempre tiene una duración de 1 tick
- En la recepción, si tenemos un pulso LOW de un item de pulso mayor de 1 tick, lo consideraremos como fin de transmisión.
- En la recepción, un item de pulso será reconocido porque:
  - El pulso HIGH tiene una duración inferior a 2*pulseTicks
  - El pulse LOW no es timeout
- En la recepción, un item de pulso de paridad será reconicido porque:
  - El pulso HIGH tiene una duración superior a 1*pulseTicks
  - El pulso LOW tiene una duración de 1 tick o timeout

Descripción:
  Cada vez que se pulsa el botón se envía un mensaje, un array de 3 bytes.
  Se siguen las reglas del protocolo especificado.
  Se toman valores óptimos para clk_div & pulseTicks.
*/

#include <Arduino.h>
#include <driver/rmt.h>
#include <RoJoSwitch.h>

const byte pinLed=33;
//const byte clk_div=160;
const byte clk_div=255;
const uint16_t minPulseTicksTx0=2;
const uint16_t minPulseTicksTx1=2;

const byte msgSize=3; //Tamaño del mensaje
const byte msg[msgSize]={33,110,222}; //Mensaje a enviar 0b00100001,0b01101110,0b11011110
RoJoSwitch button(32);
rmt_config_t configRMT;
rmt_item32_t pulseItems[5*msgSize]; //5 items por byte de mensaje

//Codifica el byte value en items de pulso y los guarda en el array pulseItemsBase
void codePulseItem(byte value,rmt_item32_t *pulseItemsBase) {
  byte paridad=0;
  rmt_item32_t *currentItem;
  for(byte i=0;i<4;i++) { //Codificamos el bit
    currentItem=&pulseItemsBase[i]; //Tomamos el item actual
    //Primer bit
    currentItem->level0=1;
    //currentItem->duration0=minPulseTicks;
    currentItem->duration0=minPulseTicksTx0;
    if(value & 128) { //Si el bit es 1...
      currentItem->duration0++;
      paridad++;
    }
    value<<=1;
    //Segundo bit
    currentItem->level1=0;
    currentItem->duration1=minPulseTicksTx1;
    if(value & 128) { //Si el bit es 1...
      currentItem->duration1++;
      paridad++;
    }
    value<<=1;
  }
  currentItem=&pulseItemsBase[4]; //Tomamos el último item (paridad)
  currentItem->level0=1;
  //Si el contador de bits 1 es impar...el pulso HIGH será de pulseTicks*3
  currentItem->duration0=(paridad%2>0)?minPulseTicksTx0+3:minPulseTicksTx0+2;
  currentItem->level1=0;
  currentItem->duration1=minPulseTicksTx1;
}

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

  for(byte i=0;i<msgSize;i++) codePulseItem(msg[i],&pulseItems[i*5]); //Codificamos los distintos bytes

  for(byte i=0;i<5*msgSize;i++) Serial.printf("%u : %u - %u : %u - %u\n",i,pulseItems[i].duration0,pulseItems[i].duration1,pulseItems[i].duration0-minPulseTicksTx0,pulseItems[i].duration1-minPulseTicksTx1);


}

void loop() {
  //Si se pulsa el botón...enviamos los items de pulso y esperamos a que se complete
  if(button.pressed()) rmt_write_items(configRMT.channel,pulseItems,msgSize*5,true);
}