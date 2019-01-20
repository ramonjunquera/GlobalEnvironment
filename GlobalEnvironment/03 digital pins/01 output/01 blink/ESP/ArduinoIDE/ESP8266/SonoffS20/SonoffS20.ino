/*
  Autor: Ramón Junquera
  
  Tema: Sonoff S20
  
  Objetivo: Comprobar que la comunicación funciona correctamente
  
  Material adicional: Sonoff S20, cables, FTDI

  Una vez conectados los cables del FTDI a la placa de Sonoff de la siguiente manera:

  SonoffS20  FTDI
  --------  ----
  3.3V      3.3V
  RX        TX
  TX        RX
  GND       GND

  Debemos configurar el dispositivo con estos parámetros:
    Placa: “Generic ESP8266 Module”
    Flash Mode: “DOUT”
    Flash Frequency: “40MHz”
    Upload Using: “Serial”
    CPU Frequency: “80 MHz”
    Flash Size: “1m(64K SPIFFS)”
    Reset Method: “ck”
    Upload Speed: “115200”
    
  Es muy importante recordar que para que Sonoff S20 se encuentre en modo de recepción de 
  datos, se debe arrancar manteniendo el interruptor pulsado.
  Si no se hace, el IDE no podrá conectar con el dispositivo.

  Atención al parámetro "Flash mode", que debe ser DOUT y no DIO como siempre.

  Este el el listado de GPIO (pines) disponibles:
  
  GPIO  Función
  ----  --------
    0   Interruptor (pulsador)
    1   TX
    3   RX
   12   Relé y led azul
   13   Led verde

  En el siguiente ejemplo hacermos parpadear el led verde.
 */

#include <Arduino.h>

const byte pinLed=13;

void setup()
{
  //Inicializamos el pin del led integrado como salida
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(pinLed, HIGH);
  //Esperamos un segundo
  delay(1000);
  //Apagamos el led
  digitalWrite(pinLed, LOW);
  //Esperamos un segundo
  delay(1000);
}
