/*
  Autor: Ramón Junquera
  Fecha: 20200830
  Tema: Multitasking / FreeRTOS
  Objetivo: Demostración de uso de una cola
  Material adicional: placa ESP32
  Descripción:
    El objetivo es calcular la suma de los 100 primeros números.
    Para ello utilizaremos simultáneamente las dos CPUs del ESP32.
    Lanzaremos dos tareas. La primera calculará la suma de los 50 primeros 
    números. La segunda los 50 restantes.
    Necesitaremos saber cuándo han terminado y el resultado obtenido.
    Esto lo conseguiremos mediante una cola.
    Puesto que el total es un entero, positivo y relativamente pequeño, la cola
    utilizará variables uint16_t.
    La función a la que llaman las tareas será la misma. Sólo cambiarán los
    parámetros.
    Puesto que necesitamos más de un parámetro, crearemos una estructura para
    agruparlos todos.
    - startValue : valor inicial a sumar
    - endValue : valor final a sumar
    - queue : cola en la que se informará del resultado
    
  Nota:
    Una vez lanzada la tarea, no sabemos cuándo serán leidos los parámetros, por
    lo tanto, deberían estar disponibles mientras la tarea no finalice.
    Por esto, creamos un puntero de estructura de parámetros y reservamos
    memoria para ello. Los completamos y entregamos el puntero en la creación de
    la tarea. Después nos desprecupamos de esta reserva.
    La razón es porque antes de que finalice la tarea, ella misma se encarga de
    liberar la memoria de los parámetros.
*/

#include <Arduino.h>

//Estructura de parámetro para función sum
struct sumParam {
  uint16_t startValue; //Valor inicial a sumar
  uint16_t endValue; //Valor final a sumar
  QueueHandle_t queue; //Cola en la que se informará del resultado
};

//Suma una secuencia de números
void sum(void *param) {
  sumParam *p=(sumParam*)param; //Sabemos que es una estructura sumParam
  uint16_t total=0; //Cramos e inicializamos variable para guardar el total
  //Recorremos todos los valores del ranindicado y los añadimos a total
  for(uint16_t i=p->startValue;i<=p->endValue;i++) total+=i;
  xQueueSend(p->queue,&total,portMAX_DELAY); //Guardamos el total en la cola indicada
  delete p; //Eliminamos la estructura de parámetros de la memoria
  vTaskDelete(NULL); //Eliminamos la tarea
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
}

void loop() {
  //Creamos una cola con un máximo de 2 nodos de tipo uint16_t
  QueueHandle_t q=xQueueCreate(2,sizeof(uint16_t));
  if(q==NULL) Serial.println("Error. No hay memoria para crear la cola.");
  else { //Cola creada correctamente
    //Creamos puntero a estructura de parámetros y reservamos memoria
    sumParam *p=new sumParam;
    //Rellenamos los valores de la estructura
    p->startValue=1;
    p->endValue=50;
    p->queue=q;
    //Creamos tarea para que sume los primeros 50 números en la CPU0 y entregue
    //el resultado a la cola.
    xTaskCreatePinnedToCore(sum,"",2000,(void*)p,1,NULL,0);
    //Crearemos otra tarea para que sume los segundos 50 números en la CPU1
    p=new sumParam;
    p->startValue=51;
    p->endValue=100;
    p->queue=q;
    xTaskCreatePinnedToCore(sum,"",2000,(void*)p,1,NULL,1);
    //Las tareas han sido lanzadas
    //Consultamos los resultados
    uint16_t total1,total2;
    xQueueReceive(q,&total1,portMAX_DELAY); //Recuperamos valor de primer resultado
    xQueueReceive(q,&total2,portMAX_DELAY); //Recuperamos valor de segundo resultado
    Serial.printf("%li : Total = %i\n",millis(),total1+total2);
    vQueueDelete(q); //Borramos la cola
  }
  delay(2000);
}
