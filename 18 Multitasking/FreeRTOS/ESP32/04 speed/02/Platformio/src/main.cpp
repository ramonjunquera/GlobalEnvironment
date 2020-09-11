/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Comprobar si dos tareas simultáneas en distintos cores tardan lo mismo
  Material adicional: placa ESP32
  Descripción:

  Simplificamos el ejercicio anterior.
  Utilizaremos una única variable global, una única función de carga y una 
  única función a llamar como proceso.
  
  Resultado:
  Comprobamos que los resultados, prácticamente, no varían.
  Los procesos no se interfieren.

  Experimento:
  Si asignamos las dos tareas al core 0, el tiempo de ejecución será similar
  para ambas y parecido al de la tarea que se ejecutaba en el core 1.
  La razón es que ahora tenemos dos tareas en el core 0 y antes las dos tareas
  (programa principal y tarea) estaban em el core 1.
*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;
//Definimos las variables globales
float f;

//Función de cálculo
void calculation() {
  for(uint32_t a=0;a<999999;a++) f=3.0*a;        
}

//Función de test
void myFunction(void *parameter) {
  //Creamos un bucle infinito
  while(1) {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation();
    //Mostramos el core y el tiempo invertido
    Serial.printf("Task in core %i: %li\n",xPortGetCoreID(),millis()-now);
    //Esperamos
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie

  //Asignamos los parámetros a los objetos de las tareas y las lanzamos
  xTaskCreatePinnedToCore(myFunction,"",2000,NULL,1,&myTask0,0);
  xTaskCreatePinnedToCore(myFunction,"",2000,NULL,1,&myTask1,1);
}

void loop() {
  //Nada especial que hacer aquí
}