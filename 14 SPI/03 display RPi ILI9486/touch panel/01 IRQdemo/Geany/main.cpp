/*
  Autor: Ramón Junquera
  Fecha: 20190625
  Tema: Touch Panel de ILI9486
  Objetivo: Prueba de concepto del funcionamiento del pinIRQ
  Material: display ILI9486
  Dispositivos: RPi, ESP8266, ESP32, UNO, Nano, Mega
  
  Descripción:
  El touch panel del display ILI9486 tiene un pin (TP_IRQ) que indica en
  todo momento si se está pulsando el display.
    HIGH: no se está pulsando  
    LOW: se está pulsando
  Este pin permite implementar la técnica de interrupciones de hardware
  para consultar las coordenadas de pulsación y así evitar un polling
  contínuo.

  El programa muestra por el puerto serie el estado del pin IRQ
  contínuamente.
*/

#include <Arduino.h>

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinIRQ=D1;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinIRQ=17;
#elif defined(__arm__) //Si es una RPi...
  const byte pinIRQ=17;
#elif defined(ARDUINO_ARCH_AVR) //Si es placa Arduino
  const byte pinIRQ=2;
#endif

void setup()
{
  Serial.begin(115200);
  //Inicializamos el pin TP_IRQ como entrada
  //No es necesario utilizar resistencias de pullup poque estamos
  //alimentando el display. El "interruptor" no queda nunca desconectado
  pinMode(pinIRQ,INPUT);
}

void loop()
{
  //Mostramos por el puerto serie el estado de pin IRQ contínuamente
  Serial.println(digitalRead(pinIRQ));
  delay(10);
}
