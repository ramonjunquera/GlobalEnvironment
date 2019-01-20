/*
  Autor: Ramón Junquera
  Fecha: 02/05/2018
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
  Vemos que el proceso ejecutado en el core 0 tarda aprosimadamente la mitad de tiempo
  que el que se ejecuta en el core 1.
  La razón es que realmente tenemos 3 procesos en nuestro programa.
  Los dos que nosostros generamos y la ejecución de nuestro programa.
  Ya sabemos que un programa se ejecuta por defecto en el core 1. Por lo tanto, el
  core 1 está ejecutando 2 procesos. El de cálculo y nuestro programa.
  Por eso el proceso del core 1 tarda el doble. Porque tiene que atender a dos 
  procesos distintos.

  Conclusión:
  El sistema operativo FreeRTOS, no sólo limita la ejecución de un proceso por core.
  Se pueden lanzar varios. La limitación es que el tiempo de proceso se repartirá
  entre los distintos procesos asignados al core.
*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;
//Definimos las variables globales
float f0,f1;

//Funciones de cálculo
void calculation0()
{
  for(uint32_t a=0;a<999999;a++)
    f0=3.0*a;        
}
void calculation1()
{
  for(uint32_t a=0;a<999999;a++)
    f1=3.0*a;
}

//Funciones de test
void myFunction0(void *parameter)
{
  //Creamos un bucle infinito
  while(1)
  {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation0();
    //Mostramos el core y el tiempo invertido
    Serial.println("Core"+String(xPortGetCoreID())+": "+String(millis()-now));
    //Esperamos
    delay(1000);
  }
}
void myFunction1(void *parameter)
{
  //Creamos un bucle infinito
  while(1)
  {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation1();
    //Mostramos el core y el tiempo invertido
    Serial.println("Core"+String(xPortGetCoreID())+": "+String(millis()-now));
    //Esperamos
    delay(1000);
  }
}

void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);

  //Asignamos los parámetros a los objetos de las tareas y las lanzamos
  xTaskCreatePinnedToCore(myFunction0,"task0",1000,NULL,1,&myTask0,0);
  xTaskCreatePinnedToCore(myFunction1,"task1",1000,NULL,1,&myTask1,1);
}

void loop()
{
  //Nada especial que hacer aquí
}
