/*
  Autor: Ramón Junquera
  Fecha: 20200910
  Tema: Multitasking / FreeRTOS
  Objetivo: Eficiencia de librería RoJoFloatMatrix
  Material adicional: placa ESP32
  Descripción:
    Crearemos un par de matrices y repetiremos un número determinado de veces
    su producto matricial, calculando el tiempo que tarda.
*/

#include <Arduino.h>
#include <RoJoFloatMatrix.h> //Gestión de matrices

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  uint16_t size=50;
  RoJoFloatMatrix A(size,size),B(size,size),C; //Creamos matrices
  A.fillRand();
  B.fillRand();
  Serial.println("start");
  uint32_t startT=micros();
  for(uint32_t epoch=0;epoch<1000;epoch++) {
    //Serial.printf("epoch=%i\n",epoch);
    C.mProd(&A,&B);
  }
  Serial.printf("duration=%li\n",micros()-startT);
}

void loop() {
  //Nada especial que hacer aquí
}