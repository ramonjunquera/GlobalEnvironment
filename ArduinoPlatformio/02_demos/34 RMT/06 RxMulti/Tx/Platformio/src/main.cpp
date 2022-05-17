/*
  Tema: Librería RMT
  Objetivo: Emisor de varios pulso iguales
  Fecha: 20220505
  Autor: Ramón Junquera

  Descripción:
    Al pulsar el botón se emiten 3 pulsos de 30ms espaciados por 20ms
*/

#include <Arduino.h>
#include <driver/rmt.h>
#include <RoJoSwitch.h>

const byte pinLed=33;

RoJoSwitch button(32);

rmt_config_t configRMT;
rmt_item32_t pulseItems[3]; //3 items
const byte clk_div=80; //1 tick = 1us

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
  for(byte i=0;i<3;i++) {
    pulseItems[i].duration0=30000; //Pulso alto = 30000us = 30ms
    pulseItems[i].level0=1; //Primer pulso = HIGH
    pulseItems[i].duration1=20000; //pulso bajo = 20000us = 20ms
    pulseItems[i].level1=0; //Segundo pulso = LOW
  }
}

void loop() {
  //Si se pulsa el botón...enviamos los 3 items de pulso y esperamos a que se complete
  if(button.pressed()) rmt_write_items(configRMT.channel,pulseItems,3,true);
}