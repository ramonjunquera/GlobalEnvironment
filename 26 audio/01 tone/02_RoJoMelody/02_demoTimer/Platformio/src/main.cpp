/*
  Autor: Ramón Junquera
  Fecha: 20201116
  Tema: Buzzer (zumbador)
  Objetivo: Uso de librería RoJoMelody.h con timers
  Descripción:
    Para no tener que llamar contínuamente en cada ciclo al método play
    crearemos un timer que lo llama atutomáticamente cada milésima
    de segundo
  Resultado:
    La melodía es gestionada por la librería que es llamada por un timer.
    No necesitamos código específico para ello en el bucle principal.
*/

//Librerías
#include <Arduino.h>
#include <RoJoMelody.h> //Gestión de melodías
#if defined(ESP32) || defined(ESP8266)  //Si es un ESP
  #include <Ticker.h> //Gestión de timers para ESP
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  #include <RoJoTimerAVR1.h> //Gestión de timer1 para Arduino
#else
  #error Deviced not defined
#endif

//Pinout
#ifdef ARDUINO_M5STACK_FIRE //Si es un M5Stack Fire...
  const byte pinBuzzer=25;
#elif defined(ARDUINO_M5Stick_Watch) //Si es un M5Stick Watch
  const byte pinBuzzer=26;
#elif defined(ARDUINO_M5Stick_C) //Si es un M5Stick C+
  const byte pinBuzzer=2;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinBuzzer=8; //Coincide con el buzzer integrado de Maker UNO
#endif

//Declaración de variables globales
RoJoMelody melody(pinBuzzer); //Creamos objeto melody indicando el pin del buzzer
#if defined(ESP32) || defined(ESP8266)  //Si es un ESP
  Ticker playMelody;
#else //Si es una placa Arduino
  RoJoTimerAVR1 playMelody; //Utilizaremos el timer 1. El 2 lo utiliza internamente la función tone
#endif

//Llama al método play de la melodía
void callPlay() {
  melody.play();
}

void setup() {
  #ifdef ARDUINO_M5Stick_C
    //En M5Stick-C+ es necesario activar la alimentación desde el chip
    //AXP192 que gestiona la batería antes de usar el buzzer
    toneBegin();
  #endif
  #if defined(ESP32) || defined(ESP8266)  //Si es un ESP
    playMelody.attach_ms(1,callPlay); //Mantiene la reproducción de la librería cada milisegundo
  #else //Si es una placa Arduino
    playMelody.set(callPlay,1000); //Mantiene la reproducción de la librería cada milisegundo
    playMelody.start(); //Activamos el timer
  #endif
  //Creamos la canción para que se repita indefinidamente
  melody.setSong("E5-400B4-200C5D-400C-200B4A-390S-10A-200C5E-400D-200CB4-390S-10B-200C5D-400ECA4S-800D5-400F-200A-400G-200FE-600C-200E-400D-200CB4-390S-10B-200C5D-400ECA4S-800",true);
  //Comienza su ejecución
  melody.start();
}

void loop() {
  delay(1000); //Nada especial que hacer aquí
}

