/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Eliminación de tarea concreta
  Material adicional: placa ESP32
  Descripción:

  Demostración de cómo eliminar una tarea concreta.
  Para ello utilizamos el objeto de tarea.
  
  El ejemplo lanza una tarea que muestra los milisegundos transcurridos desde
  el inicio y el core de ejecución de manera indefinida, cada segundo.
  Transcurridos 10 segundos, forzamos la eliminación de la tarea.
  Cuando esto ocurre, dejamos de ver los mensajes que generaba.
*/

#include <Arduino.h>

//Definimos una tarea para poder lanzar el proceso
TaskHandle_t myTask;

void sayCore(void *parameter) {
  //Muestra el core de ejecución cada segundo indefinidamente
  while(1) {
    Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  //Creamos y lanzamos la tarea contra el core 0
  xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,&myTask,0);
  //Esperaremos 10 segundos
  delay(10000);
  //Forzaremos la eliminación de la tarea
  vTaskDelete(myTask);
  Serial.println("Tarea eliminada");
}

void loop() {
  //Nada especial que hacer aquí
}