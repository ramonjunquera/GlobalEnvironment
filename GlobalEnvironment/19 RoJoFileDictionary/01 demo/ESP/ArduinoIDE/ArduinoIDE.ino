/*
  Autor: Ramón Junquera
  Fecha: 20180314
  Tema: Librería RoJoFileDictionary
  Objetivo: Demostración de uso
  Material: placa ESP
  Descripción:
    Diversos tests comentados.
*/
#include <Arduino.h>
#include "RoJoFileDictionary.h" //Librería de gestión de diccionarios en archivo

//Objeto de gestión de diccionario
RoJoFileDictionary myDicc;

void list()
{
  //Muestra en pantalla el contenido del diccionario
  
  RoJoFileDictionaryItem item;
  for(uint16_t i=0;i<myDicc.count();i++)
  {
    item=myDicc.item(i);
    Serial.println(String(i)+" = '"+ item.key + "' : '" + item.value + "'");
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);
  //Inicializamos el diccionario
  myDicc.begin("/mydicc.txt");
  //Comenzamos vaciando cualquier información contenida
  myDicc.clear();
  //Mostramos el número de registros contenidos
  Serial.println("# registros="+String(myDicc.count()));
  //Añadimos un registro dando los valores
  myDicc.add("primero","el uno");
  //Añadimos otro registro dando el item
  RoJoFileDictionaryItem item;
  item.key="segundo";
  item.value="el dos";
  myDicc.add(item);
  Serial.println("Añadidos dos registros");
  //Mostramos el número de registros contenidos
  Serial.println("# registros="+String(myDicc.count()));
  //Mostramos el contenido
  list();
  //Valor del índice 0
  Serial.println("Valor del índice 0 = '" + myDicc.value(0) + "'");
  //Clave del índice 1
  Serial.println("Clave del índice 1 = '" + myDicc.key(1) + "'");
  //Añadimos un registro con clave existente
  myDicc.add("primero","el nuevo uno");
  Serial.println("Añadido registro con clave existente");
  //El valor de habrá actualizado
  //Mostramos el contenido
  list();
  //Preguntamos si contiene una clave existente
  Serial.print("Existe la clave 'segundo'? ");
  if(myDicc.containsKey("segundo")) Serial.println("SI");
  else Serial.println("NO");
  //Preguntamos si contiene una clave inexistente
  Serial.print("Existe la clave 'tercero'? ");
  if(myDicc.containsKey("tercero")) Serial.println("SI");
  else Serial.println("NO");
  //Devuelve el valor de una clave existente
  Serial.println("Valor de la clave 'primero' = '" + myDicc.value("primero","no existe") + "'");
  //Devuelve el valor de una clave inexistente
  Serial.println("Valor de la clave 'cuarto' = '" + myDicc.value("cuarto","no existe") + "'");
  //Eliminamos una clave existente
  myDicc.remove("segundo");
  Serial.println("Eliminada clave 'segundo'");
  list();
  //Eliminamos una clave inexistente
  myDicc.remove("tercero");
  Serial.println("Eliminada clave 'tercero'");
  list();
  //Añadimos un nuevo valor
  myDicc.add("tercero","el tres");
  Serial.println("Añadido nuevo registro");
  list();
}

void loop()
{
  //Nada especial que hacer aquí
}
