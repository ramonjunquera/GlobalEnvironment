/*
  Autor: Ramón Junquera
  Fecha: 20201122
  Tema: Multitasking / FreeRTOS
  Objetivo: Balanceo de carga ente cores. Librería RoJoBalancer
  Material adicional: placa ESP32
  Descripción:
    Basado en el ejercicio anterior.
    La diferencia es que el parámetro de la función no es simple.
    Esta vez pasamos el parámetro de una estructura que nos permite agrupar
    más datos.
    - El identificador. Número entero consecutivo.
    - El tiempo de duración en milisegundos. Aleatorio entre 100 y 400ms.

    Crearemos las tareas con un intervalo de 200ms.
    El resultado es que en algunos casos, la tarea habrá tenido tiempo de
    finalizar antes de que se lance la siguiente.
    Por esta razón, deberían aparecer más tareas asignadas a la CPU0 que
    a la 1.
  Curiosidad:
    Lo último que hace el programa principal es crear la última de las tareas.
    Después borra su propia tarea. Pero al menos la última tarea creada (a veces
    dos, dependiendo de cuando dure), quedan vivas y finalizan a posteriori.
*/

#include <Arduino.h>
#include <RoJoBalancer.h> //Balanceo de carga de CPU

struct taskParam { //Definimos estructura que guarda el parámetro de la tarea
  byte id; //Identificador de la tarea
  uint16_t msDelay; //Tiempo de demora en milisegundos
};

void sayCore(void *params) {
  //Sabemos que el parámetro apunta a una estructura taskParam
  taskParam *p=(taskParam*)params;
  byte id=p->id; //Obtenemos identificador
  uint16_t msDelay=p->msDelay; //Obtenemos tiempo de demora
  delay(msDelay); //Esperamos el tiempo indicado
  Serial.printf("task %i, core %i, delay %i\n",id,xPortGetCoreID(),msDelay);
  delete p; //Antes de finalizar la tarea debemos eliminar el parámetro de la memoria
  RoJoBalancer::deleteTask(); //Indicamos que hemos finalizado la tarea
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie

  randomSeed(analogRead(A0)); //Inicializamos la semilla
  for(byte i=0;i<9;i++) {
    delay(200);
    //Creamos un puntero a una estructura de parámetro de tarea 
    //Y reservamos memoria para él
    taskParam *p=new taskParam;
    //Completamos los valores del parámetro
    p->id=i; //Identificador. Número consecutivo
    p->msDelay=random(100,400); //[100,400[
    //Creamos la tarea
    //- Siempre se llama a la función sayCore
    //- El parámetro es un puntero de una estructura taskParam, pero antes
    //  hacemos un cast a puntero void.
    //- Utilizaremos el tamaño de pila por defecto.
    RoJoBalancer::addTask(sayCore,(void*)p);
  }
  Serial.println("end main program");
  RoJoBalancer::deleteTask();
}

void loop() {
  //Nunca se ejecutará esta función
}
