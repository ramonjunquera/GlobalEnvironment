/*
  Autor: Ramón Junquera
  Fecha: 20200828
  Tema: Multitasking / FreeRTOS
  Objetivo: Balanceo de carga ente cores. Librería RoJoBalancer
  Material adicional: placa ESP32
  Descripción:
    Demostración de uso de librería RoJoBalancer.
    Se define una función que será la que se llame en distintas tareas (sayCore).
    Puntos obligatorios para las funciones que se llamen como tareas:
    - Definición: void functionName(void *params)
    - Terminan llamando al método deleteTask de la clase RoJoBalancer.
    
    Puesto que hemos introducido un delay en la función de tarea, no dará 
    tiempo a que finalice la primera tarea antes de crearlas todas.
    El resultado son 5 tareas asignadas a la CPU0 y 4 a CPU1.
    Esto es correcto, porque por defecto tenemos la tarea del programa
    principal corriendo en la CPU1.

  Nota:
    La función de tareas está preparada para recibir parámetros, pero en este
    ejemplo no los utilizaremos.

  Curiosidad:
    La tarea que se crea por defecto en la CPU1 para el programa principal
    también puede eliminarse!. Esto hace que jamás se llegue a ejecutar
    la función loop.
    No es un problema para FreeRTOS no tener tareas activas.

  Experimento:
    Si eliminamos el delay en la función de tarea y creamos las tareas
    con un delay suficiente como para darles tiempo a que se completen
    (por ejemplo 0.5s), siempre se asociarán a la CPU0, que es la que
    siempre queda libre tras finalizar la tarea anterior.
*/

#include <Arduino.h>
#include <RoJoBalancer.h> //Balanceo de carga de CPU

void sayCore(void *params) {
  delay(1000);
  Serial.printf("%li running in core %i\n",millis(),xPortGetCoreID());
  RoJoBalancer::deleteTask();
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie

  for(byte i=0;i<9;i++) {
    //Todas las tareas serán iguales:
    //- Llamarán a la función sayCore
    //- No tendrán parámetros
    //- El tamaño de pila se tomará por defecto
    RoJoBalancer::addTask(sayCore,NULL);
  }
  Serial.println("end main program");
  RoJoBalancer::deleteTask();
}

void loop() {
  //Nunca se ejecutará esta función
}