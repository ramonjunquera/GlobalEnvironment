/*
  Librería: RoJoList
  Autor: Ramón Junquera
  Fecha: 20181130
  Objetivo: Ejemplo de uso de librería de gestión de listas
  Material adicional: Cualquier placa
  Descripción:
    Utilizaremos la clase RoJoList con varios ejemplos complejos para aprender su uso.
    El tipo del valor de la lista será una estructura.
  Resultado:
    Se envían por el puerto serie los resultados de los ejercicios.
*/

#include "RoJoList.h" //Librería de gestión de diccionarios en memoria

//Definimos la escrutura de datos para el valor
struct numberStruct
{
  byte number;
  String numberString;
};

//Creamos el objeto de lista que utilizaremos
RoJoList<numberStruct> myList;

void list()
{
  //Muestra en pantalla el contenido de la lista

  //Puntero a valor del nodo
  numberStruct *pValue;

  //Mostramos el número de nodos
  Serial.println("# nodos="+String(myList.count())+" : free memory="+String(ESP.getFreeHeap()));
  //Recorremos todos los nodos de la lista
  for(uint16_t i=0;i<myList.count();i++)
  {
    //Obtenemos el puntero del valor de una posición de la lista
    myList.index(&pValue,i);
    //Mostramos la información obtenida
    Serial.println("index="+String(i)+" : number=" + String(pValue->number) + " : numberString='"+ pValue->numberString + "'");
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);
  Serial.println();

  //Listamos el contenido de la lista. Debería estar vacía
  list();
  
  //Añadimos un nodo
  //Creamos un puntero para el valor del nodo
  numberStruct *pValue;
  //Reservamos memoria
  pValue=new numberStruct;
  //Damos valores a los parámetros de la estructura
  pValue->number=33;
  pValue->numberString="treintaytres";
  //Añadimos el valor al principio de la lista 
  myList.add(pValue);
  Serial.println("\nAñadido nodo 33 al principio");
  //Mostramos el contenido de la lista. Sólo debería tener uno
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un segundo nodo al principio
  pValue=new numberStruct;
  pValue->number=24;
  pValue->numberString="veinticuatro";
  myList.add(pValue);
  Serial.println("\nAñadido nodo 24 al principio");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un tercer nodo al final
  pValue=new numberStruct;
  pValue->number=42;
  pValue->numberString="cuarentaydos";
  myList.add2end(pValue);
  Serial.println("\nAñadido nodo 42 al final");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un cuarto nodo en la posición 1
  pValue=new numberStruct;
  pValue->number=56;
  pValue->numberString="cincuentayseis";
  myList.add(pValue,1);
  Serial.println("\nAñadido nodo 56 en la posición 1");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Eliminamos el primer nodo
  myList.remove();
  Serial.println("\nEliminado el primer nodo");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Eliminamos el nodo 1
  myList.remove(1);
  Serial.println("\nEliminado el nodo 1");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Borramos todos los nodos
  myList.clear();
  Serial.println("\nBorrados todos los nodos");
  list();
}

void loop()
{
  //Nada especial que hacer aquí
}

