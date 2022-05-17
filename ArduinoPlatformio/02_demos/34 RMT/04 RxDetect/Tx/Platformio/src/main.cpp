/*
  Tema: Librería RMT
  Objetivo: Emisor de pulso simple
  Fecha: 20220504
  Autor: Ramón Junquera

  Descripción:
    Al pulsar el botón se emite un pulso de 0.1s
*/

#include <Arduino.h>
#include <driver/rmt.h>
#include <RoJoSwitch.h>

const byte pinLed=33;
const float periodo=100000; //100ms=0.1s

RoJoSwitch button(32);

rmt_config_t configRMT;
rmt_item32_t pulseItems[1]; //Sólo un item

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
  configRMT.rmt_mode=RMT_MODE_TX; //Transmitiremos
  configRMT.channel=RMT_CHANNEL_0; //Seleccionamos el canal
  configRMT.gpio_num=(gpio_num_t)pinLed;
  configRMT.clk_div=clk_div; //Valor calculado
  configRMT.mem_block_num=1; //Sólo un item
  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,0,0)); //Activamos canal

  //Definimos el item de pulso
  pulseItems[0].duration0=duration; //Valor calculado
  pulseItems[0].level0=1; //Primer pulso = HIGH
  pulseItems[0].duration1=duration; //Valor calculado
  pulseItems[0].level1=0; //Segundo pulso = LOW
}

void loop() {
  //Si se pulsa el botón...generamos un pulso y esperamos a que se complete
  if(button.pressed()) rmt_write_items(configRMT.channel, pulseItems, 1, true);
}