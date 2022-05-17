/*
  Tema: Librería RoJoRMT
  Objetivo: Enviar un mensaje de texto
  Fecha: 20220509
  Autor: Ramón Junquera

Descripción:
  Cada vez que se pulsa el botón se envía un mensaje de texto
*/

#include <Arduino.h>
#include <RoJoSwitch.h>
#include <RoJoRMT.h>

const byte pinLed=33;
const String msg="Hola mundo!";

RoJoSwitch button(32);
RoJoRMT conn;

void setup() {
  Serial.begin(115200);
  byte errorCode=conn.begin(pinLed,true,160,msg.length()); //Inicializamos configuración como Tx
  //byte errorCode=conn.begin(pinLed,true,255); //Inicializamos configuración como Tx
  Serial.printf("Inicializado con error %u\n",errorCode);
}

void loop() {
  if(button.pressed()) { //Si se pulsa el botón...
    byte errorCode=conn.Tx((byte*)&msg,msg.length()); //Enviamos el mensaje
    Serial.printf("%u Enviado con error %u\n",millis(),errorCode);
  } 
}