/*
  Autor: Ramón Junquera
  Fecha: 20220808
  Tema: Librería RoJoFileDictionary
  Objetivo: Demostración de uso
  Material: cualquier placa, lector SD (opcional)
  Descripción:
    Diversos tests comentados.
*/
#include <Arduino.h>
#include <RoJoFileDictionary.h> //Librería de gestión de diccionarios en archivo

//Objeto de gestión de diccionario
RoJoFileDictionary myDicc;

//Muestra en pantalla el contenido del diccionario
void list() {
  RoJoFileDictionaryItem item;
  Serial.println("list "+String(myDicc.count())+ " items:");
  for(uint16_t i=0;i<myDicc.count();i++) {
    item=myDicc.item(i);
    Serial.println("#"+String(i)+" = '"+ item.key + "' : '" + item.value + "'");
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  myDicc.begin("/mydicc.txt"); //Inicializamos el diccionario
  myDicc.clear(); //Comenzamos vaciando cualquier información contenida
  list(); //Mostramos el contenido (debería estar vacío)
  myDicc.add("primero","el uno"); //Añadimos un registro dando los valores
  RoJoFileDictionaryItem item; //Añadimos otro registro dando el item
  item.key="segundo";
  item.value="el dos";
  myDicc.add(item);
  Serial.println("Añadidos dos registros");
  list(); //Mostramos el contenido
  //Valor del índice 0
  Serial.println("Valor del índice 0 = '" + myDicc.value(0) + "'");
  //Clave del índice 1
  Serial.println("Clave del índice 1 = '" + myDicc.key(1) + "'");
  //Añadimos un registro con clave existente
  myDicc.add("primero","el nuevo uno");
  Serial.println("Añadido registro con clave existente");
  //El valor se habrá actualizado
  list(); //Mostramos el contenido
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

void loop() {
  //Nada especial que hacer aquí
}
