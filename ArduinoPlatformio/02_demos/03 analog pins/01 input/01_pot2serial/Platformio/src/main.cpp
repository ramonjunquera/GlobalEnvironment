/*
  Tema: Pines analógicos de entrada
  Fecha: 20211019
  Autor: Ramón Junquera
  Objetivo: Aprender cómo funcionan las lecturas analógicas y cómo se comporta un potenciómetro
  Material: breadboard, potenciómetro de 10 Kohmios, cables y placa ESP32

  Descripción:
  El programa enviará al puerto serie el valor leido por el puerto analógico diez veces por
  segundo.
  Podremos comprobar como cambia al mover el potenciómetro.

  Recordemos que las placas ESP32 tienen una resolución de 12 bits = 4096 niveles.
  El resto tiene sólo 10bits = 1024 niveles.

  Las placas Arduino trabajan con 5V. El valor 1024 corresponde con 5V.
  ESP trabaja con 3.3V. No deberíamos conectar un pin ADC a más de este voltaje.
  Si queremos hacerlo, debeos asegurarnos de que ponemos una resistencia lo suficientemente
  grande como para vajar el voltaje máximo por debajo de los 3.3V.
      
  Resultado:
  Vemos el valor del pin analógico en el monitor serie en tiempo real
*/

#include <Arduino.h>

#ifdef ESP32 //Si es una ESP32
  const byte pinADC=12;
#elif defined(ARDUINO_AVR_ATTINYX5) //Si es una ATtiny85
  const byte pinADC=2;
#else //Para cualquier otro dispositivo
  const byte pinADC=A0;
#endif

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(pinADC));
  delay(100);
}
