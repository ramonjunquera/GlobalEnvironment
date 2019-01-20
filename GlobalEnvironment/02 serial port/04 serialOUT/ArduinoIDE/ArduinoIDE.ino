/*
  Autor: Ramón Junquera
  Tema: Puerto serie
  Objetivo: Cómo se envía información por el puerto serie hacia el ordenador
  Material adicional: cualquier placa
  Descripción:
  Cada segundo envía el tiempo que ha transcurrido desde que se inició el
  programa en milisegundos por el puerto serie.
  
  Resultado:
  En la consola del puerto serie se recibe contínuamente la información del
  tiempo transcurrido.

  Nota:
  La función F() no es obligatoria.
  Lo que hace es incluir el texto constante de su interior dentro del espacio de 
  memoria correspondiente al programa y no la memoria de variables.
  Puesto que la memoria de programas es mucho mayor que la de variables, y en
  estos dispositivos no hay demasiada, es muy conveniente usar esta función para
  liberar memoria de variables.
*/  

#include <Arduino.h>

void setup()
{
  //Inicializamos el puerto serie para que transmita a 115200 baudios
  Serial.begin(115200);
}

void loop()
{
  Serial.print(F("Han pasado: "));
  Serial.print(millis());
  Serial.println(F(" milisegundos"));
  //Esperamos un segundo
  delay(1000);
}


