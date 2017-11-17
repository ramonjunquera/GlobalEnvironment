/*
  Autor: Ramón Junquera
  Tema: Pines digitales de salida
  Objetivo: Configuración y escritura de pines digitales en placas Arduino
  Material adicional: placa Arduino
  Descripción:

  En el siguiente ejemplo aprenderemos el uso de las siguientes funciones:
  - Función setup. Sólo se ejecuta una vez.
  - Función loop. Se ejecuta contínuamente después de la función setup.
  - Configuración un pin para que sea de salida.
  - Función de espera que detiene el flujo del programa.
  - Función de escritura en un pin digital

  La función delay tiene un parámetro. Es el tiempo de espera en milisegundos.

  El programa configura un pin como salida en su inicialización (setup).
  Enciende el led, espera, lo apaga, espera y repite el ciclo.

  Aprovechamos que Arduino UNO tiene un led integrado en placa unido al pin 13.
  Por esa razón no necesitamos ningún componente.

  Puesto que el pin no permite PWM, el led asociado tampoco.
 */

#include<Arduino.h>

//Definimos el pin del led
const byte pinLed=13; //led integrado

void setup()
{
  //Inicializamos el pin del led integrado como salida
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(pinLed, HIGH);
  //Esperamos un segundo
  delay(1000);
  //Apagamos el led
  digitalWrite(pinLed, LOW);
  //Esperamos un segundo
  delay(1000);
}
