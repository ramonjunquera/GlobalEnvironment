/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: función xDelay
  Material adicional: placa ESP32
  Descripción:

  Cuando necesitamos detener temporalmente la ejecución de una tarea podemos
  tomar nota del tiempo actual y esperar hasta que se alcance el tiempo
  deseado, comprobando contínuamente el reloj.
  Esto consume el 100% de la CPU.
  Cuando tenemos varias tareas corriendo en paralelo, no nos interesa utilizar
  este método, porque cada tarea utilizará el 100% de su tiempo de CPU asignado.
  En FreeRTOS tenemos la función xDelay para solventarlo.
  Cuando la llamamos, simplemente indica al gestor de tareas que no debe continuar
  hasta que pase cierto tiempo (en milisegundos). Ese tiempo lo dedicará a la
  ejecución del resto de tareas.
  Afortunadamente la implementación del clasico delay() de Arduino llama
  internamente a xDelay.
  Por lo tanto, podemos utilizar xDelay o delay indistintamente para ESP32.
*/

#include <Arduino.h>

void sayCore(void *parameter) {
  while(1) {
    Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
    vTaskDelay(1000); //Sustituye a delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  //Asignamos los parámetros al objeto tarea y lo lanzamos
  xTaskCreatePinnedToCore(sayCore,"",2000,NULL,1,NULL,0);
}

void loop() {
  //Nada especial que hacer aquí
}