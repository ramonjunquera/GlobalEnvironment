/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Detectar el problema
  Material adicional: placa ESP32

  Descripción:
  El programa que utilizamos como ejemplo declara un array de bytes de 20 elementos
  tanto desde el programa principal como desde un archivo externo.
  Ambos contienen el comando PROGMEM.

  Cuando lo compilamos obtenemos los siguientes datos:
    Memoria de programa; 109092 bytes
    Memoria de datos   :   9612 bytes

  Si duplicamos el tamaño de array externo a 40 bytes, el compilador muestra:
    Memoria de programa; 109112 bytes
    Memoria de datos   :   9612 bytes
  Vemos que los 20 bytes nuevos se han añadido a la memoria de programa

  Si recuperamos el programa original y esta vez ampliamos el array definido en el 
  programa principal, obtenemos:
    Memoria de programa; 109108 bytes
    Memoria de datos   :   9612 bytes
  Aumenta sólo el tamaño de la memoria de programas.

  Volvemos a recuperar el programa original y eliminamos el parámetro PROGMEM de la definición del
  array en el archivo externo.
    Memoria de programa; 109092 bytes
    Memoria de datos   :   9612 bytes
  No hay variación respecto al programa original.

  Una última prueba. Si recuperamos el original y esta vez eliminamos el parámetro PROGMEM de la
  definición del array en la función setup  y compilamos, obtenemos:
    Memoria de programa; 109092 bytes
    Memoria de datos   :   9612 bytes
  No hay variación respecto al programa original.

  Deducciones: 
  - Los datos siempre se almacenan en memoria de programa. Nunca en memoria de datos.
  - No es necesario el uso de PROGMEM para que esto ocurra. Se hace por defecto.
  - No dónde estén definidos los datos.
  
  Aparte de los resultados y deducciones obtenidas de la información del compilador, podemos ejecutar
  cada una de las variantes de los ejemplos y deducimos lo siguiente:
  - El compilador acepta siempre el uso de PROGMEM, pero no tiene ningún efecto.
  - No se necesitan funciones especiales para acceder directamente a los datos guardados en momeria de programa.
  
  Como coclusión podemos sacar que:
  En ESP32 no es necesario el uso de PROGMEM porque los datos siempre se guardan en memoria de programa.

  No desarrollaremos más ejemplos.
*/
#include <Arduino.h>
#include "ExternalData.h" //Definición de estructuras de datos en archivo externo

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Definición de estructuras de datos en archivo interno
  const byte internalArray[] PROGMEM = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  //const byte internalArray[] PROGMEM = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};
  //const byte internalArray[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

  //Mostramos el contenido del array definido en local
  Serial.print("setup.Internal:");
  for(byte i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(internalArray[i]);
  }
  Serial.println();

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("setup.External:");
  for(byte i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(externalArray[i]);
  }
  Serial.println();

  //Mostramos el contenido del array definido en local a través de una función
  Serial.print("Function.Internal:");
  PrintArray(internalArray);
  
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
