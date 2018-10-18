/*
  Autor: Ramón Junquera
  Fecha: 20181018
  Tema: Motores paso a paso
  Objetivo: Demo de librería RoJoStepper
  Material adicional: placa Arduino, cables, motor paso a paso (28BYJ-48)
  Descripción:
    Demostración de uso de más de un motor.
  Nota:
    Una placa Arduino no tiene suficiente potencia como para alimentar dos motores.
    Es necesaria una fuente de alimentación externa.
  Resultado:
    Giran medias vueltas simultáneamente y en sentidos contrarios.
*/

#include <Arduino.h>
#include "RoJoStepper.h" //Gestión de motores paso a paso

//No necesitamos crear un objeto de gestión de RoJoStepper porque es una clase estática

void setup()
{
  Serial.begin(115200);
  
  //Utilizaremos la interrupción 1 para el timer que refresca los motores
  RoJoStepper::startTimer(1);
  //Añadimos el motor 0:
  //  motorId=0
  //  pin1=9
  //  pin2=10
  //  pìn3=11
  //  pin4=12
  //  sequenceType=2
  RoJoStepper::addMotor(0,9,10,11,12,2);
  //Añadimos el motor 1:
  //  motorId=1
  //  pin1=2
  //  pin2=3
  //  pìn3=4
  //  pin4=5
  //  sequenceType=2
  RoJoStepper::addMotor(1,2,3,4,5,2);
  //Activamos los motores
  RoJoStepper::enableMotor(0,true);
  RoJoStepper::enableMotor(1,true);
}

void loop()
{
  //Giramos los dos motores media vuelta en sentidos contrarios
  RoJoStepper::go(0,1024);
  RoJoStepper::go(1,-1024);
  //Esperamos a que ambos motores hayan completado el giro
  while(RoJoStepper::busy(0)||RoJoStepper::busy(1)) {}
  //Recuperamos la posición inicial
  RoJoStepper::go(0,0);
  RoJoStepper::go(1,0);
  //Esperamos a que ambos motores hayan completado el giro
  while(RoJoStepper::busy(0)||RoJoStepper::busy(1)) {}
  //Esperamos un momento
  delay(500);
}

