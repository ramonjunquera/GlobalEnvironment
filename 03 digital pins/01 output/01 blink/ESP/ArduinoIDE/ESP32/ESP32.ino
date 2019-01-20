/*
  Autor: Ramón Junquera
  Tema: Pines digitales de salida
  Fecha: 20181105
  Objetivo: Configuración y escritura de pines digitales en ESP32
  Material adicional: placa ESP32
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

  En casi todas las placas ESP32, el led integrado en placa viene asociado a
  la constante LED_BUILTIN. Si no fuese así, se debe sustituir por el pin correcto.
 */

#include <Arduino.h>

//#define LED_BUILTIN 22 //Descomentar si no tiene la constante definida como en el LOLIN32 Lite

//Definimos el pin del led
const byte pinLed=LED_BUILTIN; //led integrado

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
