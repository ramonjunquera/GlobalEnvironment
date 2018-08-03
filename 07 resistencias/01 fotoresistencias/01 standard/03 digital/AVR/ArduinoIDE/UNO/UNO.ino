/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia
  Objetivo: Funcionamiento de una fotorresistencia estándar
  Material: breadboard, fotorresistencia, resistencia de 10Kohmios, cables, Arduino UNO

  Descripción:
  El circuito de este ejercicio es igual que el primero.

  En esta ocasión sólo nos interesa el valor digital de la fotoresistencia.

  Cómo calcula su valor un pin digital si lo que introducimos es una señal analógica?
  Todos los vlores por debajo de la mitad del máximo valor se tomarán como 0.
  El resto como 1.
  Si sabemos que el valor analógico no es constante y está práximo a la mitad (512), nos
  encontraremos que el valor digital obtenido fluctua. Por eso en valores digitales nos
  deberíamos alejar de los valores medios para que no haya dudas.

  En el primer ejercicio comprobábamos que los valores obtenidos estaban tanto por encima
  como por debajo del valor central. Por lo tanto, no deberíamos tener problemas.

  Para no complicar el circuito, utilizaremos el led integrado en placa para mostrar
  el valor digital leido.

  Pero... si vamoa a utilizar un pin digital, porqué sigue conectada la fotoresistencia al
  primer pin analógico (A0)?. Cómo leeremos el valor digital?
  Arduino UNO tiene 6 pines de entrada analógicos. Pero si no los utilizamos se pueden 
  considerar digitales.
  La numeración es correlativa al último pin digital etiquetado. Así que el pin A0
  corresponde con el pin digital 14,...
  La siguiente tabla muestra las correspondencias:

  analógico digital
        A0 = 14
        A1 = 15
        A2 = 16
        A3 = 17
        A4 = 18
        A5 = 19
        A6 = 20

  Resultado:
  El led integrado se mantiene encendido mientras haya suficiente luz.
*/

#include <Arduino.h>

void setup()
{
  //Configuramos el pin 13 como salida para poder escribir en él
  pinMode(13, OUTPUT);
}

void loop()
{
  //El estado del led integrado (13) será el mismo que el del pin digital 14 (A0)
  digitalWrite(13,digitalRead(14));
}
