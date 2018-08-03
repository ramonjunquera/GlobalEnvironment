/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Demo de librería de gestión de la memoria EEPROM del módulo de reloj Tiny RTC I2C Real Time
  Material: breadboard, cualquier placa, cables, módulo "Tiny RTC I2C"

  Descripción:
  RTC incluye una memoria de 32Kb = 32*1024= 8192 bytes
  Crearemos un programa que nos permita interactuar con la memoria EEPROM que contiene el Tiny RTC.
  Nos comunicaremos a traves del puerto serie.
  Utilizaremos una colección de comandos básicos para ello: poke y peek.
  Ejemplos:
    Escribe el valor 23 en la posición de memoria 700
      poke 700,23
    Lee el contenido de la posición de memoria 932
      peek 932

  Con estos simples comandos podemos leer y escrbir en la memoria EEPROM.
  
  Resultado:
  Con la colección de comando podemos escribir y leer en la memoria EEPROM
*/

#include <Arduino.h>
#include "RoJoRTCeeprom.h"

//Objeto de gestión de la memoria EEPROM del RTC
RoJoRTCeeprom mem;

//Definimos la máxima longitud de texto de los comandos
const byte commandMaxLength = 5; //4 + el caracter 0 de fin de cadena
//Definimos un buffer para guardarlo
byte textBuffer[commandMaxLength];

void setup()
{
  //Activamos la conexión por puerto serie
  Serial.begin(115200);
  if(!mem.check()) Serial.println(F("\nEEPROM no encontrada!"));
  else Serial.println(F("\nEEPROM activa"));
}

void loop()
{
  uint16_t address;
  byte value;
  
  //Si hay algo pendiente de ser recibido...
  if(Serial.available())
  {
    //Leemos texto hasta encontrar el caracter espacio y tomamos nota del número de bytes leidos
    byte countBytes=Serial.readBytesUntil(' ',textBuffer,commandMaxLength);
    //Todos los comandos tienen una longitud de 4
    if(countBytes==4)
    {
      //Si se trata del comando peek...
      if(textBuffer[0]=='p' && textBuffer[1]=='e' && textBuffer[2]=='e' && textBuffer[3]=='k')
      {
        //...obtenemos la dirección
        address=Serial.parseInt();
        //Mostramos el comando reconocido
        Serial.print(F("peek "));
        Serial.println(address);
        //Si la dirección es mayor que la memoria real...error
        if(address>=8192)
        {
          Serial.println(F("Error. Direccion demasiado grande"));
        }
        else
        {
          //Recuperamos el valor de esa dirección y lo mostramos
          Serial.println(mem.peek(address));
        }
      }
      //Si se trata del comando poke...
      else if(textBuffer[0]=='p' && textBuffer[1]=='o' && textBuffer[2]=='k' && textBuffer[3]=='e')
      {
        //...obtenemos la dirección
        address=Serial.parseInt();
        //...obtenemos el valor
        value=Serial.parseInt();
        Serial.print(F("poke "));
        Serial.print(address);
        Serial.print(F(","));
        Serial.println(value);
        //Si la dirección es mayor que la memoria real...error
        if(address>=8192)
        {
          Serial.println(F("Error. Direccion demasiado grande"));
        }
        else
        {
          //Escribimos el valor en la dirección
          mem.poke(address,value);
          Serial.println(F("ok"));
        }
      }
      else
      {
        Serial.println(F("Comando desconocido"));
      }
    }
  }
}
