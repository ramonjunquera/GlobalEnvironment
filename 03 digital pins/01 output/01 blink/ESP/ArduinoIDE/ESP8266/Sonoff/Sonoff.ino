/*
  Autor: Ramón Junquera
  
  Tema: Sonoff
  
  Objetivo: Comprobar que la comunicación funciona correctamente
  
  Material adicional: Sonoff, cables, FTDI

  Una vez conectados los cables del FTDI a la placa de Sonoff de la siguiente manera:

  Sonoff  FTDI
  ------  ----
  3.3V    3.3V
  RX      TX
  TX      RX
  GND     GND

  Debemos configurar el dispositivo con estos parámetros:
    Placa: “Generic ESP8266 Module”
    Flash Mode: “DIO”
    Flash Frequency: “40MHz”
    Upload Using: “Serial”
    CPU Frequency: “80 MHz”
    Flash Size: “1m(64K SPIFFS)”
    Reset Method: “ck”
    Upload Speed: “115200”
    
  Es muy importante recordar que para que Sonoff se encuentre en modo de recepción de 
  datos, se debe arrancar manteniendo el interruptor pulsado.
  Si no se hace, el IDE no podrá conectar co el dispositivo.

  Este el el listado de GPIO (pines) disponibles:
  
  GPIO  Función
  ----  --------
    0   Interruptor (pulsador)
    1   TX
    3   RX
   12   Relé
   13   Led

  En el siguiente ejemplo hacermos parpadear el led integrado.
 */

#include <Arduino.h>

const byte pinLed=13;

void setup()
{
  //Inicializamos el pin del led integrado como salida
  pinMode(pinLed,OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(pinLed,HIGH);
  //Esperamos un momento
  delay(1000);
  //Apagamos el led
  digitalWrite(pinLed,LOW);
  //Esperamos un momento
  delay(1000);
}
