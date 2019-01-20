/*
  Autor: Ramón Junquera
  Tema: Puerto serie
  Objetivo: Cómo gestionar cadenas con el puerto serie
  Material adicional: cualquier placa
  Descripción:
  El texto rebidido lo guarda en una cadena y lo devuelve saludando
  
  Resultado:
  Saluda a cada nombre que recibe
*/ 

#include<Arduino.h>

void setup()
{
  //Inicializamos el puerto serie a 115200 baudios
  Serial.begin(115200);
  Serial.println(F("Preparado!"));
}

void loop()
{
  //Si hay caracteres en el buffer del serial...
  if(Serial.available())
  {
    //Anotamos en un string el texto recibido
    String nombre=Serial.readString();
    //Saludamos con el nombre
    Serial.print(F("Hola "));
    Serial.println(nombre);
  }
}



