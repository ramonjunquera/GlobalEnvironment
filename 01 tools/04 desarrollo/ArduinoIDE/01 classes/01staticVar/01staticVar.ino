/*
  Autor: Ramón Junquera
  Tema: Clases
  Objetivo: Ejemplo de clase con variable estática
  Fecha: 20180211
  Material: cualquier placa

  Descripción:
  Una variable estática siempre pertenece a la clase y no a la instancia de ella que
  se pueda crear.
  Estas variables pueden ser escritas y leidas desde la clase como cualquier otra, pero
  sus valores serán compartidas por todas las instancias que se creen.
*/

#include <Arduino.h>
#include "RoJoClass.h"

void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);

  //Antes de crear ninguna instancia de la clase RoJoClass, podemos asignarle un valor a la
  //variable estática. la variable pertenece a la clase, no a la instancia
  RoJoClass::s=55;
  //Creamos una instancia de la clase
  RoJoClass class1;
  //Asignamos valor a una variable dinámica (perteneciente a la instancia)
  class1.d=13;
  //Mostramos los valores
  Serial.println("\nclass1");
  class1.print();
  //Se mostrarán los valores: s=55 y d=13

  //Creamos una segunda instancia de la clase
  RoJoClass class2;
  //Asignamos valor a la variable dinámica
  class2.d=23;
  //Asignamos valor a la variable estática a través de la instancia
  //Es lo mismo que RoJoClass::s=24;
  class2.s=24;
  //Los mostramos
  Serial.println("\nclass2");
  class2.print();
  //Se mostrarán los valores: s=24 y d=23
  
  //Volvemos a mostrar los valores de la primera clase
  Serial.println("\nclass1");
  class1.print();
  //Se mostrarán los valores: s=24 y d=13
  //Es debido a que la variable estática se ha sobreescrito para todas las instancias
}

void loop()
{
  //Nada especial que hacer aquí
}

