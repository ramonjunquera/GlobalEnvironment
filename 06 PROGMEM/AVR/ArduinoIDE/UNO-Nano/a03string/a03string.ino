/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Mostrar strings almacenados en memoria de programa
  Material adicional: placa Arduino Uno/Nano

  Descripción:
  En este ejemplo guardaremos texto en la memoria de programa e intentaremos recuperarlo.
  Puesto que la longitud del texto inicialmente es indeterminada, no podemos utilizar
  las funciones pgm* como en ejemplos anteriores.

  Lo primero que debemos hacer es saber la longitud del texto.
  Recordemos que en C un texto tiene un tamaño de un byte por carácter y que siempre finaliza
  con un caracter 0. Por lo tanto necesitamos siempre un byte más de la longitud máxima del texto.

  Necesitaremos reservar memoria de datos con suficiente tamaño como para almacenar el texto.
  Por eso creamos la variable buffer.

  El tamaño del texto lo obtenemos con la función sizeof().
  A partir de aquí, utilizamos la función memcpy_P para copiar el contenido de la memoria de
  programa a la de datos.
*/

#include "ExternalData.h" //Definición de estructuras de datos en archivo externo

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Creamos la variable que contendrá el valor leido
  //Es importante que se lo suficientemente grande como para albergar todo el texto
  char buffer[100];
  //Copiamos al buffer el contenido de la memoria de programa correspondiente a la constante que queremos leer
  //Para no copiar más que lo necesario, calculamos el tamaño del string
  memcpy_P(buffer,pgmString,sizeof(pgmString));
  //Mostramos una etiqueta de texto
  //Con la función F obligamos a que el texto se almacene en memoria de programa y no en memoria de datos
  Serial.print(F("pgmString="));
  //Mostramos el valor del buffer
  Serial.println(buffer);
}

void loop()
{

}
