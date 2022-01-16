/*
  Autor: Ramón Junquera
  Fecha: 20200305
  Tema: Acelerómetro MPU9250
  Objetivo: Lectura de los valores
  Material: M5Stack Watch, M5Stick-C, M5Stack Fire
  Descripción:
    Inicializa los sensores y muestra los valores contínuamente por el puerto serie
  Nota:
    Ni M5Stick-C ni Atom Matrix tienen magnetómetro. Sus lecturas no se mostrarán.
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

RoJoMPU9250 GAM; //Giróscopo, Acelerómetro y Magnetómetro
bool hasM; //Tiene magnetómetro?

void setup() {
   Serial.begin(115200);
   //Mientras se inicializa el puerto serie, hacemos parpadear el led integrado
   pinMode(pinLED,OUTPUT);
   for(byte i=0;i<20;i++) {
      digitalWrite(pinLED,!digitalRead(pinLED));
      delay(200);
   }
   GAM.begin(pinSDA,pinSCL); //Inicialización de sensores
}
 
void loop() {
   digitalWrite(pinLED,HIGH); //Mientras tomamos y enviamos datos, encendemos el led integrado
   int16_t data[3]; //Array para guardar datos de ejes de un sensor
   GAM.readG(data); //Leemos datos de Giróscopo
   Serial.print("["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   GAM.readA(data); //Leemos datos de Acelerómetro
   Serial.print("\t["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   //Si conseguimos datos del Magnetómetro...los mostramos
   if(GAM.hasM()) {
     GAM.readM(data);
     Serial.print("\t["+String(data[0])+"\t"+String(data[1])+"\t"+String(data[2])+"]");
   }
   Serial.println();
   digitalWrite(pinLED,LOW); //Apagamos el led integrado
   delay(300);
}