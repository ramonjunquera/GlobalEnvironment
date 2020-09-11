/*
  Autor: Ramón Junquera
  Fecha: 25/08/2020
  Tema: Multitasking / FreeRTOS
  Objetivo: Identificar el core de ejecución
  Material adicional: placa ESP32
  
  Descripción:
  Las últimas versiones de las librerías de Espressif para ESP32 para 
  guardar compatibilidad con Arduino están basadas en FreeRTOS.

  FreeRTOS es un sistema operativo para microcontroladores, que permite
  la multitarea (ejecutar procesos en paralelo en uno o más cores).
  RTOS = Real-Time Operating System

  El microprocesador ESP32 tiene dos núcleos denominados 0 y 1.
  Ambos son idénticos.
  Cuando creamos un programa para ESP32, habitualmente es lanzado en el
  core 1. El 0 está activo, pero no ejecuta nada.

  En el ejemplo actual mostraremos a través del puerto serie, el identificador
  del core en el que se ejecutan los procesos por defecto.
  Lo haremos a través de la instrucción xPortGetCoreID()
*/

#include <Arduino.h>

void sayCore() {
  while(1) {
    Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  sayCore();
}

void loop() {
  //Nada especial que hacer aquí
}