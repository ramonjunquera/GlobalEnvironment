/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Gestión de varios servomotores
  Fecha: 20180602
  Material: cualquier placa, PCA9685, 2x servomotor SG90 9g
  Descripción:
  Ejemplo simple de control de dos servomotores.
  Estarán conectados a los canales 0 y 3.
  Fijamos los valores de los límites. En este caso los servos no
  tienen los mismos límites.
  En el bucle principal hacemos girar a ambos en 4 pasos de 0 a 180
  grados y en el mismo sentido, pero los pasos para el servo 0 son
  0, 45, 90 y 180, y para el servo 3, 0, 60,120 y 180.
  
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

//Declaración de objeto de gestión de módulo PCA9685
RoJoPCA9685 pwm;

void setup()
{
  //Inicializamos el objeto. Suponemos que el identificador es el 0x40
  pwm.begin();
  //Los servomotores habitualmente trabajan a una frecuencia de 60Hz
  pwm.setFreq(60);
  //Fijamos el valor mínimo (0 grados) y máximo (180 grados) de los servos
  pwm.setServoLimits(0,170,610);
  pwm.setServoLimits(3,145,600);
}

void loop()
{
    pwm.setServoDegrees(0,0);
    pwm.setServoDegrees(3,0);
    delay(1000);
    pwm.setServoDegrees(0,45);
    pwm.setServoDegrees(3,60);
    delay(500);
    pwm.setServoDegrees(0,90);
    pwm.setServoDegrees(3,120);
    delay(500);
    pwm.setServoDegrees(0,180);
    pwm.setServoDegrees(3,180);
    delay(1000);
}

