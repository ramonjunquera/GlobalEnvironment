/*
  Tema: Librería RoJoRMT
  Objetivo: Emisor de mensaje con protocolo con paridad
  Fecha: 20220509
  Autor: Ramón Junquera

Descripción:
  Cada vez que se pulsa el botón se envía un mensaje, un array de 3 bytes.
*/

#include <Arduino.h>
#include <RoJoSwitch.h>
#include <RoJoRMT.h>

const byte pinLed=33;
const byte msgSize=3; //Tamaño del mensaje
byte msg[msgSize]={33,110,222}; //Mensaje a enviar 0b00100001,0b01101110,0b11011110

RoJoSwitch button(32);
RoJoRMT conn;

void setup() {
  Serial.begin(115200);
  byte errorCode=conn.begin(pinLed); //Inicializamos configuración como Tx
  //byte errorCode=conn.begin(pinLed,true,255); //Inicializamos configuración como Tx
  Serial.printf("Inicializado con error %u\n",errorCode);
}

void loop() {
  if(button.pressed()) { //Si se pulsa el botón...
    byte errorCode=conn.Tx(msg,msgSize); //Enviamos el mensaje
    Serial.printf("%u Enviado con error %u\n",millis(),errorCode);
  } 
}