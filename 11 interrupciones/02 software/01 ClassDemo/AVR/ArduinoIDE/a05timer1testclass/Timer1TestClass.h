/*
  Descripción:
    Definición de la clase Timer1TestClass como ejemplo de gestión de interrupciones desde una clase.

    La clase no tiene constructor ni destructor
    Tiene una variable privada en la que se guarda el pin afectado por el parpadeo
    El método PinLed permite definir el pin afectado
    El método Run es llamado cada vez que salta la interrupción

    Es obligatorio definir la función a la que se llama cuando salta una interrupción. Esto lo hacemos en este mismo archivo.
    La función no puede tener parámetros y sólo puede utilizar valores de variables globales.
    Pero como poco antes de la definición del la función hemos definido una variable global con una instancia de la clase,
    referenciamos a uno de sus métodos (Run) y así podrá hacer uso de todas las variables y métodos privados de la clase.
*/

#ifndef RoJoClass_h
#define RoJoClass_h

#include <Arduino.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

class Timer1TestClass
{
  private:  //Definición de métodos/variables privadas
    byte _pinLed=255; //Guardará el valor del pin del led que debe parpadear. Inicialmente ninguno
  public: //Definición de métodos/variables públicas
    void pinLed(byte pinLed); //Definición del pin del led que debe parpadear
    void run(); //Función llamada en cada interrupción
}; //Punto y coma obligatorio para que no de error

//En el .cpp se ha definido timerClass como variable global, que también queremos utilizar aquí
//Por eso la referenciamos con extern
extern Timer1TestClass timerClass;

#endif
