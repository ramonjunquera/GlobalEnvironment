/*
  Tema: Librería RoJoRMT
  Objetivo: Recibir un mensaje de texto
  Fecha: 20220509
  Autor: Ramón Junquera

  Descripción:
    Se muestra el mensaje de texto recibido
  Notas:
  - La luz ambiental afecta al resultado. Se debe reducir en lo posible.
*/

#include <Arduino.h>
#include <RoJoRMT.h>

const byte pinLed=32;

RoJoRMT conn;

RingbufHandle_t rb=NULL; //Buffer circular de entrada

void setup() {
  Serial.begin(115200);

  byte errorCode=conn.begin(pinLed,false); //Inicializamos configuración como Rx
  Serial.printf("Inicializado con error %u\n",errorCode);
}

void loop() {
  byte bufferRx[11];
  uint16_t totalBytes;
  byte errorCode=conn.Rx(bufferRx,&totalBytes);
  if(errorCode>0 || totalBytes>0) {
    Serial.printf("Recibidos %u bytes con error %u\n",totalBytes,errorCode);
    if(totalBytes>0) {
      String msg="";
      for(uint16_t i=0;i<totalBytes;i++) msg+=(char)bufferRx[i];
      Serial.println(msg);
    }
  }
}