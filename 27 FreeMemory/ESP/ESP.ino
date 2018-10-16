/*
  Autor: Ramón Junquera
  Versión: 20181007
  Tema: Memoria libre
  Objetivo: Obtener la memoria disponible
  Material: placa ESP
  Descripción:
    La función ESP.getFreeHeap() devuelve la cantidad de memoria disponible en bytes.
    Hay que recordar que las variables declaradas en un programa reservan su memoria antes 
    de iniciar la ejecución del programa. Por eso no disminuyen la memoria disponible
    cuando se utilizan.
    Sólo las variables dinámicas (listas, etc) reservan una cantidad variable de memoria.
    Principalmente lo hacemos con punteros. Se declara uno y después se reserva memoria
    contra ese puntero.
    La memoria ocupada no coincide exactamente con la memoria reservada.
    La razón es:
    - Se necesitan 4 bytes que guardan el tamaño de memoria reservada
    - La memoria se reserva en bloques de 8 bytes
    Por lo tanto, la reserva de un byte consume 8 bytes: 4 de tamaño + 1 reservado = 5, pero
    como la reserva mínima es 8, la memoria consumida es 8 bytes.
    Si reservamos 4 bytes, consumiremos lo mismo que reservando 1: 4 de tamaño + 4 reservados = 8
    Si reservamos 12 bytes, consumiremos 16 bytes: 4 de tamaño + 12 reservados = 16 bytes.
    Esto ocurre con cualquier tipo de variable.
    Si reservamos un uint16_t, consumiremos 8 bytes: 4 de tamaño + 2 reservados = 6, pero
    como la reserva mínima es 8, la memoria consumida es 8 bytes.
    Un ejemplo más...
    Si reservamos 11 uint16_t consumiremos: 4 de tamaño + 2*11 reservados = 26 bytes, pero
    26 no es divisible por 8. El siguiente sivisible por 8 es 4*8=32 bytes.

    El programa hace distintas reservas de memoria y muestra la memoria reservada y la consumida real.
*/
#include <Arduino.h>

void printFreeMem()
{
  //Muestra la cantidad de memoria disponible
  Serial.println("FreeMem="+String(ESP.getFreeHeap()));
}

void setup()
{
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  //Anotamos la memoria libre inicial
  uint32_t mem0=ESP.getFreeHeap();
  //La mostramos
  Serial.println("Memoria libre inicial="+String(mem0)+" bytes");
  
  //Declaramos y asignamos valor a una variable.
  //Esto no reduce la cantidad de memoria disponible porque ya se había reservado memoria
  //antes de iniciar la ejecución del programa
  Serial.println("Declaramos y asignamos valor a variable byte");
  byte a=4;
  //Anotamos la memoria disponible actual
  uint32_t mem1=ESP.getFreeHeap();
  //Mostramos la memoria consumida
  Serial.println("Mem="+String(mem1)+",consumido="+String(mem0-mem1));
  
  //Declaramos un puntero a una dirección de memoria que contiene bytes
  byte *b;
  //Reservamos distintas cantidades de memoria
  for(byte i=1;i<=13;i++)
  {
    //Reservamos memoria
    b=new byte[i];
    //Anotamos la memoria disponible actual
    mem1=ESP.getFreeHeap();
    //Mostramos la memoria consumida
    Serial.println("Reservada memoria para "+String(i)+" bytes. Consumida="+String(mem0-mem1)+". Libre="+String(mem1));
    //Liberamos la memoria
    delete[] b;
  }
  //Anotamos la memoria disponible actual
  mem1=ESP.getFreeHeap();
  //Mostramos la memoria consumida
  Serial.println("Mem="+String(mem1)+",consumido="+String(mem0-mem1));

  //Declaramos un puntero a una dirección de memoria que contiene uint16_t
  uint16_t *c;
  //Reservamos distintas cantidades de memoria
  for(byte i=1;i<=7;i++)
  {
    //Reservamos memoria
    c=new uint16_t[i];
    //Anotamos la memoria disponible actual
    mem1=ESP.getFreeHeap();
    //Mostramos la memoria consumida
    Serial.println("Reservada memoria para "+String(i*2)+" bytes. Consumida="+String(mem0-mem1)+". Libre="+String(mem1));
    //Liberamos la memoria
    delete[] c;
  }
  //Anotamos la memoria disponible actual
  mem1=ESP.getFreeHeap();
  //Mostramos la memoria consumida
  Serial.println("Mem="+String(mem1)+",consumido="+String(mem0-mem1));
}

void loop()
{
  yield();
}
