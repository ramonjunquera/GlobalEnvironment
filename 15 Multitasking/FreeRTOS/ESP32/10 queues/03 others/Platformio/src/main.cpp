/*
  Autor: Ramón Junquera
  Fecha: 20200831
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de funcionalidad de varios comandos de colas
  Material adicional: placa ESP32
  Descripción:
    Secuencia de uso de varios comandos de colas
*/

#include <Arduino.h>

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  //Creamos una cola con un máximo de 10 nodos de tipo byte
  QueueHandle_t q=xQueueCreate(10,sizeof(byte));
  if(q==NULL) Serial.println("Error. No hay memoria para crear la cola.");
  else { //Cola creada correctamente
    byte a,b; //Respuestas
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    for(byte i=0;i<4;i++) xQueueSend(q,&i,portMAX_DELAY);
    Serial.println("Añadidos 4 mensajes FIFO");
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    xQueueReceive(q,&a,portMAX_DELAY);
    Serial.printf("Recuperado mensaje %i\n",a);
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    xQueuePeek(q,&a,portMAX_DELAY);
    Serial.printf("El siguiente mensaje es %i\n",a);
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    xQueueReceive(q,&a,portMAX_DELAY);
    Serial.printf("Recuperado mensaje %i\n",a);
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    xQueueReset(q);
    Serial.println("Cola reseteada");
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    for(byte i=0;i<6;i++) xQueueSendToFront(q,&i,portMAX_DELAY);
    Serial.println("Añadidos 6 mensajes LIFO");
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    a=33;
    xQueueSendToBack(q,&a,portMAX_DELAY); //Igual que xQueueSend
    Serial.println("Añadido mensaje 33 al final (FIFO)");
    Serial.print("Recuperando 7 mensajes:");
    for(byte i=0;i<7;i++) {
      xQueueReceive(q,&a,portMAX_DELAY);
      Serial.printf(" %i",a);
    }
    Serial.println();
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    vQueueDelete(q);
    Serial.println("Cola borrada");
    q=xQueueCreate(1,sizeof(byte));
    Serial.println("Creada cola de un elemento");
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    a=44;
    xQueueSend(q,&a,portMAX_DELAY);
    Serial.println("Añadido mensaje 44");
    a=uxQueueMessagesWaiting(q);
    b=uxQueueSpacesAvailable(q);
    Serial.printf("Mensajes pendientes=%i, libres=%i\n",a,b);
    xQueuePeek(q,&a,portMAX_DELAY);
    Serial.printf("El siguiente mensaje es %i\n",a);
    a=55;
    //Nota: xQueueOverwrite sólo funciona con colas de un sólo elemento
    //Escribe el mensaje con independencia del estado actual
    xQueueOverwrite(q,&a);
    Serial.println("Sobreescrito con mensaje 55");
    xQueuePeek(q,&a,portMAX_DELAY);
    Serial.printf("El siguiente mensaje es %i\n",a);
    vQueueDelete(q);
    Serial.println("Cola borrada");
  }
}

void loop() {
  //Nada especial que hacer aquí
}