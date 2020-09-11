/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Eliminación de tarea en curso
  Material adicional: placa ESP32
  Descripción:

  No es obligatorio que todas las tareas contengan un bucle infinito.
  Una tarea puede ser creada, llamada y cuando finalize, eliminada para que
  en un futuro pueda ser repetido el proceso.

  La función vTaskDelete(NULL) permite detener y eliminar el objeto de la
  tarea.

  En el ejemplo se crea, llama y destruye la tarea por cada ciclo de loop.
*/

#include <Arduino.h>

//Definimos una tarea para poder lanzar el proceso
TaskHandle_t myTask;

void sayCore(void *parameter) {
  Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
  //Finalizamos la tarea actual y la eliminamos
  vTaskDelete(NULL);
  //Esta línea no se ejecutará nunca porque el objeto de la tarea de destuye antes
  Serial.println("nobody reads me!");
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos y lanzamos una tarea contra el core 0
  xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,&myTask,0);
  //Esperamos un momento
  delay(1000);
}