/*
  Autor: Ramón Junquera
  Fecha: 20191004
  Tema: Buzzer (zumbador)
  Objetivo: Demo de librería para automatizar el uso del zumbador
  Descripción:
    Demostración de uso de librería RoJoMelody.h
  Resultado:
    Librería que nos facilita mucho la reproducción de canciones.
*/

#include <Arduino.h>
#include <RoJoMelody.h>

//Pinout
#ifdef ARDUINO_M5STACK_FIRE //Si es un M5Stack Fire...
  const byte pinBuzzer=25;
#elif defined(ARDUINO_M5Stick_Watch) //Si es un M5Stick Watch
  const byte pinBuzzer=26;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinBuzzer=8; //Coincide con el buzzer integrado de Maker UNO
#endif

//Declaración de variables globales
RoJoMelody melody(pinBuzzer); //Creamos objeto melody indicando el pin del buzzer

void setup() {
  //Creamos la canción para que se repita indefinidamente
  melody.setSong("E5-400B4-200C5D-400C-200B4A-390S-10A-200C5E-400D-200CB4-390S-10B-200C5D-400ECA4S-800D5-400F-200A-400G-200FE-600C-200E-400D-200CB4-390S-10B-200C5D-400ECA4S-800",true);
  //Comienza su ejecución
  melody.start();
}

void loop() {
  melody.play(); //Refrescamos continuamente la reproducción
}
