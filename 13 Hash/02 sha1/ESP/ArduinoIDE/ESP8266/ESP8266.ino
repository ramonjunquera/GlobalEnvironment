/*
  Autor: Ramón Junquera
  Tema: Cálculo de hash
  Objetivo: Calcular hash sha1
  Material adicional: placa ESP8266
  Descripción:
  Ejemplo sencillo de cómo utilizar la librería Hash.h para el cálculo de
  hash sha1.
  Mostramos varios ejemplos de de hash conocidos y a continuación los calculados.

  Resultado:
  Comprobamos que los hash calculados coinciden con los conocidos.
 */

#include <Arduino.h>
#include <Hash.h> //Librería con la clase de generación de hash sha1

void setup()
{
  Serial.begin(115200);

  Serial.println("MD5 example\n");

  Serial.println("sha1 (\"foo\") = 0beec7b5ea3f0fdbc95d0dd47f3c5bc275da8a33 (known)");
  Serial.print("sha1 (\"foo\") = ");
  Serial.print(sha1("foo"));
  Serial.println(" (calculated)\n");

  Serial.println("sha1 (\"bar\") = 62cdb7020ff920e5aa642c3d4066950dd1f01f4d (known)");
  Serial.print("sha1 (\"bar\") = ");
  Serial.print(sha1("bar"));
  Serial.println(" (calculated)\n");

  Serial.println("sha1 (\"wemos d1 mini\") = f496d031f900576e273206fb6cc755f2c9843419 (known)");
  Serial.print("sha1 (\"wemos d1 mini\") = ");
  Serial.print(sha1("wemos d1 mini"));
  Serial.println(" (calculated)\n");
}

void loop()
{
  //Nada especial que hacer aquí
}


