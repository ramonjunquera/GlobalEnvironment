/*
  Tema: Librería RMT
  Objetivo: Demo de blink de periodo máximo
  Fecha: 20220428
  Autor: Ramón Junquera

  Descripción:
    Utilizaremos RMT para hacer el ejemplo de blink más lento posible.
    Utilizaremos el valor máximo para clk_div = 255 = 3.1875us
    y la máxima duración para el pulso = 32767.
    Duración de un pulso=3.1875*32767=104444.8125us~104ms~0.1s

    Para este blink, el led cambiará su estado cada décima de segundo.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=10;
rmt_config_t configRMT;
rmt_item32_t pulseItems[1]; //Sólo un item

void setup() {
  //Definición de la configuración de RMT
  configRMT.rmt_mode=RMT_MODE_TX; //Transmitiremos
  configRMT.channel=RMT_CHANNEL_0; //Seleccionamos el canal
  configRMT.gpio_num=(gpio_num_t)pinLed;
  configRMT.clk_div=255; //Máximo divisor posible
  configRMT.mem_block_num=1; //Sólo un item
  ESP_ERROR_CHECK(rmt_config(&configRMT)); //Aplicamos configuración
  ESP_ERROR_CHECK(rmt_driver_install(configRMT.channel,0,0)); //Activamos canal

  //Definimos el item de pulso
  pulseItems[0].duration0=32767; //Duración máxima
  pulseItems[0].level0=1; //Primer pulso = HIGH
  pulseItems[0].duration1=32767; //Duración máxima
  pulseItems[0].level1=0; //Segundo pulso = LOW
}

void loop() {
  //Enviamos el array de items de pulsos
  //Por el canal definido en la configuración
  //Sólo tenemos un item
  //Esperaremos a que se complete el envío
  ESP_ERROR_CHECK(rmt_write_items(configRMT.channel,pulseItems,1,true));
}