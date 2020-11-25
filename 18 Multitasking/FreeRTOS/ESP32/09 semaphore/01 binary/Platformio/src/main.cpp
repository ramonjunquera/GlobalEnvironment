/*
  Autor: Ramón Junquera
  Fecha: 20200830
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso de semáforo binario
  Material adicional: placa ESP32
  Descripción:
    Crearemos dos tareas simultáneas, pero que deben ejecutarse de manera
    secuencial.
    Utilizaremos un semáforo binario para indicar la finalización de la
    primera tarea y que la segunda pueda comenzar.

    Creamos un objeto semáforo binario como variable global para que quede
    a disposición de todas las funciones.
    Creamos ambas tareas al mismo tiempo.
    La primera tarea comienza nada más ser creada.
    Antes de finalizar, pone el semáforo a 1.
    La primera instrucción de la segunda tarea es esperar a que el semáforo
    esté a 1 (queda hibernada sin consumir CPU).
    En cuanto se cumple la condición, se ejecuta y finaliza.
  Resultado:
    Aunque las tareas al mismo tiempo, se ejecutan secuencialmente.
*/

#include <Arduino.h>

//Creamos un semáforo binario
SemaphoreHandle_t mySema=xSemaphoreCreateBinary();

//Función de tarea 1
void task1(void *param) {
  Serial.println("Task 1 start");
  delay(1000);
  Serial.println("Task 1 end");
  xSemaphoreGive(mySema); //Incrementamos el contador del semáforo (a 1)
  vTaskDelete(NULL); //Eliminamos la tarea
}

//Función de tarea 2
void task2(void *param) {
  //Esperaremos hasta que el semáforo sea 1 = tarea 1 finalizada
  //y decrementamos su valor.
  //portMAX_DELAY indica que esperará de manera indefinida
  xSemaphoreTake(mySema,portMAX_DELAY);
  Serial.println("Task 2 start");
  delay(1000);
  Serial.println("Task 2 end");
  vTaskDelete(NULL); //Eliminamos la tarea
}


void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos dos tareas
  xTaskCreatePinnedToCore(task1,"",2000,NULL,1,NULL,0);
  xTaskCreatePinnedToCore(task2,"",2000,NULL,1,NULL,0);
  //Aunque las dos tareas se han creado al mismo tiempo, la primera se 
  //ejecutará, pero la segunda esperará a que la primera cambie el valor
  //del semáforo a 1 antes de comenzar.
  delay(3000); //Nos aseguramos que ambas tareas han finalizado
  Serial.println("Loop end");
  //Actualmente el semáforo está a 0 porque lo ha decrementado la segunda tarea.
  //Podemos repetir el proceso
}
