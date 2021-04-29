/*
  Nombre de la librería: RoJoClass.h
  Versión: 20180211
  Autor: Ramón Junquera
  Descripción:
    Clase de prueba para variables estáticas
*/

#ifndef RoJoClass_cpp
#define RoJoClass_cpp

#include <Arduino.h>
#include "RoJoClass.h"

//No tenemos ningún método que definir

//Importantísimo!!
//Necesitamos reservar memoria para albergar la variable estática
//De la misma manera que lo haríamos al declarar un método
//Si no lo hacemos, tendremos un error indicando que la variable estática no puede ser
//referenciada
int RoJoClass::s;

void RoJoClass::print()
{
  //Envía por el puerto serie los valores de las variables de la clase
  Serial.println("d=" + String(d));
  Serial.println("s=" + String(s));
}

#endif

