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
    referenciamos a uno de sus métodos (Run) y así podrá hacer uso de todoas las variables y métodos privados de la clase.
*/

#include <Arduino.h>
//Como trabajaremos directamente con los registros del procesador, incluiremos las librerías de gestión del fabricante
//del procesador. No hacemos referencia a librerías de Arduino.
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "Timer1TestClass.h"

void Timer1TestClass::pinLed(byte pinLed)
{
  //Método para seleccionarl el pin del led que debe parpadear
  
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led para que sea de salida
  _pinLed=pinLed; //Guardamos el valor del pin en la variable privada de clase
  cli(); //Deshabilitamos las interrupciones globales
  TCCR1A=0; //Fijamos el registro TCCR1A a 0 para que se mantengan los valores por defecto de la forma de onda
  TCCR1B=B100; //Activamos el prescaler 256 con el que se obtiene un periodo de un segundo (aprox).
  TIMSK1=1; //En el registro TIMSK1 activamos el primer bit (TOIE1) para que salte la interrupción al llegar al máximo
  sei(); //Volvemos a activar las interrupciones
}

void Timer1TestClass::run()
{
  //Método llamado cuando salta una interrupción

  //Si tenemos un pinLed válido...
  if(_pinLed<255)
  {
    //Cambiamos el estado del led
    digitalWrite(_pinLed,!digitalRead(_pinLed));
  }
}

//Aquí finaliza la definición de métodos de la clase
//Pero incluimos en el mismo archivo código que nos ayuda con la gestión

//Definimos timerClass como variable global de la clase
Timer1TestClass timerClass;

ISR(TIMER1_OVF_vect)
{
  //Función a la que se llama cuando se produce una interrupción
  //Con el parámetro TIMER1_OVF_vect indicamos que se llama cuando se produce el desbordamiento (overflow) del
  //contador del timer 1

  //Llamamos al método de la clase
  timerClass.run();
}


