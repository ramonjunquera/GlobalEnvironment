/*
  Autor: Ramón Junquera
  Fecha: 20191029
  Tema: Lectura de entradas analógicas
  Objetivo: Leer el valor analógico de un pin
  Material adicional: ATtiny85, breadboard, potenciómetro.

  Descripción:
  El proyecto consiste en leer el valor analógico de un pin y aplicar su valor al
  brillo de un led para que podamos comprobar que funciona correctamente.

  La descripción de los pines es la siguiente:
    Pin 0 → I2C SDA, PWM
    Pin 1 → PWM, LED integrado
    Pin 2 → I2C SCK, Analog(A1), INT0
    Pin 3 → Analog(A3)
    Pin 4 → PWM, Analog(A2)
    Pin 5 → RESET

  Según esto, tenemos 3 pines que podemos utilizar como entradas analógicas con los
  nombres de A1, A2 y A3.
  No existe la entrada analógica A0.

  En el ejemplo hemos utilizado el pin 1 para conectar un led externo.
  No hay ningún problema en usar el led integrado en placa.
  Lo importante es que el pin permita PWM.

  Hemos definido unas constantes globales que permiten cambiar muy fácilmente la definición
  de pines del proyecto.
  Gracias a eso, podemos experimentar y comprobar cómo efectívamente las entradas 
  analógicas coinciden con los pines indicados:

  entrada pin
    A1     2
    A2     4
    A3     3

  Nota:
  Es importante recalcar que A1 no es lo mismo que pin 1. A1 tiene un valor de tipo byte (un
  entero de 8 bits sin signo), que no corresponde con el valor 1.
  No es lo mismo analogRead(A1) que analogRead(1)
  
  Resultado:
  Podemos controlar el brillo del led integrado con el potenciómetro.
  Podemos cambiar fácilmente el pin analógico que leemos.

  Nota:
  En esta placa podríamos haber utilizado el led integrado del pin 1, puesto que también
  adminte pwm.
*/

#include <Arduino.h>

//Definición de pines
const byte ledPin=0;
const byte potPin=A1;

void setup() {        
  //Inicializamos el pin del led como salida
  pinMode(ledPin, OUTPUT);
}

void loop() {
  //Escribimos en el pin del led el valor leido de la entrada analógica, convertida al rango adecuado
  analogWrite(ledPin,map(analogRead(potPin),0,1023,0,255));
}
