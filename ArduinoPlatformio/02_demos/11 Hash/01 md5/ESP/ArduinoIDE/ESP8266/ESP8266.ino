/*
  Autor: Ramón Junquera
  Tema: Cálculo de hash
  Objetivo: Calcular hash md5
  Material adicional: placa ESP8266
  Descripción:
  Ejemplo sencillo de cómo utilizar la librería MD5Builder.h para el cálculo de
  hash md5.
  Mostramos varios ejemplos de de hash conocidos y a continuación los calculados.

  Resultado:
  Comprobamos que los hash calculados coinciden con los conocidos.
 */

#include <Arduino.h>
#include <MD5Builder.h> //Librería con la clase de generación de hash md5

//Creamos el objeto de gestión
MD5Builder _md5;

String md5(String str)
{
  //Dada una cadena, devuelve su md5
  _md5.begin();
  _md5.add(String(str));
  _md5.calculate();
  return _md5.toString();
}

void setup()
{
  Serial.begin(115200);

  Serial.println("MD5 example\n");

  Serial.println("MD5 (\"foo\") = acbd18db4cc2f85cedef654fccc4a4d8 (known)");
  Serial.print("MD5 (\"foo\") = ");
  Serial.print(md5("foo"));
  Serial.println(" (calculated)\n");

  Serial.println("MD5 (\"bar\") = 37b51d194a7513e45b56f6524f2d51f2 (known)");
  Serial.print("MD5 (\"bar\") = ");
  Serial.print(md5("bar"));
  Serial.println(" (calculated)\n");

  Serial.println("MD5 (\"wemos d1 mini\") = c4594198428b13c196b05161d3e9c86e (known)");
  Serial.print("MD5 (\"wemos d1 mini\") = ");
  Serial.print(md5("wemos d1 mini"));
  Serial.println(" (calculated)\n");
}

void loop()
{
  //Nada especial que hacer aquí
}
