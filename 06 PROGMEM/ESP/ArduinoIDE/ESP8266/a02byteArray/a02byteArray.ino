/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Leer datos de un array de bytes contenido en memoria de programa
  Material adicional: placa Arduino ESP8266

  Descripción:
  Tomamos como base el ejemplo de Arduino para intentar copiar datos de la memoria de programa a 
  la de datos para que sean legibles.
  Aunque el programa compila correctamente, decubrimos que su ejecución falla continuamente debido
  a que seguimos intentando acceder directamente a memoria de programa.

  Conclusión:
  En ESP8266 no podemos copiar datos de memoria de programa a la de datos, así que no podemos leerlos.
  No sirve de nada guardar datos de archivos externos en memoria de programa.

  No continuaremos con más ejemplos.
*/

#include "ExternalData.h" //Definición de estructuras de datos en archivo externo

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("Lectura directa:");
  for(int i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(externalArray[i]);
  }
  Serial.println();

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("Lectura por funcion:");
  for(int i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(pgm_read_byte_near(externalArray + i));
  }
  Serial.println();
}

void loop()
{

}
