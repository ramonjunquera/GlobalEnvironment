/*
  Autor: Ramón Junquera
  Fecha: 20230102
  Tema: Filamentos de led
  
  Los filamentos admiten un voltaje máximo de 3V.
  Para protegerlos, utilizamos resistencias 10 ohmios.
  Aunque el PWM tiene un rango [0,1023], a partir del valor 300 no se nota mucha diferencia,
  así que lo limitaremos.

  Cada uno de los leds del circuito representa un filamento.

  Encenderemos y apagaremos progresivamente cada uno de los filamentos.
*/

#include <Arduino.h>

byte pinout[]={D1,D2,D3,D4};

void setup() {
}

void loop() {
  int16_t level=0;
  byte index=0;
  int16_t delta=1;
  while(1) {
    analogWrite(pinout[index],level);
    delay(2);
    level+=delta;
    if(level==300) delta=-delta;
    else if(level==0) {
      delta=-delta;
      index=(index+1)%4;
    }
  }
}
