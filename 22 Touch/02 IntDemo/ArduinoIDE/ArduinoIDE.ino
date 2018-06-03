/*
  Autor: Ramón Junquera
  Fecha: 20180603
  Tema: ESP32 Touch pins
  Objetivo: Definición de interripciones en pines touch
  Material adicional: placa ESP32
  Descripción:
    Puesto que los pines Touch no son digitales, no podemos utilizar las funciones
    estándar para definir interrupciones.
    En su lugar utilizaremos el comando touchAttachInterrupt que acepta 3 parámetros:
    - Número de pin
    - Función a la que llamar
    - Diferencia mínima entre la lectura actual y la lectura en vacío.

    En este ejemplo usaremos los pines touch 2 y 3, correspondientes a los pines 2 y 15.
    Indicaremos que si se detecta una diferencia superior a 40 desde el valor del pin en
    vacío (si conexión a nada), debe saltar la interrupción.
    Cuando menor sea este valor, más sensible será, pero corremos el riesgo de tener 
    falsos positivos.
    Las interrupciones de los pines touch no diferencian entre RAISE o LOW como las digitales.
    Sólo se contemplan las del tipo CHANGE.
*/

#include <Arduino.h>

int threshold = 40;
bool touch1detected = false;
bool touch2detected = false;

void gotTouch1()
{
 touch1detected = true;
}

void gotTouch2()
{
 touch2detected = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Interrupt Test");
  touchAttachInterrupt(T2, gotTouch1, threshold);
  touchAttachInterrupt(T3, gotTouch2, threshold);
}

void loop(){
  if(touch1detected){
    touch1detected = false;
    Serial.println("Touch 1 detected"+String(millis()));
  }
  if(touch2detected){
    touch2detected = false;
    Serial.println("Touch 2 detected"+String(millis()));
  }
}
