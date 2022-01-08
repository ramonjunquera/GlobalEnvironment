/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Descripción:
    Ejemplo de generación de archivos de clave privada y certificado.
    En los parámetros iniciales se definen las características de los archivos.
    En el ejemplo actual se fuerza a regenerar los archivos, aunque ya existan.
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
const String validityFrom="20220101000000";
//Fecha de validez hasta 01/01/2030
const String validityTo="20390101000000";
//Tamaño de la clave de encriptación en bits
uint16_t encriptionLength=2048;
 
void setup() { 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  while(!Serial) delay(1);
  Serial.print("\nCreando clave privada & certificado...");
  byte errorCode=RoJoAutoCert(certFileName,privateKeyFileName,name,validityFrom,validityTo,true,encriptionLength);
  Serial.println("Error "+String(errorCode));
}

void loop(void) {
  //Nada especial que hacer aquí
}