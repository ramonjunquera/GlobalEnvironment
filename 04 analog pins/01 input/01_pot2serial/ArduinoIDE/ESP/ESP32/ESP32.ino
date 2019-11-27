/*
  Autor: Ramón Junquera
  Tema: Pines analógicos de entrada
  Objetivo: Aprender cómo funcionan las lecturas analógicas y cómo se comporta un potenciómetro
  Material: breadboard, potenciómetro de 10 Kohmios, cables y placa ESP32

  Descripción:
  El programa enviará al puerto serie el valor leido por el puerto analógico diez veces por
  segundo.
  Podremos conprobar como cambia al mover el potenciómetro.

  Recordemos que las placas ESP32 tienen una resolución de 12 bits = 4096 niveles.
  En este caso el pin de entrada analógica está etiquetado como VP.
      
  Resultado:
  Vemos el valor del pin analógico en el monitor serie en tiempo real
*/

#include <Arduino.h>

void setup()
{
  //Inicializamos el puerto serie para que transmita a 115200 baudios
  Serial.begin(115200);
}

void loop()
{
  //Leemos el valore el pin analógico A0 y lo enviamos por el puerto serie
  Serial.println(analogRead(A0));
  //Esperamos un momento hasta repetir la lectura
  delay(100);
}
