/*
  Autor: Ramón Junquera
  Fecha: 20190502
  Descripción:
    Ejemplo de generación de archivos de clave privada y certificado.
    En los parámetros iniciales se definen las características de los archivos.
    En el ejemplo actual se fuerza a regenerar los archivo, aunque ya existan.
*/

#include <Arduino.h>
#include <RoJoAutoCert32.h>

//Parámetros
//Archivo de certificado
const String certFileName="/autocert.crt";
//Archivo de clave privada
const String privateKeyFileName="/private.key";
//Nombre descriptivo para el certificado
const String name="CN=esp32.local,O=RoJo,C=SP";
//Fecha de validez desde 01/01/2019
const String validityFrom="20190101000000";
//Fecha de validez hasta 01/01/2030
const String validityTo="20390101000000";
//Tamaño de la clave de enciptación en bits
uint16_t encriptionLength=512;
 
void setup()
{ 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  delay(2000);
  Serial.print("Creando clave privada & certificado...");
  byte errorCode=RoJoAutoCert(certFileName,privateKeyFileName,name,validityFrom,validityTo,true,encriptionLength);
  Serial.println("Error "+String(errorCode));
}

void loop(void)
{
  //Nada especial que hacer aquí
}