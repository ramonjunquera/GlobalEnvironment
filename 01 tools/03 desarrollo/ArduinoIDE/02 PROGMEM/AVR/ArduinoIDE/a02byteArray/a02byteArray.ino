/*
  Autor: Ramón Junquera
  Tema: Definición de estructuras de datos en memoria de programa en archivos externos
  Objetivo: Leer datos de un array de bytes contenido en memoria de programa
  Material adicional: placa Arduino UNO/Nano

  Descripción:
  Ahora que sabemos que el problema queda reducido a los datos definidos en archivos externos con
  el parámetro PROGMEM, nos centraremos sólo en ese caso.

  Seguimos suponiendo que trabajamos con placas Arduino UNO o Nano que tienen muy poca memoria.

  Los 32 Kb de memoria total pueden ser direccionados con 15 bits.
  Con 16 bits podríamos direccionar hasta 64 Kb. Realmente esta es la configuración que se utiliza.
  Los punteros son unsigned int (uint16_t). Se le llama direccionamiento corto (address_short).

  La librería <avr/pgmspace.h> (Program Space Utilities) porporciona unas funciones que facilitan
  la copia de información entre memorias (de memoria de programa a memoria de datos)
  La información completa se puede consultar en:
    http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html

  Las funciones más importantes para leer informeción de la memoria de programa son:
    pgm_read_byte(address_short) Es la función utilizada en el ejemplo. Obtiene el valor de un byte
    pgm_read_dword(address_short) Obtiene un uint16_t (unsigned int) (2 bytes)
    pgm_read_float(address_short) Obtiene un float
    pgm_read_ptr(address_short) Obtiene un puntero

  Si se pretende copiar más de un byte contiguo de la memoria de programa a la de datos, podemos utilizar la función
    memcpy_P(dirección destino,dirección origen,número de bytes)

  En este ejemplo, cada vez que necesitamos información de la estructura guardada en la memoria de programa
  utilizamos la función de lectura de byte.
  
  Para comprobar que funciona correctamente, primero intentamos hacer una lectura directa al array.
  El resultado no es correcto, porque el array apunta a la memoria de programa.
  Después repetimos lo mismo, pero utilizamos la función.

  Resultado:
  A través de la función podemos acceder a la estructura creada en la memoria del programa.
*/

#include "ExternalData.h" //Definición de estructuras de datos en archivo externo

void setup()
{
  //Abrimos comunicaciones serie para debug
  Serial.begin(115200);

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("Lectura directa:");
  for(int i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(externalArray[i]);
  }
  Serial.println();

  //Mostramos el contenido del array definido en un archivo externo
  Serial.print("Lectura por funcion:");
  for(int i=0;i<20;i++)
  {
    Serial.print(" ");
    Serial.print(pgm_read_byte_near(externalArray + i));
  }
  Serial.println();
}

void loop()
{

}
