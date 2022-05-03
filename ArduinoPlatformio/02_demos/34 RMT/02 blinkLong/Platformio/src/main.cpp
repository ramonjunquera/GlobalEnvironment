/*
  Tema: Librería RMT
  Objetivo: Demo de blink de periodo superior al máximo
  Fecha: 20220501
  Autor: Ramón Junquera

  Descripción:
    En este ejercicio queremos que el led cambie su estado cada segundo.
    Recordemos que el periodo máximo que se puede conseguir con RMT es de:
      cls_div=255->3.1875us & duration=32767 -> 3.1875*32767=104444.8125us=104.4448125ms=0.1044448125s~0.1s
    El periodo que queremos es casi 10 veces mayor que el que permite RMT.
    Lo que haremos será obtener los parámetros para que un periodo exácto de 0.1s (por debajo el máximo permitido).
    Y repetiremos el estado HIGH del pulso 10 veces. Después repetiremos otras 10 veces el estado LOW.
    Puesto que cada item tiene dos estados, para 10 estados HIGH sólo necesitaremos 5 items de pulso.
    Y otros 5 para LOW. En total 10 items.
    La duración de cada pulso para todos los items será la misma (correspondiente a 0.1s).
    Los levels de los 5 primeros items será 1 (HIGH) y los últimos 5 será 0 (LOW).

    La única diferencia con ejemplos anteriores es que ahora nuestro array de items de pulsos tendrá más de
    un elemento.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=10;
const float periodo=100000;

rmt_config_t configRMT;
rmt_item32_t pulseItems[10]; //10 items. 5 de HIGH y 5 de LOW

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

  //Definimos los items pulso
  for(byte i=0;i<10;i++) pulseItems[i].duration0=pulseItems[i].duration1=duration; //Todos los pulsos tienen la misma duración
  for(byte i=0;i<5;i++) {
    pulseItems[i].level0=pulseItems[i].level1=1; //Los 5 primeros items son HIGH
    pulseItems[i+5].level0=pulseItems[i+5].level1=0; //Los 5 siguientes son LOW
  }
}

void loop() {
  //Enviamos el array de items de pulsos
  //Por el canal definido en la configuración
  //Enviamos los 10 items
  //Esperaremos a que se complete el envío
  ESP_ERROR_CHECK(rmt_write_items(configRMT.channel,pulseItems,10,true));
}