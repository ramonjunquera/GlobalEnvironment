/*
  Autor: Ramón Junquera
  Fecha: 20221109
  Tema: Motores paso a paso
  Objetivo: Demo de librería RoJoStepper
  Descripción:
    En la definición de constantes iniciales se puede seleccionar el tipo de motor y la secuencia a utilizar.
    Se hace girar el eje en cada sentido, con una pausa entre movimientos.
*/

#include <Arduino.h>
#include <RoJoStepper.h> //Gestión de motores paso a paso

RoJoStepper stepper;

//Dejar marcado sólo el motor utilizado
#define TinyMiniA
//#define S28BYJ48

const byte seqType=2; //Tipo de secuencia 0=MaxPrecission,1=MaxTorque,2=MinPower

//Pinout
//El contenido del array pinout serán los pines de conexión 
#ifdef ARDUINO_ARCH_AVR
  const byte pinout[4]={3,4,2,5}; //B-,A-,B+,A+ / IN4,IN3,IN2,IN1
#elif defined(ESP8266)
  const byte pinout[4]={D5,D6,D0,D7}; //B-,A-,B+,A+ / IN4,IN3,IN2,IN1
#elif defined(ESP32)
  const byte pinout[4]={17,21,16,22}; //B-,A-,B+,A+ / IN4,IN3,IN2,IN1
#elif defined(__arm__)
  const byte pinout[4]={26,21,19,20}; //B-,A-,B+,A+ / IN4,IN3,IN2,IN1
#endif

void setup() {
  #ifdef TinyMiniA
    #ifdef ARDUINO_ARCH_AVR //Si se alimenta con 5V
      switch(seqType) {
        case 0: //MaxPrecission
          stepper.begin(1,300);
          break;
        case 1: //MaxTorque
          stepper.begin(1,580);
          break;
        case 2: //MinPower
          stepper.begin(1,690);
          break;
      }
    #else //Si se alimenta con 3.3V
      switch(seqType) {
        case 0: //MaxPrecission
          stepper.begin(1,490);
          break;
        case 1: //MaxTorque
          stepper.begin(1,870);
          break;
        case 2: //MinPower
          stepper.begin(1,1000); //No tiene suficiente voltaje como para moverlo!!!
          break;
      }
    #endif
  #elif defined(S28BYJ48)
    switch(seqType) {
      case 0: //MaxPrecission
        stepper.begin(1,970);
        break;
      case 1: //MaxTorque
        stepper.begin(1,1980);
        break;
      case 2: //MinPower
        //stepper.begin(1,1980);
        stepper.begin(1,2030);
        break;
    }
  #endif
  stepper.setCfg(0,pinout,seqType);
  stepper.enable(0);
}

void loop() {
  stepper.move(0,1700); //Mover motor 0 a 1700 y esperar hasta que termine
  delay(1000);
  stepper.move(); //Mover motor 0 a 0 y esperar hasta que termine
  delay(1000);
}
