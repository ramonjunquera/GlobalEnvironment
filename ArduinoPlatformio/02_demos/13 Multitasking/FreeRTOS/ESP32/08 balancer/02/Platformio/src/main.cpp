/*
  Autor: Ramón Junquera
  Fecha: 20200828
  Tema: Multitasking / FreeRTOS
  Objetivo: Balanceo de carga entre cores. Librería RoJoBalancer
  Material adicional: placa ESP32
  Descripción:
    Basado en el ejercicio anterior.
    La diferencia es que está vez lanzaremos las tareas haciendo uso del parámetro.
    El parámetro apuntará a un byte que contandrá el identificador de la tarea.
    Para el identificador utilizaremos la secuencia: 11,22,33,44,55,66,77,88,99.
  Resultado:
    Aunque no todas la tareas terminan en el orden en el que se han creado
    (es normal), sí podemos comprobar que se han ido asignando a las 2 CPUs
    alternatívamente.
*/

#include <Arduino.h>
#include <RoJoBalancer.h> //Balanceo de carga de CPU

void sayCore(void *params) {
  //Sabemos que el parámetro es el puntero de un byte
  byte *id=(byte*)params;
  delay(1000);
  Serial.printf("%i running in core %i\n",*id,xPortGetCoreID());
  //Para no dejar restos en memoria, antes de terminar, debemos preocuparnos
  //por borrar los parámetros
  delete id;
  RoJoBalancer::deleteTask(); //Indicamos que hemos terminado la tarea
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  for(byte i=0;i<9;i++) {
    //Creamos un puntero de byte y reservamos memoria para él
    //Contendrá el identificador de la tarea
    byte *id=new byte;
    *id=11*i+11; //Asignamos el identificador: 11,22,33, ... , 99
    //Creamos la tarea:
    //- Todas la tareas llamarán a la función sayCore
    //- Pasamos como parámetros el puntero del id, previo cast a puntero void
    //- Tomaremos el tamaño de pila por defecto
    RoJoBalancer::addTask(sayCore,(void*)id);
  }
  Serial.println("end main program");
  RoJoBalancer::deleteTask();
}

void loop() {
  //Nunca se ejecutará esta función
}
