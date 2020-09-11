/*
  Autor: Ramón Junquera
  Fecha: 20200825
  Tema: Multitasking / FreeRTOS
  Objetivo: Comprobar si dos tareas simultáneas en distintos cores tardan lo mismo
  Material adicional: placa ESP32
  Descripción:

  Crearemos una función que genere carga al procesador.
  Hará una simple operación con números flotantes, pero repetida muchas veces.
  El resultado lo escribirá en una variable global.
  
  Crearemos dos tareas asociadas a distintos cores.
  Lanzaremos las tareas para que ejecuten la función de cálculo.
  Cada función cronometrará el tiempo invertido en cálculo.
  
  Definimos funciones distintas y variables distintas para asegurarnos que no haya
  interferencia entre ellas, porque realmente hacen lo mismo.

  Resultado:
  Vemos que el proceso ejecutado en el core 0 tarda aproximadamente la mitad
  de tiempo que el que se ejecuta en el core 1.
  La razón es que realmente tenemos 3 procesos en nuestro programa.
  Los dos que nosotros generamos y la ejecución de nuestro programa.
  Todos ellos ellos corriendo con una prioridad de 1.
  Ya sabemos que un programa se ejecuta por defecto en el core 1. Por lo tanto, el
  core 1 está ejecutando 2 procesos. El de cálculo y nuestro programa.
  Por eso el proceso del core 1 tarda el doble. Porque tiene que atender a dos 
  procesos distintos.

  Conclusión:
  El sistema operativo FreeRTOS, no sólo limita la ejecución de un proceso
  por core.
  Se pueden lanzar varios. La limitación es que el tiempo de proceso se repartirá
  entre los distintos procesos asignados al core.

  Nota:
  La variable flotante será global para que quede fuera del ámbito de la función.
  Si definimos la variable dentro de la misma función nos aparecerá un warning
  indicando que le asignamos valor, pero nunca la utilizamos. Además, debido a
  la compilación inteligente, no se tendrá en cuenta, ni el uso de la variable,
  ni el bucle en el que se encuentra la asignación, porque es la única instrucción
  que tiene. Por lo tanto, el tiempo de ejecución de la función sería nulo.

  Importante:
  Por defecto, el programa principal se lanza en el core 1 con una prioridad de 1.

  Experimentación:
  Si lanzamos nuestros procesos con prioridad 2 comprobaremos que entonces tardan
  aproximadamente lo mismo. Y además se sigue ejecutando el programa principal.
  En multitarea preemtiva como FreeRTOS, se atenderá siempre a la tarea (o tareas)
  con mayor prioridad. Entonces, porqué se sigue ejecutando el programa principal
  que tiene prioridad 1, mientras hay tareas con prioridad 2?.
  La respuesta es que las tareas incluyen la instrucción delay, que cede el
  uso de la CPU a los demás durante un periodo de tiempo en el que queda hibernada.
  Si no tuviésemos delay, el programa principal nunca se ejecutaría, porque las
  tareas contiene un bucle infinito.

  Experimentación:
  Si asignamos a nuestras tareas prioridades superiores a 1 (a la prioridad del
  programa principal), no aumentaremos su velocidad, puesto que no hay otras
  tareas con mayor prioridad.

*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;
//Definimos las variables globales
float f0,f1;

//Funciones de cálculo
void calculation0() {
  for(uint32_t a=0;a<999999;a++) f0=3.0*a;        
}
void calculation1() {
  for(uint32_t a=0;a<999999;a++) f1=3.0*a;
}

//Funciones de test
void myFunction0(void *parameter) {
  //Creamos un bucle infinito
  while(1) {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation0();
    //Mostramos el core y el tiempo invertido
    Serial.printf("Task in core %i: %li\n",xPortGetCoreID(),millis()-now);
    //Esperamos
    delay(1000);
  }
}
void myFunction1(void *parameter) {
  //Creamos un bucle infinito
  while(1) {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation1();
    //Mostramos el core y el tiempo invertido
    Serial.printf("Task in core %i: %li\n",xPortGetCoreID(),millis()-now);
    //Esperamos
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie

  //Asignamos los parámetros a los objetos de las tareas y las lanzamos
  xTaskCreatePinnedToCore(myFunction0,"",2000,NULL,1,&myTask0,0);
  xTaskCreatePinnedToCore(myFunction1,"",2000,NULL,1,&myTask1,1);
}

void loop() {
  //Nada especial que hacer aquí
}