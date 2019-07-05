/*
  Autor: Ramón Junquera
  Fecha: 03/05/2018
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

void sayCore(void *parameter)
{
  //Muestra el core de ejecución cada segundo indefinidamente
  while(1)
  {
    Serial.println(String(millis())+" running in core "+String(xPortGetCoreID()));
    delay(1000);
  }
}

void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);
  //Creamos y lanzamos la tarea contra el core 0
  xTaskCreatePinnedToCore(sayCore,"task1",1000,NULL,1,&myTask,0);
  //Esperaremos 10 segundos
  delay(10000);
  //Forzaremos la eliminación de la tarea
  vTaskDelete(myTask);
  Serial.println("Tarea eliminada");
}

void loop()
{
  //Nada especial que hacer aquí
}
