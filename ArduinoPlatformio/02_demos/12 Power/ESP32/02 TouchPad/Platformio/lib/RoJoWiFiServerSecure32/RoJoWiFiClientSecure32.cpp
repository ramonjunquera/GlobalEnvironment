/*
  Nombre de la librería: RoJoWiFiServerSecure32.h
  Autor: Ramón Junquera
  Fecha: 20211107
  Descripción:
    Cliente seguro (SSL)
*/

//Impedimos que se defina los métodos más de una vez
#ifndef RoJoWiFiClientSecure32_CPP
#define RoJoWiFiClientSecure32_CPP

#include <RoJoWiFiClientSecure32.h>

//Detiene la conexión
void RoJoWiFiClientSecure32::stop() {
  if(_SSLconn!=NULL) { //Si hay alguna conexión...
    SSL_shutdown(_SSLconn); //...la paramos
    SSL_free(_SSLconn); //Liberamos la conexión
    close(_fd); //Cerramos el socket
    _SSLconn=NULL; //Ya no tenemos conexión
  } 
}

//Constructor para cliente con socket
//Incluimos en la declaración guardar el parámtro del socket en la variable privada
RoJoWiFiClientSecure32::RoJoWiFiClientSecure32(int fd,SSL_CTX *SSLcontext):_fd(fd) {
  //Creamos una nueva conexión en el contexto SSL
  _SSLconn=SSL_new(SSLcontext);
  //Si no hemos podido crear la conexión...paramos
  if(_SSLconn==NULL) stop();
  else { //Hemos podido crear la conexión...
    //Relacionamos la conexión con la del cliente conectado
    SSL_set_fd(_SSLconn,fd);
    //Si no se acepta la conexión con el cliente...paramos
    if(!SSL_accept(_SSLconn)) stop();
  }
}

//Destructor
RoJoWiFiClientSecure32::~RoJoWiFiClientSecure32() {
  stop(); //Detenemos cualquier conexión abierta
}

//Lee datos del buffer de entrada
//Parámetros:
//  - Puntero del buffer
//  - Tamaño del buffer
//Devuelve:
//  -2 : No tenemos conexión
//  -1 : Timeout
// >=0 : Número de bytes leidos
int RoJoWiFiClientSecure32::read(byte *buf, size_t size) {
  //Si no tenemos conexión...devolvemos error
  if (_SSLconn==NULL) return -2;
  //Tenemos conexión

  //Solicitamos recepción de datos y guardado en el buffer
  uint32_t m=millis(); //Anotamos millis actuales
  int count=SSL_read(_SSLconn,buf,size);
  m=millis()-m; //Calculamos cuánto tiempo ha tardado en ejecutar la lectura

  if(m==0 && count==0) { //Si hemos perdido la conexión...
    //Si hay datos pendientes de ser leidos...mentiremos y diremos que aun
    //se mantiene la conexión para que se puedan procesar
    if(SSL_pending(_SSLconn)) return 0;
    //No hay datos pendientes de ser leidos
    stop(); //Finalizamos la conexión del cliente
    return -2; //Indicamos que no tenemos conexión
  }
  return count; //Devolvemos el número de bytes leidos o -1 si fue un timeout
}

//Lee el siguiente byte del cliente
//Devuelve true si lo consigue
bool RoJoWiFiClientSecure32::read(byte *b) {
  int count=read(b,1); //Leemos un sólo byte
  return count==1; //Si hemos conseguido leer 1 byte...todo ok
  //Nota: llamamos al otro método read porque ya hace la gestión de
  //desconexión si es necesaria.
}

//Tenemos conexión?
bool RoJoWiFiClientSecure32::connected() {
  byte buffer[1]; //Buffer de lectura
  //Leemos 0 bytes en el buffer. Si no estamos desconectados...todo Ok
  return read(buffer,0)!=-2;
}

//Lee caracteres hasta encontrar una cadena, detectar
//la cadena endText o haber leido todos los caracteres pendientes.
//El parámetro endText es optativo.
//La longitud de los parámetros findText y endText puede ser distinta.
//Devuelve true si ha encontrado la cadena
bool RoJoWiFiClientSecure32::readFindString(String findText,String endText) {
  //Buffer de texto recibido
  String buffer="";
  //Calculamos el tamaño del buffer
  byte bufferLen=findText.length();
  if(endText.length()>bufferLen) bufferLen=endText.length();

  //Inicializamos el buffer a su tamaño correcto
  for(byte c=0;c<bufferLen;c++) buffer+="@";
  //Carácter recibido
  byte c;
  //Mientras recibamos caracteres...
  while(read(&c)) {
    //Añadimos el carácter recibido al final del buffer
    buffer=buffer.substring(1)+(char)c;

    //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
    if(buffer.substring(bufferLen-findText.length())==findText) return true;
    //Si tenemos cadena de fin...
    if(endText.length()) {
      //Si el final del buffer coincide con la cadena de fin...
      //...no lo hemos encontrado
      if(buffer.substring(bufferLen-endText.length())==endText) return false;
    }
  }
  //Hemos terminado de leer todos los caracteres y no hemos encontrado
  //la cadena que buscábamos
  return false;
}

//Devuelve el texto recibido hasta encontrar la cadena de marca de fin.
//Si no encuentra la marca de fin, devuelve todo una cadena vacía.
String RoJoWiFiClientSecure32::readString(String endText) {
  //Variable en la que guardaremos la respuesta
  String answer="";

  //Carácter recibido
  byte c;
  //Mientras recibamos caracteres...
  while(read(&c)) {
    //Leemos el siguiente carácter y lo añadimos a la respuesta
    //Añadimos el carácter recibido a la respuesta
    answer+=(char)c;
    //Si el final de la cadena coincide con la marca de fin...
    if(answer.substring(answer.length()-endText.length())==endText) {
        //...devolvemos la respuesta sin incluir la marca de fin
      return answer.substring(0,answer.length()-endText.length());
    }
  }
  //Ya no hay más caracteres que leer y no hemos encontrado la
  //marca de fin. Devolvemos cadena vacía
  return "";
}

//Parsea caracteres numéricos
//Se detiene cuando encuentra un carácter no numérico
String RoJoWiFiClientSecure32::readInt() {
  String answer=""; //Variable en la que guardaremos la respuesta
  byte c; //Carácter leido
  while(read(&c)) { //Mientras recibamos caracteres...
    //Si es un carácter numérico...lo tomamos como correcto lo añadimos a la respuesta
    if(c>='0' && c<='9') answer+=(char)c;
    //Si no es un carácter numérico...devolvemos el resultado
    else return answer;
  }
  //Hemos terminado de leer todos los caracteres.
  //Devolvemos lo que hayamos podido encontrar
  return answer;
}

//Envía una cadena al cliente
void RoJoWiFiClientSecure32::write(String s) {
  //Si hay alguna conexión...enviamos el String
  //Previamente debemos transformarlo a formato C
  if(_SSLconn!=NULL) SSL_write(_SSLconn,s.c_str(),s.length());
}

//Devuelve el número de bytes pendientes de leer del buffer
//o -1 si hemos perdido la conexión
int RoJoWiFiClientSecure32::available() {
  if(connected()) return SSL_pending(_SSLconn);
  return -1;
}

//Localiza un campo numérico en datos JSON recibidos {"fieldName":1234}
//Parámetros:
//- fieldName : Nombre del campo sin comillas
//- endText : señal de fin de búsqueda (opcional)
//Devuelve String con valor numérico o String vacío si no se encuentra
String RoJoWiFiClientSecure32::readFindFieldInt(String fieldName,String endText) {
  String answer=""; //Inicialmente no hemos encontrado nada
  //Añadimos las comillas al nombre del campo y los dos puntos de separación con su valor
  //Si hemos encontrado el campo...leemos el valor entero
  if(readFindString("\""+fieldName+"\":",endText)) answer=readInt();
  return answer; //Devolvemos el resultado
}

//Localiza un campo de texto en datos JSON recibidos {"fieldName":"textValue"}
//Parámetros:
//- fieldName : Nombre del campo sin comillas
//- endText : señal de fin de búsqueda (opcional)
//Devuelve String con valor del campo sin comillas o String vacío si no se encuentra
String RoJoWiFiClientSecure32::readFindFieldString(String fieldName,String endText) {
  String answer=""; //Inicialmente no hemos encontrado nada
  //Añadimos las comillas al nombre del campo, los dos puntos de separación con su valor
  //y las comillas iniciales del valor
  //Si hemos encontrado el campo...leemos el texto hasta las siguientes comillas
  if(readFindString("\""+fieldName+"\":\"",endText)) answer=readString("\"");
  return answer;
}

#endif