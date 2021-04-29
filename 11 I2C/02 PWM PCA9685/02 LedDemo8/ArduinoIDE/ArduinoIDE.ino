/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Ejemplo de gestión de varios leds
  Fecha: 20180530
  Material: cualquier placa, PCA9685, 8 leds
  Descripción:
  Utilizamos la clase creada para la gestión de este módulo.
  
  Efecto Kitt con 8 leds

  En los ejemplos con leds tomaremos utilizaremos la misma señal para
  alimentar al led. Por eso no usamos la alimentación del circuito
  de trabajo en el módulo.

  I2C pinout:

  device  SCL SDA
  ------- --- ---
  Mega    21  20
  Nano    A5  A4
  UNO     A5  A4
  ESP8266 D1  D2
  ESP32   22  21
  RPi 0   28  27 -> ID_SC & ID_SD    
  RPi 1    5   3 -> GPIO3 & GPIO2
*/
 
#include <Arduino.h>
#include "RoJoPCA9685.h"

//Creamos objeto de gestión
RoJoPCA9685 pwm;
//Array con los niveles de cada canal
int levels[8];
//Declaración de variables para la dirección y el canal
int delta=1;
int channel=0;

void applyAndFade()
{
  //Aplica los niveles de todos los canales en el módulo
  
  //Recorremos los 8 canales utilizados y enviamos sus niveles al módulo
  for(byte c=0;c<8;c++) pwm.setPWM(c,levels[c]);
  //Recorremos los 8 canales utilizados...
  for(byte c=0;c<8;c++)
  {
    //Reducimos el nivel del canal en un valor fijo
    levels[c]-=50;
    //Si hemos reducido demasiado, como mínimo lo dejaremos en 0
    if(levels[c]<0) levels[c]=0;
  }
}

void setup()
{
  //Inicializamos el módulo con el id 0x40 (por defecto)
  pwm.begin();
}

void loop()
{
  //Calculamos el valor del nuevo canal
  channel+=delta;
  //Si hemos llegado a un extremo...cambiamos la dirección
  if((channel==0) || (channel==7)) delta=-delta;
  //Aplicamos el nivel máximo al canal afectado
  levels[channel]=4095;
  //Repetimos un bucle que sólo baja la intensidad de todos los
  //leds en cada ciclo
  for(byte z=0;z<14;z++)
  {
    //Aplicamos configuración actual
    applyAndFade();
    //Esperamos un momento
    delay(10);
  }
}

