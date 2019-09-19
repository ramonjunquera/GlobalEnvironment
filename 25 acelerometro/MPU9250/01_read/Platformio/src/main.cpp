/*
  Autor: Ramón Junquera
  Fecha: 20190916
  Tema: Acelerómetro MPU9250
  Objetivo: Lectura de los valores
  Material: M5Stack Watch
  Descripción:
    Inicializa los sensores y muestra los valores contínuamente por el puerto serie
*/

#include <Arduino.h>
#include <RoJoMPU9250.h>

RoJoMPU9250 GAM; //Giróscopo, Acelerómetro y Magnetómetro

void setup() {
   Serial.begin(115200);
   //Mientras se inicializa el puerto serie, hacemos parpadear el led integrado
   pinMode(19,OUTPUT);
   for(byte i=0;i<20;i++) {
      digitalWrite(19,!digitalRead(19));
      delay(200);
   }
   GAM.begin(); //Inicialización de sensores
}
 
void loop() {
   digitalWrite(19,HIGH); //Mientras tomamos y enviamos datos, encendemos el led integrado
   int16_t data[3]; //Array para guardar datos de ejes de un sensor
   GAM.readG(data); //Leemos datos de Giróscopo
   Serial.print("["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   GAM.readA(data); //Leemos datos de Acelerómetro
   Serial.print("\t["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   //Si conseguimos datos del Magtetómetro...los mostramos
   if(GAM.readM(data)) Serial.println("\t["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   else Serial.println();
   digitalWrite(19,LOW); //Apagamos el led integrado
   delay(300);
}