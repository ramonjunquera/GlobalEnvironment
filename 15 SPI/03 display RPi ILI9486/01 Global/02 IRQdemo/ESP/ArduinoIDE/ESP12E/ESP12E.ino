/*
  Autor: Ramón Junquera
  Fecha: 20180414
  Tema: Touch Panel de ILI9486
  Objetivo: Prueba de concepto del funcionamiento del pinIRQ
  Material: placa ESP8266, display ILI9486
  
  Descripción:
  El touch panel del display ILI9486 tiene un pin (TP_IRQ) que indica en
  todo momento si se está pulsando el display.
    HIGH: no se está pulsando  
    LOW: se está pulsando
  Este pin permite implementar la técnica de interrupciones de hardware
  para consultar las coordenadas de pulsación y así evitar un polling
  contínuo.

  El ejercicio sólo duplica el estado de un pin en el led integrado.
  Recordamos que en ESP8266 el led integrado tiene un estado inverso.
  Por lo tanto, si no se pulsa el display, recibiremos un estado HIGH
  que al aplicarlo al led integrado se apagará.
  Al contrario, cuando pulsemos, se encenderá.

  Resultado: el led integrado se enciende al pulsar el display
 */

#include <Arduino.h>

//Definición de constantes
const byte pinLed=LED_BUILTIN; //led integrado
const byte pinIRQ=D1;

void setup()
{
  //Inicializamos el pin del led integrado como salida
  pinMode(pinLed,OUTPUT);
  //Inicializamos el pin TP_IRQ como entrada
  //No es necesario utilizar resistencias de pullup poque estamos
  //alimentando el display. El "interruptor" no queda nunca desconectado
  pinMode(pinIRQ,INPUT);
}

void loop()
{
  //Copiamos contínuamente el estado del pin de entrada sobre el led
  digitalWrite(pinLed,digitalRead(pinIRQ));
}
