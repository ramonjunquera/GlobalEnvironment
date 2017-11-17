/*
  Autor: Ramón Junquera
  Tema: Pines analógicos de entrada
  Objetivo: Aprender cómo funcionan las lecturas analógicas y cómo se comporta un potenciómetro
  Material: breadboard, potenciómetro de 10 Kohmios, cables y placa ESP8266

  Descripción:
  El programa enviará al puerto serie el valor leido por el puerto analógico diez veces por
  segundo.
  Podremos conprobar como cambia al mover el potenciómetro.

  Las placas ESP8266 no suelen llegar el 0 porque detectan el poco voltaje que puede pasar.
  Y tampoco llegan al 1023, porque están calibradas para que 1000 corresponda con 3.3V.
  Pero tampoco se alcanza el 1000 nunca porque la placa siempre tiene un voltaje inferior a los 3.3V
  (sobre los 3.28V)
      
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
