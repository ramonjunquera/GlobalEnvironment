/*
  Autor: Ramón Junquera
  Fecha: 20200830
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso simple de una cola
  Material adicional: placa ESP32
  Descripción:
    Simplemente crearemos una cola que almacene bytes con una capacidad
    máxima de 10 elementos.
    Llenaremos la cola añadiendo 10 nodos.
    Utilizaremos el contador del bucle como dato para el nodo.
    Después, con otro bucle igual, recuperaremos los 10 elementos y los mostraremos
  Conclusiones:
  - El orden de recuperación es el mismo que el de inserción (FIFO).
  - La variable del bucle de llenado cambia a cada ciclo y nosotros hemos pasado
    el puntero de la variable, pero xQueueSend ha COPIADO el contenido de la
    variable al nodo, porque previamente le hemos indicado el tamaño de un nodo.
    Por lo tanto, aunque la variable cambia en cada ciclo, el nodo no se ve
    afectado. 
*/

#include <Arduino.h>

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos una cola con un máximo de 10 nodos de tipo byte
  QueueHandle_t q=xQueueCreate(10,sizeof(byte));
  if(q==NULL) Serial.println("Error. No hay memoria para crear la cola.");
  else { //Cola creada correctamente
    for(byte i=0;i<10;i++) {
      xQueueSend(q,&i,portMAX_DELAY);
    }
    byte res; //Respuesta
    Serial.print(millis());
    for(byte i=0;i<10;i++) {
      xQueueReceive(q,&res,portMAX_DELAY);
      Serial.printf(" %i",res);
    }
    Serial.println();
    vQueueDelete(q); //Borramos la cola
  }
  delay(2000);
}