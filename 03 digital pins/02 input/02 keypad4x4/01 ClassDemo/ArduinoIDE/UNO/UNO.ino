/*
  Autor: Ramón Junquera
  Tema: Teclado de membrana de 4x4
  Objetivo: Demo de librería
  Material adicional: breadboard, cables, teclado de 4x4, Arduino UNO
  Descripción:
  Demostración de la librería de gestión del teclado de membrada de 4x4.
  La librería es muy sencilla. No gestiona efecto rebote porque en este tipo de teclado no suele ocurrir.
  Sólo tiene un método que devuelve las nuevas pulsaciones detectadas.
  la propia librería se encarga de la configuración de los pines.

  Envía por el puerto serie el carácter correspondiente a la nueva pulsación detectada.
     
  Resultado:
  Vemos en el monitor del puerto serie los caracteres de las pulsaciones
*/

#include <Arduino.h>
#include "RoJoKeypad4x4.h"

//Objeto de gestión. Indicamos todos los pines de conexión
RoJoKeypad4x4 pad(13,12,11,10,9,8,7,6);

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Preparado!"));
}

void loop()
{
  //Creamos una variable para guardar el carácter que se ha pulsado
  //Si es distinto de NULL, lo enviamos por el puerto serie
  if(char c=pad.get()) Serial.println(c);
}


