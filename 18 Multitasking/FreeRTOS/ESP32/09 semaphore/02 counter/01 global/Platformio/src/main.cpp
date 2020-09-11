/*
  Autor: Ramón Junquera
  Fecha: 20200830
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso de semáforo de contador
  Material adicional: placa ESP32
  Descripción:
    Crearemos dos tareas simultáneas que se ejecuten en distintas CPUs.
    Utilizaremos un semáforo contador para saber cuándo han finalizado ambas.

    Creamos un objeto semáforo como variable general para que quede a disposición
    de todas las funciones.
    Cuando creamos un semáforo con xSemaphoreCreateCounting tenemos dos parámetros.
    El primero es el número máximo que puede alcancar el contador y el segundo
    el valor inicial del contador.
    Cuando incrementamos el valor del contador con xSemaphoreGive y ya tiene el 
    valor máximo, congelará la tarea sin consumir recursos, hasta que alguna
    otra tarea reduzca el contador con xSemaphoreTake.
    De igual manera, cuando queremos decrementar el contador con xSemaphoreTake,
    se puede indicar en el segundo de sus parámetros el tiempo máximo de espera
    para completar la petición en el caso de que el contador sea cero.
    portMAX_DELAY indica una espera infinita.

    Antes de finalizar la tarea, incrementa el semáforo.
    En el programa principal solicitamos decrementar el semáforo dos veces,
    una por tarea.

    Repetimos el proceso indefinidamente.
*/

#include <Arduino.h>

//Definimos las variables globales
float f;
//Creamos un semáforo con un contador máximo de 2 y con valor inicial de 0
SemaphoreHandle_t mySema=xSemaphoreCreateCounting(2,0);

//Función de cálculo
void calculation() {
  for(uint32_t a=0;a<999999;a++) f=3.0*a;        
}

//Función de tarea
void taskFunc(void *param) {
  uint32_t now=millis();
  calculation(); //Llamamos a la función de cálculo
  Serial.printf("Task in core %i: %li\n",xPortGetCoreID(),millis()-now);
  xSemaphoreGive(mySema); //Incrementamos el contador del semáforo
  vTaskDelete(NULL); //Eliminamos la tarea
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos dos tareas. Una en cada CPU.
  for(byte cpu=0;cpu<2;cpu++) {
    xTaskCreatePinnedToCore(taskFunc,"",2000,NULL,1,NULL,cpu);
  }
  //Decrementamos el contador del semáforo tantas veces como tareas hemos lanzado.
  //Gracias al parámetro portMAX_DELAY, xSemaphoreTake deja la tareas actual
  //(programa principal) en suspensión y sin consumir recursos mientras el
  //semáforo tenga valor cero.
  for(byte task=0;task<2;task++) {
    xSemaphoreTake(mySema,portMAX_DELAY);
  }
  Serial.println("tareas completadas!");
  delay(2000);
}
