/*
  Librería: RoJoList
  Autor: Ramón Junquera
  Fecha: 20190705
  Objetivo: Ejemplo de uso de librería
  Material adicional: Cualquier placa
  Descripción:
    Utilizaremos la clase RoJoList con varios ejemplos simples para aprender su uso.
    El tipo del valor de la lista es String.
  Resultado:
    Se envían por el puerto serie los resultados de los ejercicios.
*/

#include <RoJoList.h> //Librería de gestión de listas

//Creamos el objeto de lista que utilizaremos
RoJoList<String> myList;

uint32_t freeMem()
{
  #ifdef ARDUINO_ARCH_AVR //Placas Arduino
    extern uint32_t __heap_start,*__brkval; 
    uint32_t v; 
    return (uint32_t)&v-(__brkval==0?(uint32_t)&__heap_start:(uint32_t)__brkval);
  #elif defined(ESP8266) || defined(ESP32) //Placas ESP
    return ESP.getFreeHeap();
  #elif defined(__arm__) //Placa RPi
    //En Linux no tiene sentido el cálculo de la RAM libre
    return 0;
  #else //Placa desconocida
    #error Unknown device
  #endif
}

//Muestra en pantalla el contenido de la lista
void list()
{
  //Puntero a valor del nodo
  String *pValue;

  //Mostramos el número de nodos
  Serial.println("# nodos="+String(myList.count())+" : free memory="+String(freeMem()));
  //Recorremos todos los nodos de la lista
  for(uint16_t i=0;i<myList.count();i++)
  {
    //Obtenemos el puntero del valor de una posición de la lista
    myList.index(&pValue,i);
    //Mostramos la información obtenida
    Serial.println("index="+String(i)+" : value='" + *pValue + "'");
  }
}

void setup()
{
  //Activamos la comunicación serie
  Serial.begin(115200);
  delay(3000);
  Serial.println();

  //Listamos el contenido de la lista. Debería estar vacía
  list();
  
  //Añadimos un nodo
  //Creamos un puntero de String para el valor
  String *pValue;
  //Reservamos memoria y la inicializamos con un texto
  pValue=new String("treintaytres");
  //Añadimos el valor al principio de la lista 
  myList.add(pValue);
  Serial.println("\nAñadido nodo 'treintaytres' al principio");
  //Mostramos el contenido de la lista. Sólo debería tener uno
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un segundo nodo al principio
  pValue=new String("veinticuatro");
  myList.add(pValue);
  Serial.println("\nAñadido nodo 'veinticuatro' al principio");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un tercer nodo al final
  pValue=new String("cuarentaydos");
  myList.add2end(pValue);
  Serial.println("\nAñadido nodo 'cuarentaydos' al final");
  list();

  //Esperamos un momento a que pueda terminar de enviar por puerto serie
  delay(10);
  //Añadimos un cuarto nodo en la posición 1
  pValue=new String("cincuentayseis");
  myList.add(pValue,1);
  Serial.println("\nAñadido nodo 'cincuentayseis' en la posición 1");
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

