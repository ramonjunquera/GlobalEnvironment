/*
  Tema: PCA9685 gestor de PWM
  Fecha: 20220217
  Autor: Ramón Junquera
  Objetivo: Aprender a utilizar un servomotor
  
  Descripción:
  En este ejemplo utilzaremos el módulo PCA9685 para gestionar un servomotor conectado al primer canal.

  Recerdemos que un servomotor tiene 3 cables:
  - ROJO: VCC
  - NEGRO o MARRON: GND
  - BLANCO o NARANJA: señal

  El modelo que estamos utilizando ya viene preparado para ser conectado directamente al módulo.
  Puesto que los servomotores consumen mucho más que los leds, nos vemos obligados a alimentar la toma superior.
  Para este ejemplo sólo tenemos un servomotor. No es necesaria una fuente de alimentación externa, aunque sí aconsejable,
  puesto que el motor genera muchas distorsiones que desestabilizan la alimentación y podrían dañar al correcto
  funcionamiento de la placa.

  Habitualmente un servomotor tiene en cuenta una señal analógica para posicionar el eje.
  La señal la generamos nosotros con pulsos PWM.
  La frecuencia habitual de los pulsos es de 60Hz.

  Se introduce una la función SetDegrees que permite asignar un ángulo determinado al eje del servomotor en
  el recorrido de 0 a 180 grados.
  
  Resultado:
  El eje gira los ángulos programados
*/

#include <Arduino.h>
#include <RoJoPCA9685.h>

RoJoPCA9685 pwm; //Objeto de gestión de PCA9685

void setup() {
  pwm.begin();
  pwm.setFreq(60); //Los servomotores funcionan a una frecuencia de 60Hz.
  //Utilizaremos los límites por defecto
}

void loop() {
  //Fijamos servo a 0 grados y esperamos un segundo
  pwm.setServoDegrees(0,0);
  delay(1000);
  //Fijamos servo a 45 grados y esperamos medio segundo
  pwm.setServoDegrees(0,45);
  delay(500);
  //Fijamos servo a 90 grados y esperamos medio segundo
  pwm.setServoDegrees(0,90);
  delay(500);
  //Fijamos servo a 180 grados y esperamos un segundo
  pwm.setServoDegrees(0,180);
  delay(1000);
}
