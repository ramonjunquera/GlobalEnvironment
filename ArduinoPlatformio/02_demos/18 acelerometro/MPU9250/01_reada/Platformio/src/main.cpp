/*
  Autor: Ramón Junquera
  Fecha: 20201213
  Tema: Acelerómetro MPU9250
  Objetivo: Lectura de los valores
  Material: M5Stack Watch, M5Stick-C
  Descripción:
    Inicializa los sensores y muestra los valores contínuamente por el puerto serie
  Nota:
    M5Stick-C no tiene magnetómetro. Sus lecturas no se mostrarán.
*/

#include <Arduino.h>
#include <RoJoMPU9250.h>

//Pinout
#ifdef ARDUINO_M5Stick_C //Para M5Stick-C...
  const int pinSDA=21;
  const int pinSCL=22;
  const byte pinLED=10;
#elif defined(ARDUINO_M5Stick_Watch) //Para M5Stack Watch...
  const int pinSDA=-1;
  const int pinSCL=-1;
  const byte pinLED=19;
#elif defined(ARDUINO_M5Stick_Atom_Matrix) //Para M5Stack Atom Matrix...
  const int pinSDA=25;
  const int pinSCL=21;
  const byte pinLED=255; //No tiene led integrado
#elif defined(ARDUINO_M5STACK_FIRE) //Para M5Stack Fire...
  const int pinSDA=-1;
  const int pinSCL=-1;
  const byte pinLED=255; //No tiene led integrado
#else //Para cualquier otra placa...
  const int pinSDA=-1;
  const int pinSCL=-1;
  const byte pinLED=LED_BUILTIN;
#endif

RoJoMPU9250 gam; //Giróscopo, Acelerómetro y Magnetómetro
bool hasM; //Tiene magnetómetro?

void setup() {
  Serial.begin(115200);
  //Mientras se inicializa el puerto serie, hacemos parpadear el led integrado
  pinMode(pinLED,OUTPUT);
  for(byte i=0;i<20;i++) {
    digitalWrite(pinLED,!digitalRead(pinLED));
    delay(200);
  }
  gam.begin(pinSDA,pinSCL); //Inicialización de sensores
  hasM=gam.hasM();
}
 
void loop() {
  digitalWrite(pinLED,HIGH); //Mientras tomamos y enviamos datos, encendemos el led integrado
  int16_t dataI[3]; //Array para guardar datos enteros
  float dataF[3]; //Array para guardar datos flotantes
  gam.readA(dataI); //Leemos valores de acelerómetro
  Serial.print("A=["+String(dataI[0])+","+String(dataI[1])+","+String(dataI[2])+"]");
  gam.angleA(dataF); //Leemos ángulos de acelerómetro
  Serial.print(" aA=["+String(dataF[0])+","+String(dataF[1])+","+String(dataF[2])+"]");
  gam.readG(dataI); //Leemos valores de giróscopo
  Serial.print(" G=["+String(dataI[0])+","+String(dataI[1])+","+String(dataI[2])+"]");
  gam.angleG(dataF); //Leemos ángulos de giróscopo
  Serial.print(" aG=["+String(dataF[0])+","+String(dataF[1])+","+String(dataF[2])+"]");
  //Si conseguimos datos del Magnetómetro...los mostramos
  if(hasM) {
   gam.readM(dataI);
   Serial.print(" M=["+String(dataI[0])+","+String(dataI[1])+","+String(dataI[2])+"]");
  }
  Serial.println();
  digitalWrite(pinLED,LOW); //Apagamos el led integrado
  delay(300);
}