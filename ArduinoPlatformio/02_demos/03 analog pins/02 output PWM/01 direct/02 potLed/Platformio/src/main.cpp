/*
  Autor: Ramón Junquera
  Fecha: 20191001
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Descripción:
    Copiamos contínuamente el valor analógico como nivel PWM del led.
    El problema es que el valor analógico tiene 1024 niveles y el de PWM sólo 256 en las placas Arduino.
    Podríamos solventarlo dividiendo en entrada analógica por 4.
    Sólo como muestra usaremos la función map, que calcula con una regla de tres el valor de salida.
      valor resultante = map(valor a convertir, límite inferior de rango de entrada, límite superior de rango
      de entrada, límite inferior de rango de salida, límite superior de rango de salida).
    
  Resultado:
    Al girar el potenciómetro hacemos que el led se ilumine más o menos.
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = 3;
const byte pinAnalog = A5;

void setup() {
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
}

void loop() {
  //Escribimos en el pin análogico del led el valor leido de la entrada analógica A0, convertida al rango adecuado
  analogWrite(pinLed,map(analogRead(pinAnalog),0,1023,0,255));
}
