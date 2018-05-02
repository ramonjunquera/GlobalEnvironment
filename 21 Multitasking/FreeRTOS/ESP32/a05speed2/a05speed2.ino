/*
  Autor: Ramón Junquera
  Fecha: 02/05/2018
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

  Comprobación:
  Podemos comprobar fácilmente que si asignamos ambas tareas al core 0, el
  tiempo de ejecución será para ambos procesos el mismo y similar al que 
  aparece ahora para el core 1.
  La razón es que tendremos al core 0 con 2 procesos, de la misma manera que
  ahora tenemos el core 1 con 2 procesos.
*/

#include <Arduino.h>

//Definimos los objetos a los que asignaremos las tareas
TaskHandle_t myTask0,myTask1;
//Definimos las variables globales
float f;

//Función de cálculo
void calculation()
{
  for(uint32_t a=0;a<999999;a++)
    f=3.0*a;        
}

//Función de test
void myFunction(void *parameter)
{
  //Creamos un bucle infinito
  while(1)
  {
    //Anotamos el tiempo actual
    uint32_t now=millis();
    //Llamamos a la función de cálculo
    calculation();
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
  xTaskCreatePinnedToCore(myFunction,"task0",1000,NULL,1,&myTask0,0);
  xTaskCreatePinnedToCore(myFunction,"task1",1000,NULL,1,&myTask1,1);
}

void loop()
{
  //Nada especial que hacer aquí
}
