/*
  Autor: Ramón Junquera
  Fecha: 20191029
  Tema: Pines analógicos de entrada
  Objetivo: Aprender cómo funcionan las lecturas analógicas y cómo se comporta un potenciómetro
  Material: breadboard, potenciómetro de 10 Kohmios, cables y placa ATtiny85

  Descripción:
  El programa enviará al puerto serie el valor leido por el puerto analógico cada segundo.
  Podremos comprobar como cambia al mover el potenciómetro.
    
  Resultado:
  Vemos el valor del pin analógico en el monitor serie en tiempo real
*/

#include <Arduino.h>
#include <DigiCDC.h> //Gestión de puerto serie

void setup() {
  SerialUSB.begin(); //Inicializamos el puerto serie
}

void loop() {
  //Leemos el valore el pin analógico A1 (pin 2) y lo enviamos por el puerto serie
  //Debería ser un número entero entre 0 y 1023
  SerialUSB.println(analogRead(A1));
  //Esperamos un momento hasta repetir la lectura
  SerialUSB.delay(1000);
}
