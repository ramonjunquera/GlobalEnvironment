/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Paso de parámetros a una tarea
  Material adicional: placa ESP32
  Descripción:
    Una función llamada como tarea tiene un sólo parámetro de tipo fijo:
      void *parameter
    Sólo admite un parámetro que es un puntero de tipo void.
    Esto significa que debemos conocer con antelación el tipo real del
    parámetro que recibirá. Así podremos convertirlo al tipo correcto
    y utilizarlo.
    El puntero de parámetro es a una variable, estructura o clase.
    Al ser variable tenemos que tener cuidado con un detalle: su valor
    puede variar.
    Si creamos una tarea basada en una función que apunta a una variable
    y a continuación cambiamos el valor de la variable, es posible que
    a la tarea no le haya dado tiempo a consultar el parámetro inicial
    y para cuando lo lea, tome el último valor.
    Esto ocurre si en el ejemplo actual, utilizamos la variable del bucle
    directamente como parámetro. Creamos la tarea y a continuación cambiamos
    el valor de la variable del bucle. Seguramente la tarea no ha tenido
    tiempo suficiente para leer el parámetro y tomará un valor equivocado.

    Para evitar este problema, en el ejercicio actual hemos creado un array
    global que mantendrá todos los valores de los parámetros de las
    tareas que creemos. Estos valores no se modificarán con el tiempo y no
    correremos el riesgo de que la tarea los tome equivocados.
    El array lo llenamos momentos antes de crear la tarea.

    El array es global y está disponible para todas las funciones del programa.
    Esto es así porque si lo declaramos en la función setup, cuando finalice,
    dejará de estar disponible y corremos el riesgo que la tarea creada no
    haya tenido tienpo de tomar sus valores.

    Todas las tareas se ejecutan con la misma prioridad. Eso no quiere decir que
    obligatoriamente se ejecuten en la misma secuencia en la que se crearon.
    Es el sistema operativo el que gestiona esto.
    Por esta razón podremos ver que las tareas no coinciden con su orden de creación.
*/

#include <Arduino.h>

//Array con los parámetros de cada tarea
byte param[10];

void sayParam(void *parameter) {
  //Función ejecutada como tarea que muestra el valor del parámetro
  //Suponemos que el tipo de parámetro es un byte

  //Anotamos el valor del parámetro
  //Primero indicamos convertimos parameter de void* a byte*
  //y después tomamos su valor.
  byte value=*((byte*)parameter);
  //Mostramos el valor del parámetro
  Serial.printf("%i\n",value);
  //Borramos la tarea
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  //Lanzaremos 10 tareas
  //A cada una de ellas se le pasará un valor distinto como parámetro
  for(byte t=0;t<10;t++) {
    //Guardamos el valor del parámetro
    param[t]=t;
    //Lanzamos la tarea y comprobamos si todo ha ido bien
    //- sayParam: nombre de la función
    //- "": nombre de la tarea (no importa)
    //- 2000: tamaño de pila
    //- (void*)&param[t]: puntero a parámetro
    //- 1: prioridad baja
    //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
    //- 0: core en el que se ejecuta
    if(xTaskCreatePinnedToCore(sayParam,"",2000,(void*)&param[t],1,NULL,0)!=pdPASS) Serial.println("Error al crear la tarea");
  }
}

void loop() {
  //Nada especial que hacer aquí
}