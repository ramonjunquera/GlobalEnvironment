/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Gestión de varios servomotores
  Fecha: 20180602
  Material: cualquier placa, 2x PCA9685, 2x servomotor SG90 9g
  Descripción:
  Ejemplo simple de control de dos servomotores con dos módulos.
  En ambos módulos estarán conectados al canal 0.
  Fijamos los valores de los límites. En este caso los servos no
  tienen los mismos límites.
  En el bucle principal hacemos girar a ambos en 4 pasos de 0 a 180
  grados y en el mismo sentido, pero los pasos para el servo del
  módulo 0x40: 0, 45, 90 y 180, y para el servo del módulo 0x41:
  0, 60,120 y 180.
  
  Resultado:
  Los servomotores giran independientemente.

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

//Declaración de objetos de gestión de módulos PCA9685
RoJoPCA9685 pwm40;
RoJoPCA9685 pwm41;

void setup()
{
  //Inicializamos los objetos
  pwm40.begin();
  pwm41.begin(0x41);
  //Los servomotores habitualmente trabajan a una frecuencia de 60Hz
  pwm40.setFreq(60);
  pwm41.setFreq(60);
  //Fijamos el valor mínimo (0 grados) y máximo (180 grados) de los servos
  pwm40.setServoLimits(0,170,610);
  pwm41.setServoLimits(0,145,600);
}

void loop()
{
    pwm40.setServoDegrees(0,0);
    pwm41.setServoDegrees(0,0);
    delay(1000);
    pwm40.setServoDegrees(0,45);
    pwm41.setServoDegrees(0,60);
    delay(500);
    pwm40.setServoDegrees(0,90);
    pwm41.setServoDegrees(0,120);
    delay(500);
    pwm40.setServoDegrees(0,180);
    pwm41.setServoDegrees(0,180);
    delay(1000);
}

