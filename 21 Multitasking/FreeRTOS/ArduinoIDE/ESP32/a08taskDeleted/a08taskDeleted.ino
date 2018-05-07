
/*
  Autor: Ramón Junquera
  Fecha: 07/05/2018
  Tema: Multitasking / FreeRTOS
  Objetivo: Destección de eliminación de tareas
  Material adicional: placa ESP32
  Descripción:

  Cuando creamos una tarea con xTaskCreatePinnedToCore entregamos como último 
  parámetro el puntero de una variable de tipo TaskHandle_t.
  A este puntero se le asigna la dirección del objeto que gestiona la tarea.
  Cuando la tarea es eliminada con el comando vTaskDelete, se elimina el
  objeto de la tarea, pero no se hace nada con los punteros que puedan existir
  apuntando a ese objeto.
  Por lo tanto, si tenemos anotada la dirección del objeto de la tarea, y lo
  borramos, el puntero seguirá apuntando al mismo lugar. Con el inconveniente
  de que ahora esa región de memoria no guardará ninguna tarea.

  Si queremos utilizar nuestro puntero para saber si la tarea ha sido
  eliminada, tendremos que preocuparnos nosotros mismos de volverlo nulo
  momentos antes de eliminar la tarea.

  En el ejemplo actual lanzaremos una tarea que muestr una secuencia de números
  y al terminar, anota como nulo el puntero (definido de manera global) y a continuación
  elimina la tarea actual.
  Gracias a ello, desde el programa principal detectaremos cuándo ha terminado la tarea
  y se ha eliminado, comprobando si el puntero es nulo.
*/

#include <Arduino.h>

//Definimos una tarea para poder lanzar el proceso
TaskHandle_t myTask;

void count5(void *parameter)
{
  //Cuenta hasta 5 y finaliza
  Serial.print("start function.");
  for(byte c=1;c<=5;c++)
  {
    Serial.print(" "+String(c));
    delay(1000);
  }
  Serial.print(".end function.");
  //Antes de que la tarea se borre a sí misma, pondremos la variable
  //que almacena el objeto a NULL para identificar que la tarea se ha borrado
  myTask=NULL;
  //Ahora borramos la tarea actual
  vTaskDelete(NULL);
}

void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);
}

void loop()
{
  //Creamos y lanzamos la tarea contra el core 0
  xTaskCreatePinnedToCore(count5,"task1",1000,NULL,1,&myTask,0);
  //Mientras la variable del objeto de la tarea tenga algo...
  while(myTask)
  {
    delay(100);
  }
  //La tarea ya no existe
  Serial.println("task has been deleted.");
  //Esperamos un momento antes de comenzar de nuevo
  delay(2000);
}
