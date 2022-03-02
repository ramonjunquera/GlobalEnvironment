/*
  Tema: PCA9685 gestor de PWM
  Fecha: 20220217
  Autor: Ramón Junquera
  Objetivo: Gestionar varios servomotores
  
  Descripción:
  En este ejemplo utilzaremos el módulo PCA9685 para gestionar dos servomotores conectados a los dos primeros canales.

  Recordemos que un servomotor tiene 3 cables:
  - ROJO: VCC
  - NEGRO o MARRON: GND
  - BLANCO o NARANJA: señal

  Puesto que cadaservomotor puede tener sus propios valores de máximo y mínimo, se crea un array de constantes
  La función setDegrees calcula los grados en función de los valores límite de cada servomotor
  
  Resultado:
  Gestionamos dos servomotores de manera independiente
*/

#include <Arduino.h>
#include <RoJoPCA9685.h>

RoJoPCA9685 pwm; //Gestión de PCA9685

void setup() {
  pwm.begin();
  pwm.setFreq(60); //Los servomotores funcionan a una frecuencia de 60Hz.
  //Fijamos los límites de cada servo
  //No es obligatorio porque ya existen unos valores por defecto, pero si es necesario
  //cada servo puede tener su propia configuración
  pwm.setServoLimits(0,170,600);
  pwm.setServoLimits(0,130,590);
}

void loop() {
  pwm.setServoDegrees(0,0);
  pwm.setServoDegrees(1,0);
  delay(1000);
  pwm.setServoDegrees(0,45);
  pwm.setServoDegrees(1,60);
  delay(500);
  pwm.setServoDegrees(0,90);
  pwm.setServoDegrees(1,120);
  delay(500);
  pwm.setServoDegrees(0,180);
  pwm.setServoDegrees(1,180);
  delay(1000);
}
