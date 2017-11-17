/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Detectar el problema
  Material adicional: placa ESP8266

  Descripción:
  Utilizamos como ejemplo la misma base que en el ejemplo de las placas Arduino.
  La primer diferencia la encontramos cuan descubrimos que es imposible compilar el
  comando PROGMEM cuando aparece dentro del programa principal.
  Por lo tanto trataremos estos ejemplos.
  
  El programa que utilizamos como ejemplo declara un array de bytes de 20 elementos
  en un archivo externo con el comando PROGMEM.

  Cuando lo compilamos obtenemos los siguientes datos:
    Memoria de programa; 224677 bytes
    Memoria de datos   :  31756 bytes

  Si duplicamos el tamaño de array externo a 40 bytes, el compilador muestra:
    Memoria de programa; 224709 bytes
    Memoria de datos   :  31756 bytes
  Vemos que sólo se incrementa la memoria de programa.

  Si recuperamos el programa original, pero ahora eliminamos el comando PROGMEM, tenemos:
    Memoria de programa; 224685 bytes
    Memoria de datos   :  31772 bytes
  Vemos que ahora, se eleva ligeramente la memoria de programa, y aumenta la memoria de datos.

  Deducciones: 
  - Todos los datos definidos en un programa se almacenan, al menos, en la memoria de programa.
  - Sólo los datos definidas en archivos externos son afectadas por el parámetro PROGMEM.

  Aparte de los resultados y deducciones obtenidas de la información del compilador, podemos ejecutar
  cada una de las variantes de los ejemplos y deducimos lo siguiente:
  - Cuando intentamos acceder a datos que se incuentran en memoria de programa el programa falla y
    produce un reset.
  - No hay problema en acceder a datos en memoria de datos

  Como coclusión podemos sacar que:
  Guardar los datos sólo en la memoria de programa es una excelente solución para mejorar la eficiencia
  de la memoria. El problema es que no podemos utilizar esos datos directamente.

  Resolveremos este problema en los siguientes ejemplos.
*/
#include <Arduino.h>
#include "ExternalData.h" //Definición de estructuras de datos en archivo externo

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("setup.External:");
  for(byte i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(externalArray[i]);
  }
  Serial.println();

  //Mostramos el contenido del array definido en un archivo externo a través de una función
  Serial.print("Function.External:");
  PrintArray(externalArray);
}

void loop()
{

}

void PrintArray(const byte *myData)
{
  //La función recibe el puntero de un array constante de bytes y muestra la dirección física del
  //puntero entre paréntesis y sus 20 primeros elementos
  //La siguiente declaración es equivalente: void PrintArray(const byte myData[])

  Serial.print("(");
  Serial.print((long)myData);
  Serial.print(")");
  
  for(byte i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(myData[i]);
  }
  Serial.println();
}
