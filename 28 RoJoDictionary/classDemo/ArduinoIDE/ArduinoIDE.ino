/*
  Librería: RoJoDictionary
  Autor: Ramón Junquera
  Fecha: 20181016
  Objetivo: Ejemplo de uso de librería
  Material adicional: Cualquier placa
  Descripción:
    Utilizaremos la clase RoJoDictionary con varios ejemplos para aprender su uso.
    Para hacerlo más sencillo, en este ejemplo los valores serán String.
  Resultado:
    Se envían por el puerto serie los resultados de los ejercicios.
*/

#include "RoJoDictionary.h" //Librería de gestión de diccionarios en memoria

RoJoDictionary<String> myDicc;

void list()
{
  //Muestra en pantalla el contenido del diccionario

  //Puntero a valor del item
  String *pValue;
  //Clave
  byte key;

  //Mostramos el número de items
  Serial.println("# items="+String(myDicc.count()));
  //Recorremos todos los items del diccionario
  for(byte i=0;i<myDicc.count();i++)
  {
    //Obtenemos los datos de una posición del diccionario: la clave y el puntero al valor
    myDicc.index(i,&key,&pValue);
    //Mostramos la información obtenida
    Serial.println("index="+String(i)+" : key="+ String(key) + " : value='" + (*pValue) + "'");
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);

  //Listamos el contenido del diccionario. Debería estar vacío
  list();
  //Añadimos un registro dando los valores
  //Creamos un puntero de String
  String *myTxt;
  //Reservamos memoria y la inicializamos con un texto
  myTxt=new String("treintaytres");
  //Añadimos la clave y el valor al diccionario
  myDicc.add(33,myTxt);
  //Mostramos la lista de items en el diccionario. Sólo debería tener uno
  list();

  //Creamos dos nuevos items
  myTxt=new String("veintinueve");
  myDicc.add(29,myTxt);
  myTxt=new String("dieciocho");
  myDicc.add(18,myTxt);
  //Mostramos la lista de items en el diccionario. Deberían ser 3
  list();

  //Quitamos el item con key=29 (el del medio)
  myDicc.remove(29);
  list();

  //Comprobamos si existe unos elementos
  Serial.println("Existe elemento 18? : "+String(myDicc.containsKey(18)?"SI":"NO"));
  Serial.println("Existe elemento 17? : "+String(myDicc.containsKey(17)?"SI":"NO"));

  //Actualizamos el valor de la clave 33
  myTxt=new String("treinta y tres");
  myDicc.add(33,myTxt);

  //Recuperamos el valor de un item
  //Aprovechamos el puntero de String
  myDicc.value(33,&myTxt);
  Serial.println("Valor de item 33 : '"+*myTxt+"'");

  //Borramos todos los items
  myDicc.clear();
  list();
}

void loop()
{
  //Nada especial que hacer aquí
}

