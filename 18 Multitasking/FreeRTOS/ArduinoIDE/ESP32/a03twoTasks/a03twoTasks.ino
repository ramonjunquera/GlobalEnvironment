/*
  Autor: Ramón Junquera
  Fecha: 20180528
  Tema: Multitasking / FreeRTOS
  Objetivo: Lanzar dos tareas simultáneas
  Material adicional: placa ESP32
  Descripción:

  Basado en el ejercicio anterior, crearemos dos objetos tarea y lanzaremos
  la ejecución de la función dos veces. Una en cada core.

  Nota
  En este ejercicio utilizamos dos variables para guardar los objetos de tareas creadas.
  Puesto que las tareas nunca terminan, tampoco nos preocupa tener guardado
  su objeto para poder borrar la tarea en un futuro.
  Por lo tanto, podríamos olvidarnos de las variables y en la función de creación
  de tarea pasar un parámetro nulo en la posición del puntero de tarea.
  La tarea se creará igual, pero no tendremos manera de gestionarla.
  Ej: xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,NULL,0)

  Nota
  En teoría, el nombre del identificador que se le asigna a una tarea debe ser único.
  Gracias a ello, podríamos obtener el puntero del objeto de la tarea gracias a él.
  En la práctica la versión de FreeRTOS que para ESP32 no incluye la función para
  obtener punteros en base al identificador. Por lo tanto, el identificador
  no nos es útil.
  Además no hay ningún tipo de restricción ni error si asignamos a dos tareas distintas
  el mismo identificador.

  Nota:
  No es un problema asignar a más de una tarea la misma prioridad.
  Así se hace en este ejemplo.
  
  Sugerencia: para ahorrar algo de memoria siempre asignaremos un nombre de longitud
  nula ("") para todas las tareas.
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
  if(!xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,&myTask0,0)) Serial.println("Error creando task0");
  if(!xTaskCreatePinnedToCore(sayCore,"",1000,NULL,1,&myTask1,1)) Serial.println("Error creando task1");
}

void loop()
{
  //Nada especial que hacer aquí
}
