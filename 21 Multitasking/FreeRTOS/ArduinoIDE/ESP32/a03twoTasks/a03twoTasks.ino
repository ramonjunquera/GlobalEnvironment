/*
  Autor: Ramón Junquera
  Fecha: 02/05/2018
  Tema: Multitasking / FreeRTOS
  Objetivo: Lanzar dos tareas simultáneas
  Material adicional: placa ESP32
  Descripción:

  Basado en el ejercicio anterior, crearemos dos objetos tarea y lanzaremos
  la ejecución de la función dos veces. Una en cada core.
*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;

void sayCore(void *parameter)
{
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
  //Asignamos los parámetros a los objetos de las tareas y las lanzamos
  xTaskCreatePinnedToCore(sayCore,"task0",1000,NULL,1,&myTask0,0);
  xTaskCreatePinnedToCore(sayCore,"task1",1000,NULL,1,&myTask1,1);
}

void loop()
{
  //Nada especial que hacer aquí
}
