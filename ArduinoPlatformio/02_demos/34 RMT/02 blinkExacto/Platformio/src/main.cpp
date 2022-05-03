/*
  Tema: Librería RMT
  Objetivo: Demo de blink de periodo exacto
  Fecha: 20220428
  Autor: Ramón Junquera

  Descripción:
    En el ejercicio anterior hemos desarrollado un ejemplo de blink con el máximo periodo posible.
    Aquí aprenderemos a calcular los valores de los parámetros para que un periodo determinado.

    La fórmula es la siguiente:
      periodo = duración total del pulso
      clk_div = redondeo a entero superior de periodo*80/32767
      duration0 = parte entera de periodo/clk_div*80
    El resultado son los parámetro óptimos que producen el mínimo error posible.

    En este caso queremos que el periodo del pulso sea exáctamente 0.1s = 100ms = 100000us
      periodo = 100000us
      clk_div = periodo*80/32767 = 100000*80/32767 = 244.148 ~ 245
      duration0 = periodo/clk_div*80 = 100000/245*80 = 32653.06 ~32653
    
    El periodo resultante de estos parámetros es 99999.8125us.
    Que comete un error de 0.1875us.
    
    Si tomásemmos clk_div=244, duration0 saldría 32786, que es un valor superior al permitido.
    Con clk_div=246, duration0 sería 32520, con un periodo de 99999us y un error de 1us.
    Con clk_div=247, duration0 sería 32388, con un periodo de 99997.95 y un error de 2.05us.

    Conclusión. Los parámetros calculados son los más ajustados.

    Para facilitar su uso, se crea un función para que los calcule automáticamente.
*/

#include <Arduino.h>
#include <driver/rmt.h>

const byte pinLed=10;
const float periodo=100000;

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
  //Enviamos el array de items de pulsos
  //Por el canal definido en la configuración
  //Sólo tenemos un item
  //Esperaremos a que se complete el envío
  ESP_ERROR_CHECK(rmt_write_items(configRMT.channel,pulseItems,1,true));
}