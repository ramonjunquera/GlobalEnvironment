//Comprobamos si ya se ha definido una etiqueta para evitar que se ejecute más de una vez
#ifndef MyClass_h
#define MyClass_h

#include <Arduino.h>

//Definimos la clase en dos líneas (se podría hacer en una sóla)
template<class T>
class MyClass
{ 
  private:
    //Sólo tenemos una variable en privado del mismo tipo que el definido para la clase
    T _value;
  public:
    //Método para guardar el valor en la variable privada
    void set(T value);
    //Método para recuperar el valor de la variable privada
    T get();
};

//Llamamos al archivo en el que están definidos los métodos
//Esto es obligatorio cuando utilizamos clases con template
#include "MyClass.cpp"

#endif
