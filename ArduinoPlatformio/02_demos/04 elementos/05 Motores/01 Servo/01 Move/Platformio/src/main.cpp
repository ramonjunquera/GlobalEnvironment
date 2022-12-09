/*
  Autor: Ramón Junquera
  Tema: Servomotores
  Fecha: 20221110
  Objetivo: Demo de clase RoJoServo.h con un sólo servo

  Descripción:
  - Se posiciona el servo a 0 grados instantáneamente
  - Se esperan 2 segundos
  - Se gira de 0 a 90 grados a velocidad constante de 60 grados/s esperando a que finalice
  - Se gira de 90 a 180 grados a velocidad variable a 90 grados/s sin esperar a que finalice
  - Se esperan 2 segundos
  - Se inicia de nuevo la secuencia
*/

#include <Arduino.h>
#include <RoJoServo.h>

RoJoServo servo;

//Dejar marcado sólo el servo utilizado
//#define SG90
#define MG90S
//#define MG996R

//Pinout
#ifdef ARDUINO_ARCH_AVR
  const byte pin=9;
#elif defined(ESP8266)
  const byte pin=D5;
#elif defined(ESP32)
  const byte pin=13;
#elif defined(__arm__)
  const byte pin=18;
#endif

//Definición de duración de pulsos
#ifdef SG90
  #ifdef ARDUINO_ARCH_AVR
    const uint16_t us0=450;
    const uint16_t us180=2200;
  #else
    const uint16_t us0=700;
    const uint16_t us180=2500;
  #endif
#elif defined(MG90S)
  #ifdef ARDUINO_ARCH_AVR
    const uint16_t us0=200;
    const uint16_t us180=2150;
  #else
    const uint16_t us0=400;
    const uint16_t us180=2400;
  #endif
#elif defined(MG996R)
  const uint16_t us0=450;
  const uint16_t us180=2400;

#endif

void setup() {
  Serial.begin(115200);
  servo.begin(1,us180); //Inicialización. 1 servo. Duración de pulso de 180 grados por defecto
  servo.setCfg(0,pin,us0,us180); //Servo 0 en pin 9
  servo.enable(0); //Servo 0 activo
}

void loop() {
  servo.move(); //Servo 0 a 0 grados instantáneamente
  Serial.println("angle 0");
  servo.delayMs(2000);
  servo.move(0,90,1,60); //Servo 0 a 90 grados a 60 grados/segundo y esperamos
  Serial.println("angle 90");
  servo.move(0,180,2,90,false); //Servo 0 a 180 grados a 90 grados/segundo con velocidad variable y no esperamos
  Serial.println("angle 180");
  servo.delayMs(2000);
}