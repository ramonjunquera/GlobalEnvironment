/*
  Autor: Ramón Junquera
  Tema: Template
  Objetivo: Ejemplo de función con template
  Fecha: 20181016
  Material: cualquier placa

  Descripción:
  Utilizaremos template cuando queramos crear una función o clase para la que no tenemos 
  definidos todos sus tipos de variables.
  En este programa usaremos mostraremos cómo definir funciones.
  Se muestrasn tres ejemplos.
    1.- Ejemplo con un sólo parámetro indefinido
    2.- Ejemplo con dos parámetros con tipo indefinido
    3.- Ejemplo con dos parámetros con tipo indefinido y distintos entre sí
    
  Nota:
    Al definir los tipos de variables en la template utilizaremos la palabra 'class'.
    Veremos muchos ejemplos de C++ en los que se utiliza la palabra 'typename'.
    Ambas son equivalentes.
 */

#include <Arduino.h>

//La definición de la función con template debe hacerse en una sóla linea
//Si la intentamos dividir en dos líneas, como habitualmente se hace en C++:
//  template <class T>
//  T x2(T value)
//El compilador dará un error porque no relaciona las líneas
//Este problema sólo ocurre en el IDE de Arduino
template <class T> T x2(T value)
{
  //Función que multiplica por 2 el parámetro y devuelve el resultado
  //El tipo de la variable del parámetro y que devuelve se puede seleccionar
  return 2*value;
}

template <class T> T sum(T v1,T v2)
{
  //Función que suma los valores de los dos parámetros del mismo tipo y devuelve el resultado
  return v1+v2;
}

template <class T1,class T2> T1 product(T1 value,T2 times)
{
  //Función que devuelve el primer parámetro repetido tantas veces como el valor del segundo

  //Definimos una variable para la respuesta del tipo del primer parámetro y no le damos valor
  T1 answer;
  //Repetimos tantas veces como el segundo parámetro en un bucle cuyo contador es del mismo tipo
  //que el segundo parámetro y en cada ciclo sumamos a la respuesta el valor del primer parámetro.
  for(T2 i=0;i<times;i++) answer+=value;
  //Devolvemos la respuesta
  return answer;
}

void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);
  //Creamos una variable entera y le asignamos valor
  int i1=3;
  //Llamamos a ls función que multiplica por 2 indicando que trabajamos con enteros
  //Le pasamos como parámetro la variable entera y recogemos el resultado en otra
  //variable entera
  int i2=x2<int>(i1);
  //Mostramos el resultado
  Serial.println(i2);

  //Repetimos el ejemplo pero con variables decimales
  float f1=3.4;
  float f2=x2<float>(f1);
  Serial.println(f2);

  //Asignamos valores a dos variables enteras
  i1=2;
  i2=3;
  //Llamamos a la función que suma los dos parámetros indicando que utilizaremos enteros
  //Enviamos la respuesta directamente por el puerto serie
  Serial.println(sum<int>(i1,i2));

  //Repetimos la llamada a la función sum pero con variables de String
  //La suma de dos Strings es su concatenación
  String s1,s2;
  s1="Hola";
  s2=" amigo!";
  Serial.println(sum<String>(s1,s2));

  //Definimos una variable entera y otra entera sin signo
  i1=2;
  uint16_t ui1=1000;
  //Llamamos a la función que suma el primer parámetro tantas veces como el valor del segundo
  //Indicamos que el primer parámetro será un entero y el segundo un entero sin signo
  //Enviamos la respuesta directamente por el puerto serie
  Serial.println(product<int,uint16_t>(i1,ui1));

  //Repetimos la llamada a la función product pero el primer parámetro será un String y el
  //segundo un byte. El resultado será la concatenación.
  s1="Hello";
  byte b1=4;
  Serial.println(product<String,byte>(s1,b1));
}

void loop()
{
  //Nada especial que hacer aquí
}
