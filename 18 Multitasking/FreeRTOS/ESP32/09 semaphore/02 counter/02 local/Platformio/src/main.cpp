/*
  Autor: Ramón Junquera
  Fecha: 20200830
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso de semáforo de contador
  Material adicional: placa ESP32
  Descripción:
    Basado en el ejemplo anterior.
    Eliminamos la necesidad de declarar el semáforo de manera global.
    Ahora se declara en la función loop.
    Para que la tarea pueda acceder al semáforo, entregamos su puntero como
    parámetro.
    Antes de terminar la función loop, eliminamos el semáforo con vSemaphoreDelete
  Experimento:
    La función uxSemaphoreGetCount(mySema) devuelve el valor actual del 
    semáforo.
    Si mostramos el valor del semáforo, antes o después de hacer el xSemaphoreTake
    nos encontraremos que siempre es 0!!!
    La razón es que tras lanzar las tareas, nos ponemos a comprobar si alguna ha
    terminado. La respuesta es que no. Y esperamos a que termine la primera.
    Y justo después no hay ninguna otra tarea finalizada. Porque no terminan
    las dos en el mismo instante.
    Para poder comprobar que este comendo funciona, deberíamos esperar un
    tiempo después del lanzamiento de tareas que no asegure que ambas han
    terminado. Entonces sí veremos cómo el contador ha subido hasta 2 y tras
    decrementarlo, tenemos 1. Repitiendo de 1 a 0 para la segunda tarea.
*/

#include <Arduino.h>

//Definimos las variables globales
float f;

//Función de cálculo
void calculation() {
  for(uint32_t a=0;a<999999;a++) f=3.0*a;        
}

//Función de tarea
void taskFunc(void *param) {
  uint32_t now=millis();
  calculation(); //Llamamos a la función de cálculo
  Serial.printf("Task in core %i: %li\n",xPortGetCoreID(),millis()-now);
  //Sabemos que el parámetro de la función es un puntero de semáforo
  xSemaphoreGive((SemaphoreHandle_t)param); //Incrementamos el contador del semáforo
  vTaskDelete(NULL); //Eliminamos la tarea
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos un semáforo con un contador máximo de 2 y con valor inicial de 0
  SemaphoreHandle_t mySema=xSemaphoreCreateCounting(2,0);
  if(mySema==NULL) Serial.println("Error. Sin memoria para crear semáforo.");
  else { //Objeto semáforo creado correctamente
    for(byte cpu=0;cpu<2;cpu++) {
      //Pasamos a la función de la tarea como parámetro el puntero del semáforo
      xTaskCreatePinnedToCore(taskFunc,"",2000,(void*)mySema,1,NULL,cpu);
    }
    for(byte task=0;task<2;task++) {
      xSemaphoreTake(mySema,portMAX_DELAY);
    }
    Serial.println("tareas completadas!");
    vSemaphoreDelete(mySema); //Borramos el semáforo
  }
  delay(2000);
}