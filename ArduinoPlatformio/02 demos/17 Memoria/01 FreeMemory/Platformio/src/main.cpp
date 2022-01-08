/*
  Autor: Ramón Junquera
  Versión: 20190705
  Tema: Memoria libre
  Objetivo: Obtener la memoria disponible
  Material: Cualquier placa
  Descripción:
    Define la función freeMem que calcula la memoria libre disponible.
    Es compatible concualquier placa.
    El programa hace distintas reservas de memoria y muestra la memoria reservada y la consumida real.
*/
#include <Arduino.h>

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

void setup()
{
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  delay(3000);
  //Anotamos la memoria libre inicial
  uint32_t mem0=freeMem();
  //La mostramos
  Serial.println("Memoria libre inicial="+String(mem0)+" bytes");
  
  //Declaramos y asignamos valor a una variable.
  //Esto no reduce la cantidad de memoria disponible porque ya se había reservado memoria
  //antes de iniciar la ejecución del programa
  Serial.println("Declaramos y asignamos valor a variable byte");
  byte a=4;
  //Hacemos una operación simple para que no de el warning de variable no utilizada
  a*=2;
  //Anotamos la memoria disponible actual
  uint32_t mem1=freeMem();
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
    mem1=freeMem();
    //Mostramos la memoria consumida
    Serial.println("Reservada memoria para "+String(i)+" bytes. Consumida="+String(mem0-mem1)+". Libre="+String(mem1));
    //Liberamos la memoria
    delete[] b;
  }
  //Anotamos la memoria disponible actual
  mem1=freeMem();
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
    mem1=freeMem();
    //Mostramos la memoria consumida
    Serial.println("Reservada memoria para "+String(i*2)+" bytes. Consumida="+String(mem0-mem1)+". Libre="+String(mem1));
    //Liberamos la memoria
    delete[] c;
  }
  //Anotamos la memoria disponible actual
  mem1=freeMem();
  //Mostramos la memoria consumida
  Serial.println("Mem="+String(mem1)+",consumido="+String(mem0-mem1));
}

void loop()
{
  //Nada interesante que hacer aquí
  yield();
}