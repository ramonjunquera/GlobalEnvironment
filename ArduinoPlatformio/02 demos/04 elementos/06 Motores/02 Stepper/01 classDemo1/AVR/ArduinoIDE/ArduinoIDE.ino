/*
  Autor: Ramón Junquera
  Fecha: 20181018
  Tema: Motores paso a paso
  Objetivo: Demo de librería RoJoStepper
  Material adicional: placa Arduino, cables, motor paso a paso (28BYJ-48)
  Descripción:
    Se aplicarán los 3 tipos de secuencias disponibles a un motor para comprobar
    sus diferencias en cuanto a velocidad y torque.
    Siempre se pide recorrer 2048 pasos.
    En la primera secuencia corresponde a media vuelta. En las restantes a una.
  Resultado:
    Se hace girar el eje en ambos sentidos para cada uno de los tipos de secuencia.
*/

#include <Arduino.h>
#include "RoJoStepper.h" //Gestión de motores paso a paso

//No necesitamos crear un objeto de gestión de RoJoStepper porque es una clase estática

void setup()
{
  Serial.begin(115200);
  
  //Utilizaremos la interrupción 1 para el timer que refresca los motores
  RoJoStepper::startTimer(1);
}

void loop()
{
  //Recorremos todos los tipos de secuencias
  //  0 - Máxima precisión (4096 pasos/vuelta)
  //  1 - Máximo torque (2048 pasos/vuelta)
  //  2 - Mínimo consumo (2048 pasos/vuelta)
  for(byte seqType=1;seqType<3;seqType++)
  {
    //Informamos del tipo de secuencia
    Serial.println("seqType="+String(seqType));
    //Añadimos el motor 0:
    //  motorId=0
    //  pin1=9
    //  pin2=10
    //  pìn3=11
    //  pin4=12
    //  sequenceType=seqType
    //Cada vez que se añade un motor con un id existente, se sobre escribe la configuración
    RoJoStepper::addMotor(0,9,10,11,12,seqType);
    //Activamos el motor 0
    RoJoStepper::enableMotor(0,true);
    //Pedimos que el motor 0 avance hasta una posición
    RoJoStepper::go(0,2048);
    //Esperamos a que el motor 0 se pare
    while(RoJoStepper::busy(0)) {}
    //Pedimos que el motor 0 vuelva a la posición original
    RoJoStepper::go(0,0);
    //Esperamos a que el motor 0 se pare
    while(RoJoStepper::busy(0)) {}
    //Esperamos un momento
    delay(500);
  }
}

