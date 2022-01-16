/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Gestión de un servomotor
  Fecha: 20180531
  Material: cualquier placa, PCA9685, servomotor SG90 9g
  Descripción:
  Ejemplo simple de control de un servomotor.
  Fijamos los valores límite del servo y utilizamos el método
  setServoDegrees para fijar el ángulo.
  Las constantes de los límites pueden variar dependiendo del servomotor.
  Incluso servomotores del mismo modelo podrían tener parámetros
  distintos. Se debe ajustar para cada caso.
  
  Resultado:
  El servomotor gira distintos ángulos

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
  //Fijamos el valor mínimo (0 grados) y máximo (180 grados) del servo
  pwm.setServoLimits(0,170,610);
}

void loop()
{
    pwm.setServoDegrees(0,0);
    delay(1000);
    pwm.setServoDegrees(0,45);
    delay(500);
    pwm.setServoDegrees(0,90);
    delay(500);
    pwm.setServoDegrees(0,180);
    delay(1000);
}

