/*
  Autor: Ramón Junquera
  Tema: Receptor AX-1838HS
  Objetivo: Funcionamiento de un receptor de infrarrojos AX-1838HS
  Material: breadboard, receptor de infrarrojos modelo AX-1838HS, pulsadore, led IR, cables y Arduino Nano.

  Descripción:
  Ahora que hemos comprobado el comportamiento analógico del sensor, probaremos qué ocurre si lo 
  conectamos a un pin digital.

  El circuito utilizado es muy parecido al anterior, excepto que en vez de conectar el sensor a A0
  lo hacemos al pin 2.

  El cálculo del valor digital de una señal analógica es sencillo. Se divide el rango total de valores en dos
  secciones iguales.
  En nuestro caso cada sección contendría 512 valores y los límites serían:
  
  Valor digital | Valor analógico mínimo | Valor analógico máximo
  ---------------------------------------------------------------
  0 = LOW       | 0                      | 511
  1 = HIGH      | 512                    | 1023
  ---------------------------------------------------------------

  Es cierto que los valores próximos a 512 se podrían poner en duda, o podrían fluctuar de un estado a otro
  fácilmente. Pero teniendo en cuenta los valores analógicos observados, sabemos que no se dará esta situación.

  En la práctica vemos que el valor digital casi siempre se mantiene en estado HIGH.
  Pero en cambio, podemos ver cómo se enciende el led de actividad del sensor.
  Parece que no se detecta actividad cuando soltamos (apagamos) el led emisor.
  Deducimos que todavía nos falta algo por descubrir.

  Resultado:
  El estado del pin digital al que está conectado el sensor NO cambia cuando se enciende un led infrarrojo frente a él.
*/

#include <Arduino.h>

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
}

void loop()
{
  //Leemos el valor del pin 2
  Serial.println(digitalRead(2));
  //Esperamos un momento hasta repetir la lectura
  delay(50);
}
