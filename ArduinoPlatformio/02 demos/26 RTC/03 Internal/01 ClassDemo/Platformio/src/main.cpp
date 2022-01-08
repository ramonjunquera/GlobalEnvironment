/*
  Autor: Ramón Junquera
  Fecha: 20191128
  Tema: Librería de gestión de RTC interno
  Objetivo: Demo de librería
  
  Descripción:
    Se fija una hora y se envía por el puerto serie la hora leida cada segundo
*/

#include <Arduino.h>
#include <RoJoRTCinternal.h>

RoJoRTCinternal myClock; //Objeto de gestión del RTC interno

void setup() {
  Serial.begin(115200);
  RoJoDateTime r;
  //Damos valores de ejemplo: 29/02/2020 23:59:50
  r.day=29;
  r.month=2;
  r.year=2020;
  r.hour=23;
  r.minute=59;
  r.second=50;
  myClock.set(&r);
  Serial.println("Reloj inicializado correctamente");
}

void loop() {
  RoJoDateTime r;
  myClock.get(&r);
  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));
  delay(1000);
}