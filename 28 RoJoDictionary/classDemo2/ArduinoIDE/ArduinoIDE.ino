/*
  Librería: RoJoDictionary
  Autor: Ramón Junquera
  Fecha: 20181017
  Objetivo: Ejemplo de uso de librería
  Material adicional: Cualquier placa
  Descripción:
    Utilizaremos la clase RoJoDictionary con varios ejemplos complejos para aprender su uso.
    El tipo de la clave será String y del valor una estructura.
  Resultado:
    Se envían por el puerto serie los resultados de los ejercicios.
*/

#include "RoJoDictionary.h" //Librería de gestión de diccionarios en memoria

//Definimos la escrutura de datos para el valor
struct persona
{
  byte edad;
  String empleo;
};

RoJoDictionary<String,persona> myDicc;

void list()
{
  //Muestra en pantalla el contenido del diccionario

  //Puntero a valor del item
  persona *pValue;
  //Clave
  String key;

  //Mostramos el número de items
  Serial.println("# items="+String(myDicc.count()));
  //Recorremos todos los items del diccionario
  for(uint16_t i=0;i<myDicc.count();i++)
  {
    //Obtenemos los datos de una posición del diccionario: la clave y el puntero al valor
    myDicc.index(i,&key,&pValue);
    //Mostramos la información obtenida
    Serial.println("index="+String(i)+" : key="+ key + " : edad=" + String(pValue->edad) + " : empleo=" + pValue->empleo);
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);

  //Listamos el contenido del diccionario. Debería estar vacío
  list();
  //Creamos la clave
  String myKey="Fernando";
  //Creamos una nueva estructura para el valor
  persona *pValue = new persona;
  //Asignamos valores a la estructura
  pValue->edad=33;
  pValue->empleo="tornero";
  //Añadimos la clave y el valor al diccionario
  myDicc.add(myKey,pValue);
  Serial.println("Añadido item Fernando");
  //Mostramos la lista de items en el diccionario. Sólo debería tener uno
  list();

  //Creamos dos nuevos items
  pValue = new persona;
  pValue->edad=29;
  pValue->empleo="fresador";
  myDicc.add("Alberto",pValue);
  Serial.println("Añadido item Alberto");
  pValue = new persona;
  pValue->edad=18;
  pValue->empleo="ayudante";
  myDicc.add("Juan",pValue);
  Serial.println("Añadido item Juan");
  //Mostramos la lista de items en el diccionario. Deberían ser 3
  list();

  //Quitamos el item con key="Alberto" (el del medio)
  myDicc.remove("Alberto");
  Serial.println("Eliminado item Alberto");
  list();

  //Comprobamos si existe unos elementos
  Serial.println("Existe elemento Juan? : "+String(myDicc.containsKey("Juan")?"SI":"NO"));
  Serial.println("Existe elemento Juana? : "+String(myDicc.containsKey("Juana")?"SI":"NO"));

  //Recuperamos el valor de un item
  myDicc.value("Fernando",&pValue);
  Serial.println("Valor de item Fernando : edad=" + String(pValue->edad) +" : empleo=" + pValue->empleo);

  //Actualizamos el valor de la clave Fernando
  //Podemos hacerlo de dos maneras
  //1. Recuperando su valor y actualizandolo
  //Ahora mismo lo tenemos en pValue. Lo actualizamos
  pValue->edad=61;
  Serial.println("Actualizada edad de Fernando a 61");
  list();
  //2. Creando una nueva estructura añadiendo un item con la misma clave, porque antes de añadir
  //un nuevo item, elimina cualquiera que tenga la misma clave.
  pValue = new persona;
  pValue->edad=60;
  pValue->empleo="calderero";
  myDicc.add("Fernando",pValue);
  Serial.println("Añadido/Actualizado item Fernando");
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

