/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Ejemplo de gestión de un led
  Fecha: 20180529
  Material: cualquier placa, PCA9685, led
  Descripción:
  Utilizamos la clase creada para la gestión de este módulo.
  
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

  Resultado:
  El led se enciende y apaga progresivamente
*/
 
#include <Arduino.h>
#include "RoJoPCA9685.h"

//Creamos objeto de gestión
RoJoPCA9685 pwm;
//Declaración de variables para nivel y dirección
int delta=1;
int level=0;

void setup()
{
  //Inicializamos el módulo con el id 0x40 (por defecto)
  pwm.begin();
}

void loop()
{
  //Calculamos el valor del nuevo nivel
  level+=delta;
  //Si hemos llegado a un extremo...cambiamos la dirección
  if((level==0) || (level==4095)) delta=-delta;
  //Aplicamos el nivel al primer canal (canal 0)
  pwm.setPWM(0,level);
}

