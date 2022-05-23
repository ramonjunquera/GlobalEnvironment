/*
  Tema: Teclado de membrana de 4x4
  Objetivo: Demo de librería
  Fecha: 20220521
  Autor: Ramón Junquera

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
#include <RoJoPad4x4.h>

#ifdef ESP32 
  RoJoPad4x4 pad(13,12,14,27,26,25,33,32); //LOLIN32
#elif defined __arm__
  RoJoPad4x4 pad(18,23,24,25,12,16,20,21); //RPi
#else
  RoJoPad4x4 pad(13,12,11,10,9,8,7,6); //Mega
#endif

void setup() {
  Serial.begin(115200);
  Serial.println(F("Preparado!"));
}

void loop() {
  //Creamos una variable para guardar el carácter que se ha pulsado
  //Si es distinto de NULL, lo enviamos por el puerto serie
  if(char c=pad.get()) Serial.println(c);
}


