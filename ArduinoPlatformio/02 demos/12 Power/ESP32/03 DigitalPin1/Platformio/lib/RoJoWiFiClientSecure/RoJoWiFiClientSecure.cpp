/*
  Nombre de la librería: RoJoWiFiServerSecure.h
  Autor: Ramón Junquera
  Fecha: 20211107
  Descripción:
    Clase heredada de WiFiServerSecure con funciones de gestión de datos recibidos.
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiClientSecure_CPP
#define RoJoWiFiClientSecure_CPP

#include <RoJoWiFiClientSecure.h>

//Devuelve el texto recibido hasta encontrar la cadena de marca de fin o timeout
//Parámetros:
// - endText: marca de fin de búsqueda (opcional)
// - timeOut: tiempo de espera máximo en milisegundos (opcional). Por defecto 1500
String RoJoWiFiClientSecure::readString(String endText,uint16_t timeOut) {
  uint32_t maxTime=millis()+timeOut; //Calculamos tiempo máximo de espera
  String answer=""; //inicialmente no hemos encontrado nada
  while(millis()<maxTime) { //Mientras no estemos fuera de tiempo...
    while(available()) { //Mientras haya caracteres pendientes por recibir...
      answer+=(char)read(); //Leemos el siguiente carácter y lo añadimos a la respuesta
      //Si el final de la cadena coincide con la marca de fin...
      if(answer.substring(answer.length()-endText.length())==endText) {
         //...devolvemos la respuesta sin incluir la marca de fin
        return answer.substring(0,answer.length()-endText.length());
      }
    }
  }
  return ""; //Texto no encontrado
}

//Busca un texto
//Lee caracteres hasta encontrar una cadena, detectar
//la cadena endText o haber superar el timeOut
//Parámetros:
// - findText : String a buscar
// - endText: marca de fin de búsqueda (opcional)
// - timeOut: tiempo de espera máximo en milisegundos (opcional). Por defecto 1500
//Devuelve true si ha encontrado la cadena
bool RoJoWiFiClientSecure::readFindString(String findText,String endText,uint16_t timeOut) {
  uint32_t maxTime=millis()+timeOut; //Calculamos tiempo máximo de espera
  String buffer=""; //Buffer de texto recibido
  byte bufferLen=findText.length(); //Calculamos el tamaño del buffer
  if(endText.length()>bufferLen) bufferLen=endText.length();
  for(byte c=0;c<bufferLen;c++) buffer+="@"; //Inicializamos el buffer a su tamaño correcto
  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<maxTime) { //Mientras no estemos fuera de tiempo...
    while(available()) { //Mientras haya caracteres pendientes por recibir...
      buffer=buffer.substring(1)+(char)read(); //Leemos el siguiente carácter y lo añadimos al buffer
      //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
      if(buffer.substring(bufferLen-findText.length())==findText) return true;
      if(endText.length()) { //Si tenemos cadena de fin...
        //Si el final del buffer coincide con la cadena de fin...no lo hemos encontrado
        if(buffer.substring(bufferLen-endText.length())==endText) return false;
      }
    }
    //Añadimos un delay para evitar error de WatchDog cuando tenemos
    //un timeout alto y no se reciben nuevos datos
    delay(1);
  }
  return false; //TimeOut. No lo hemos encontrado
}

//Localiza un campo de texto en datos JSON recibidos {"fieldName":"textValue"}
//Parámetros:
// - fieldName : Nombre del campo sin comillas
// - endText : señal de fin de búsqueda (opcional)
// - timeOut: tiempo de espera máximo en milisegundos (opcional). Por defecto 1500
//Devuelve String con valor del campo sin comillas o String vacío si no se encuentra
String RoJoWiFiClientSecure::readFindFieldString(String fieldName,String endText,uint16_t timeOut) {
  String answer=""; //inicialmente no hemos encontrado nada
  //Añadimos las comillas al nombre del campo, los dos puntos de separación con su valor
  //y las comillas iniciales del valor
  if(readFindString("\""+fieldName+"\":\"",endText,timeOut)) { //Si hemos encontrado el campo...
    answer=readString("\"",timeOut); //...leemos el texto hasta las siguientes comillas
  }
  return answer;
}

//Parsea caracteres numéricos
//Se detiene cuando encuentra un carácter no numérico o superamos el timeOut
//Devuelve el número en un String
String RoJoWiFiClientSecure::readInt(uint16_t timeOut) {
  uint32_t maxTime=millis()+timeOut; //Calculamos tiempo máximo de espera
  String answer=""; //inicialmente no hemos encontrado nada
  char c; //Carácter leido

  while(millis()<maxTime) { //Mientras no estemos fuera de tiempo...
    while(available()) { //Mientras haya caracteres pendientes por recibir...
      c=read(); //Leemos el siguiente carácter
      if(c>='0' && c<='9') { //Si es un carácter numérico...
        //...lo tomamos como correcto lo añadimos a la respuesta
        //El código ASCII del '0' es el 48
        answer+=c;
      } else return answer; //Si no es un carácter numérico...devolvemos el resultado
    }
  }
  return answer; //TimeOut. Devolvemos lo que hayamos podido encontrar
}

//Localiza un campo numérico en datos JSON recibidos {"fieldName":1234}
//Parámetros:
//- fieldName : Nombre del campo sin comillas
//- endText : señal de fin de búsqueda (opcional)
// - timeOut: tiempo de espera máximo en milisegundos (opcional). Por defecto 1500
//Devuelve String con valor numérico o String vacío si no se encuentra
String RoJoWiFiClientSecure::readFindFieldInt(String fieldName,String endText,uint16_t timeOut) {
  String answer=""; //inicialmente no hemos encontrado nada
  //Añadimos las comillas al nombre del campo y los dos puntos de separación con su valor
  //Si hemos encontrado el campo...leemos el valor entero
  if(readFindString("\""+fieldName+"\":",endText,timeOut)) answer=readInt(timeOut);
  return answer; //Devolvemos el resultado
}

#endif