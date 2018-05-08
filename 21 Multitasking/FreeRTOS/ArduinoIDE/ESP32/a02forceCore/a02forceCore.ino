/*
  Autor: Ramón Junquera
  Fecha: 08/05/2018
  Tema: Multitasking / FreeRTOS
  Objetivo: Forzar la ejecución de un proceso por un core
  Material adicional: placa ESP32
  Descripción:

  En el ejercicio anterior hemos comprobado como por defecto el
  programa se ejecuta en el core 1, dejando el 0 sin usar.

  Repetiremos el mismo ejercicio, pero esta vez forzaremos a que 
  ejecute por el core 0.

  La documentación oficial para la creación de tareas se encuentra en:
  https://www.freertos.org/a00125.html

  Nota
  El tipo BaseType_t es binario y sus valores son:
    errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY = 0 = Error. La tarea no se ha creado
                                   pdPASS = 1 = Ok. Tarea creada correctamente
*/

#include <Arduino.h>

//Definimos una tarea para poder lanzar el proceso
TaskHandle_t myTask;

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
  //Definimos la variable en la que guardaremos el resultado de la creación de la tarea
  BaseType_t answer;
  //Asignamos los parámetros al objeto tarea y lo lanzamos
  answer=xTaskCreatePinnedToCore
  (
    sayCore, //Nombre de la función que se lanza cuando se ejecuta esta tarea
    "task1", //Nombre de la tarea (identificador único)
    1000, //Tamaño de la pila (número de variables a almacenar)
    NULL, //Parámetros de la tarea
    1, //Prioridad. Cuanto más alta mayor prioridad
    &myTask, //Puntero del objeto tarea que lo mantendrá
    0  //Core en el que se ejecutará: 0 ó 1
  );
  //Comprobamos si se ha creado correctamente
  if(answer==pdPASS) Serial.println("La tarea se ha creado correctamente");
  else Serial.println("Error al crear la tarea");
}

void loop()
{
  //Nada especial que hacer aquí
}
