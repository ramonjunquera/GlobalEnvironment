/*
  Autor: Ramón Junquera
  Tema: Gestión de la memoria EEPROM integrada
  Objetivo: Guardar y leer valores en la memoria EEPROM integrada
  Material adicional: breadboard, cualquier placa Arduino o ESP8266, cables

  Descripción:
  Crearemos un programa que nos permita interactuar con la memoria EEPROM.
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

#if defined(ESP32)
  #error Las placas ESP32 actualmente no soportan gestión de EEPROM integrada
#endif 

#include <Arduino.h>
#include <EEPROM.h>

//Definimos la cantidad de memoria en función de la placa
//Todas tienen 1Kb, excepto Mega y las ESP8266 tiene 4Kb
#if defined(ARDUINO_AVR_MEGA2560) || defined(ESP8266)
  const uint16_t maxMem=4096;
#else
  const uint16_t maxMem=1024;
#endif
//Definimos la máxima longitud de texto de los comandos
const byte commandMaxLength = 5; //4 + el caracter 0 de fin de cadena
//Definimos un buffer para guardarlo
byte textBuffer[commandMaxLength];

void setup()
{
  //Activamos la conexión por puerto serie
  Serial.begin(115200);

  //En las placas ESP8266 el tamaño de mamoria EEPROM debe ser inicializado
  #if defined(ESP8266)
    //Podemos definir el tamaño de la memoria EEPROM entre 1 y 4096 bytes
    //Realmente no es memoria EEPROM sino parte de SPIFFS
    EEPROM.begin(4096);
  #endif
  Serial.println(F("\nPreparado!"));
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
        if(address>=maxMem)
        {
          Serial.println(F("Error. Direccion demasiado grande"));
        }
        else
        {
          //Recuperamos el valor de esa dirección y lo mostramos
          Serial.println(EEPROM.read(address));
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
        if(address>=maxMem)
        {
          Serial.println(F("Error. Direccion demasiado grande"));
        }
        else
        {
          //Escribimos el valor en la dirección
          EEPROM.write(address,value);
          //En las placas ESP8266 es necesario hacer un commit para que se guarden los posibles
          //cambios pendientes de escribir que aun quedan en cache
          #if defined(ESP8266)
            EEPROM.commit();
          #endif
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
