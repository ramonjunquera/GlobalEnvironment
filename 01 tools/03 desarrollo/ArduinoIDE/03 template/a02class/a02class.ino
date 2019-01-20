/*
  Autor: Ramón Junquera
  Tema: Template
  Objetivo: Ejemplo de clase con template
  Fecha: 20181016
  Material: cualquier placa

  Descripción:
  En el ejemplo actual se define una clase muy simple con tipos indefinidos.
  Aunque sabemos que se puede definir una clase inclusi en el mismo programa principal, optamos
  por seguir manteniendo la estructura habitual de un archivo .h para la definición de cabeceras
  y otro .cpp para la definición de los métodos.
  Desde el progrma principal añadimos el include al archivo de cabeceras, como siempre.

  El IDE de Arduino tiene ciertas limitaciones para la definición de clase con template (con
  tipos indefinidos) y para solventarlas debemos añadir algunas peculiaridades que una clase
  normal no necesita:
  - El archivo .cpp también debe comenzar comprobando si se ha ejecutado previamente, para no
    declarar varias veces los mismos métodos:
    Comenzamos con:
      #ifndef MyClass_cpp
      #define MyClass_cpp
    Y finalizamos con:
      #endif
    Es el mismo sistema que utilizamos siempre con los archivos de cabecera.
  - Al final del archivo de cabecera (y antes del #endif) llamamos al archivo .cpp con un include.
    Esto obliga a que después de definir las cabeceras se describan los métodos.
  Con estos cambios podemos utilizar la clase con cierta normalidad.
  Al contrario que con las funciones, no tenemos la limitación de definir los métodos o clases en
  una sóla línea para que reconozca el template.
  En el ejemplo todas las declaraciones tienes dos líneas, pero se pueden hacer en una sóla.

  Si utilizamos template para definir una clase, sus métodos siempre deben incluir el mismo
  template que la clase. Incluso aunque no hagan uso de los tipos del template.
 */

#include <Arduino.h>
#include "MyClass.h" //Clase con tipos no definidos

void setup()
{
  //Inicializamos el puerto seria
  Serial.begin(115200);
  //Creamos un objeto con una instancia de nuestra clase indicando que el tipo de variable
  //será un entero
  MyClass<int> obj;
  //Asignamos un valor (entero) a la variable interna de la clase
  obj.set(7);
  //Creamos una variable entera en la que guardaremos el valor de la variable interna
  //de la clase
  int myValue=obj.get();
  //Enviamos el valor por el puerto serie
  Serial.println(myValue);
}

void loop()
{
  //Nada especial que hacer aquí
}
