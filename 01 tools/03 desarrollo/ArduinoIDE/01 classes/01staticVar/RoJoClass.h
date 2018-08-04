/*
  Nombre de la librería: RoJoClass.h
  Versión: 20180211
  Autor: Ramón Junquera
  Descripción:
    Clase de prueba para variables estáticas
*/

#ifndef RoJoClass_h
#define RoJoClass_h

#include <Arduino.h>

class RoJoClass
{
  public:
    static int s; //Variable estática
    int d; //Variable dinámica
    void print(); //Envía las variables por el puerto serie
}; //Punto y coma obligatorio para que no de error

#endif

