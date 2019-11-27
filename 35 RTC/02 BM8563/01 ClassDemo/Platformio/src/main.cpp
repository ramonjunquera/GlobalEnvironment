/*
  Autor: Ramón Junquera
  Fecha: 20191120
  Tema: Librería de gestión de RTC BM8563
  Objetivo: Demo de librería
  Material: M5Stick-C

  Descripción:
    Se fija una hora y se envía por el puerto serie la hora leida cada segundo
*/

#include <Arduino.h>
#include <RoJoRTCBM8563.h>

RoJoRTCBM8563 myClock; //Objeto de gestión del RTC BM8563 del M5Stick-C

void setup() {
  Serial.begin(115200);
  //Encendemos el led para saber que está funcionando
  pinMode(10,OUTPUT); digitalWrite(10,LOW); //DEBUG
  //Nota. M5Stick-C tiene un RTC BM8563 con identificador I2C: 0x51
  //Conectado a los pines SDA=21 y SCL=22
  if(myClock.begin(21,22)) {
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
  else Serial.println("Error al inicializar el reloj");
}

void loop() {
  RoJoDateTime r;
  myClock.get(&r);
  Serial.println(String(r.day)+"/"+String(r.month)+"/"+String(r.year)+" ("+myClock.dayName[r.weekDay]+") "+String(r.hour)+":"+String(r.minute)+":"+String(r.second));
  delay(1000);
}