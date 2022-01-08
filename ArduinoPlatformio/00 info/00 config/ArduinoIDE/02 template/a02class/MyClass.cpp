//Comprobamos si ya se ha definido una etiqueta para evitar que se ejecute más de una vez
#ifndef MyClass_cpp
#define MyClass_cpp

#include <Arduino.h>
#include "MyClass.h" //Como siempre llamamos al archivo de cabecera

//Definimos el método get
template <class T>
T MyClass<T>::get()
{
  //Simplemente devuelve el valor de la variable privada
  return _value;
}

//Definimos el método set
template <class T>
void MyClass<T>::set(T value)
{
  //Simplemente guarda el parámetro en la variable privada
  _value=value;
}

#endif
