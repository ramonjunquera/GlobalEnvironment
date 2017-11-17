/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Detectar el problema
  Material adicional: placa Arduino UNO/Nano

  Descripción:
  El programa que utilizamos como ejemplo declara un array de bytes de 20 elementos
  tanto desde el programa principal como desde un archivo externo.
  Ambos contienen el comando PROGMEM.

  Cuando lo compilamos obtenemos los siguientes datos:
    Memoria de programa; 2124 bytes
    Memoria de datos   :  284 bytes

  Si duplicamos el tamaño de array externo a 40 bytes, el compilador muestra:
    Memoria de programa; 2144 bytes
    Memoria de datos   :  284 bytes
  Vemos que los 20 bytes nuevos se han añadido a la memoria de programa

  Si recuperamos el programa original y esta vez ampliamos el array definido en el 
  programa principal, obtenemos:
    Memoria de programa; 2144 bytes
    Memoria de datos   :  304 bytes
  Los 20 bytes nuevos se han añadido tanto a la memoria de programa como a la de datos.
  Necesitamos el doble de memoria cuando definimos datos dentro del programa.

  Volvemos a recuperar el programa original y eliminamos el parámetro PROGMEM de la definición del
  array en el archivo externo.
    Memoria de programa; 2124 bytes
    Memoria de datos   :  304 bytes
  Vemos que ahora, además de guardarse en la memoria de programa, también se guarda en la memoria de datos.

  Una última prueba. Si recuperamos el original y esta vez eliminamos el parámetro PROGMEM de la
  definición del array en la función setup  y compilamos, obtenemos:
    Memoria de programa; 2124 bytes
    Memoria de datos   :  284 bytes
  Vemos que no hay variación. El parámetro PROGMEM no tiene efecto.

  Deducciones: 
  - Todos los datos definidos en un programa se almacenan, al menos, en la memoria de programa.
  - Los datos definidos en el programa principal no son afectadas por el parámetro PROGMEM. Por lo tanto
    se  almacenan en la memoria de programa y en la de datos.
  - Sólo las estructuras definidas en archivos externos son afectadas por el parámetro PROGMEM.

  Aparte de los resultados y deducciones obtenidas de la información del compilador, podemos ejecutar
  cada una de las variantes de los ejemplos y deducimos lo siguiente:
  - Los datos que tienen copia en la memoria de datos son accesibles
  - Los datos que sólo se almacenan en la memoria de programa NO son accesibles (no coinciden con los
    valores esperados).
  - La lectura de los datos no depende de la función desde la que se realice.

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
