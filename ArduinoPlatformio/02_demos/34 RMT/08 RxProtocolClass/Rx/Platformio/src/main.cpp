/*
  Tema: Librería RoJoRMT
  Objetivo: Receptor de mensaje con protocolo con paridad
  Fecha: 20220509
  Autor: Ramón Junquera

  Descripción:
    Se recibe muestra la información recibida
  Notas:
  - La luz ambiental afecta al resultado. Se debe reducir en lo posible.
*/

#include <Arduino.h>
#include <RoJoRMT.h>

const byte pinLed=32;
const byte msgSize=3; //Tamaño del mensaje

RoJoRMT conn;

RingbufHandle_t rb=NULL; //Buffer circular de entrada

void setup() {
  Serial.begin(115200);

  byte errorCode=conn.begin(pinLed,false); //Inicializamos configuración como Rx
  Serial.printf("Inicializado con error %u\n",errorCode);
}

void loop() {
  byte msg[msgSize];
  uint16_t totalBytes;
  byte errorCode=conn.Rx(msg,&totalBytes);
  if(errorCode>0 || totalBytes>0) {
    Serial.printf("Recibidos %u bytes con error %u\n",totalBytes,errorCode);
    if(totalBytes>0) for(uint16_t i=0;i<totalBytes;i++) Serial.printf("  %u : %u\n",i,msg[i]);
  }
}