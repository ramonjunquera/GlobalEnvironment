/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Lanzar dos tareas simultáneas
  Material adicional: placa ESP32

  Descripción:
  Basado en el ejercicio anterior.
  Crearemos dos tareas las lanzaremos una en cada core.

  Nota
  En este ejercicio utilizamos dos variables para guardar los objetos de
  tareas creadas.
  Puesto que las tareas nunca terminan, tampoco nos preocupa tener guardado
  su objeto para poder borrar la tarea en un futuro.
  Por lo tanto, podríamos olvidarnos de las variables y en la función de creación
  de tarea pasar un parámetro nulo en la posición del puntero de tarea.
  La tarea se creará igual, pero no tendremos manera de gestionarla.
  Ej: xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,NULL,0)

  Nota
  En teoría, el nombre del identificador que se le asigna a una tarea debe
  ser único. A través de él se podría obtener el puntero del objeto tarea.
  En la práctica la versión de FreeRTOS para ESP32 no incluye la función para
  obtener punteros en base al identificador. Por lo tanto, el identificador
  no nos es útil.
  Además no hay ningún tipo de restricción ni error si asignamos a dos tareas
  distintas el mismo identificador.

  Nota:
  No es un problema asignar a más de una tarea la misma prioridad.
  Así se hace en este ejemplo.

  Nota:
  Asignamos un tamaño de stack para cada tarea de 2000.
  Con tamaños más pequeños (1000) corremos el riesgo de que la tarea no tenga
  suficiente espacio para ejecutarse. Falle. No responda. Y el proceso de
  WatchDog que comprueba que las tareas están vivas, resetee el dispositivo
  con un error de tg1wdt_sys_reset
  
  Sugerencia: para ahorrar algo de memoria siempre asignaremos un nombre de
  longitud nula ("") para todas las tareas.
*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;

void sayCore(void *parameter) {
  while(1) {
    Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  //Asignamos los parámetros a los objetos de las tareas y las lanzamos
  if(!xTaskCreatePinnedToCore(sayCore,"",2000,NULL,1,&myTask0,0)) Serial.println("Error creando task0");
  if(!xTaskCreatePinnedToCore(sayCore,"",2000,NULL,1,&myTask1,1)) Serial.println("Error creando task1");
}

void loop() {
  //Nada especial que hacer aquí
}