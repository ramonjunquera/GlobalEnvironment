/*
  Autor: Ramón Junquera
  Fecha: 20191120
  Tema: Librería de gestión de EEPROM de RTC Tiny
  Objetivo: Demo de librería

  Descripción:
    A través del puerto serie se permite el envío de comandos.
    En Platformio se envían los caracteres al instante tras su pulsación.
    El programa acumula los caracteres recibidos en un String que hace de buffer,
    hasta encontrar un retorno de línea.
    Lo comandos son: peek y poke que permiten recuperar y escribir en la EEPROM.
    Ejemplos de sintaxis:
      peek 42 (muestra el contenido de la posición 42)
      poke 34,65 (escribe el valor 65 en la posición 34)
    Recordemos que RTC Tiny tiene una memoria EEPROM de 8Kb, por lo tanto la
    posición 0 será la misma que la 8192 (es cíclico).
*/

#include <Arduino.h>
#include <RoJoRTCtinyEE.h>

RoJoRTCtinyEE mem; //Gestión de la memoria EEPROM del RTC
String buffer=""; //Buffer con los caracteres de entrada pendientes por procesar

void setup() {
  //Activamos la conexión por puerto serie
  Serial.begin(115200);
  if(!mem.begin()) Serial.println(F("\nEEPROM no encontrada!"));
  else Serial.println(F("\nEEPROM activa"));
}

//Devuelve un valor numérico entero extraido de un String a partir de una posición
//Devuelve -1 si no puede obtener nada
int16_t parseInt(String s,byte startIndex) {
  int16_t value=-1; //Valor por defecto
  //Si no debemos comenzar desde el principio...eliminamos el prefijo
  if(startIndex>0) s=s.substring(startIndex);
  //Mientras haya algo que procesar y sea numérico...
  while(s.length()>0 && s[0]>='0' && s[0]<='9') {
    if(value<0) value=0; //Si es el primer carácter numérico...comenzamos con valor 0
    value=value*10+(s[0]-'0'); //Añadimos el valor numérico del primer carácter
    s=s.substring(1); //Eliminamos el primer carácter porque ya lo hemos procesado
  }
  return value;
}

void loop() {
  //Si hay algo pendiente de ser recibido...
  if(Serial.available()) {
    char c=Serial.read(); //Leemos el carácter
    Serial.print(c); //Lo mostramos
    buffer+=c; //Lo añadimos el buffer
    
    //Procesamos el contenido del buffer

    //Si el buffer contiene un retorno de línea, es porque se ha terminado
    //de escribir y lo podemos procesar
    if(buffer.indexOf('\n')>=0) {
      if(buffer.startsWith("peek ")) {
        //Reconocido comando peek
        int16_t address=parseInt(buffer,5); //Obtenemos la dirección a partir de la posición 5
        if(address<0) { //Si la dirección es incorrecta...
          Serial.println("Error. Dirección incorrecta.");
        } else { //Dirección correcta
          Serial.println("peek "+String(address)+" = "+String(mem.peek(address)));
        }
      } else if (buffer.startsWith("poke ")) {
        //Reconocido comando poke
        int16_t address=parseInt(buffer,5); //Obtenemos la dirección a partir de la posición 5
        if(address<0) { //Si la dirección es incorrecta...
          Serial.println("Error. Dirección incorrecta.");
        } else { //Dirección correcta
          byte indexComma=5+String(address).length(); //Calculamos la posición de la coma
          if(buffer[indexComma]!=',') { //Si no encontramos la coma...
            Serial.println("Error. Coma no encontrada.");
          } else { //Hemos encontrado la coma
            int16_t value=parseInt(buffer,indexComma+1);
            if(value<0 || value>255) { //Si el valor es incorrecto...
              Serial.println("Error. Valor incorrecto.");
            } else { //Valor correcto
              mem.poke(address,value);
              Serial.println("poke "+String(address)+","+String(value));
            }
          }
        } 
      } else {
        Serial.println("comando desconocido");
      }
      buffer=""; //Limpiamos el buffer para recibir el siguiente comando
    } 
  }
}