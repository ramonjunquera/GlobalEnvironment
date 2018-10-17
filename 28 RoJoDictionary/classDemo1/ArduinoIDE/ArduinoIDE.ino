/*
  Librería: RoJoDictionary
  Autor: Ramón Junquera
  Fecha: 20181017
  Objetivo: Ejemplo de uso de librería
  Material adicional: Cualquier placa
  Descripción:
    Utilizaremos la clase RoJoDictionary con varios ejemplos simples para aprender su uso.
    El tipo de la clave será byte y del valor String.
  Resultado:
    Se envían por el puerto serie los resultados de los ejercicios.
*/

#include "RoJoDictionary.h" //Librería de gestión de diccionarios en memoria

RoJoDictionary<byte,String> myDicc;

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
  for(uint16_t i=0;i<myDicc.count();i++)
  {
    //Obtenemos los datos de una posición del diccionario: la clave y el puntero al valor
    myDicc.index(i,&key,&pValue);
    //Mostramos la información obtenida
    Serial.println("index="+String(i)+" : key="+ String(key) + " : value='" + *pValue + "'");
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);

  //Listamos el contenido del diccionario. Debería estar vacío
  list();
  //Añadimos un item dando los valores
  //Creamos un puntero de String para el valor
  String *pValue;
  //Reservamos memoria y la inicializamos con un texto
  pValue=new String("treintaytres");
  //Añadimos la clave y el valor al diccionario
  myDicc.add(33,pValue);
  Serial.println("Añadido item 33");
  //Mostramos la lista de items en el diccionario. Sólo debería tener uno
  list();

  //Creamos dos nuevos items
  pValue=new String("veintinueve");
  myDicc.add(29,pValue);
  Serial.println("Añadido item 29");
  pValue=new String("dieciocho");
  myDicc.add(18,pValue);
  Serial.println("Añadido item 18");
  //Mostramos la lista de items en el diccionario. Deberían ser 3
  list();

  //Quitamos el item con key=29 (el del medio)
  myDicc.remove(29);
  Serial.println("Eliminado item 29");
  list();

  //Comprobamos si existe unos elementos
  Serial.println("Existe elemento 18? : "+String(myDicc.containsKey(18)?"SI":"NO"));
  Serial.println("Existe elemento 17? : "+String(myDicc.containsKey(17)?"SI":"NO"));

  //Recuperamos el valor de un item
  myDicc.value(33,&pValue);
  Serial.println("Valor de item 33 : '" + *pValue + "'");

  //Actualizamos el valor del item 33
  //Podemos hacerlo de dos maneras
  //1. Recuperando su valor y actualizandolo
  //Ahora mismo lo tenemos en pValue. Lo actualizamos
  *pValue="treinta y tres";
  Serial.println("Actualizado item 33");
  list();
  //2. Creando un nuevo valor y añadiéndolo con la misma clave, porque antes de añadir
  //un nuevo item, elimina cualquiera que tenga la misma clave.
  pValue=new String("treinta y 3");
  myDicc.add(33,pValue);
  Serial.println("Añadido/Actualizado item 33");
  list();  

  //Borramos todos los items
  myDicc.clear();
  Serial.println("Diccionario vaciado");
  list();
}

void loop()
{
  //Nada especial que hacer aquí
}

