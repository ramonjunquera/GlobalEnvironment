/*
  Autor: Ramón Junquera
  Fecha: 20201018
  Tema: PoC SEAT Happy Faces
  Material: M5Stick C, buzzer pasivo, M5Stick ToF HAT
  Descripción:
    Sistema de recopilación de encuestas.
    Se muestra la pregunta en un texto impreso y una serie de caras de distintos
    colores que indican la satisfacción.
    Para evitar contactos, el dispositivo es capaz de detectar la cara a la que
    se apunta gracias al sensor de distancia.
    Cuando se detecta una selección, se muestra la cara selecciona en pantalla
    junto con una barra de progreso que tardará dos segundos en llenarse.
    Si se completa la selección, se contabiliza internamente, suena un pitido y
    se muestra un icono que muestra que se ha completado correctamente.
    Los resultados pueden ser consultados y reseteados por web http://192.168.4.1
    Cada vez que se actualiza una selección, se guarda e resultado en
    el sistema de archivos interno para que no se pierdan los valores al
    apagar del dispositivo.
*/

#include <Arduino.h>
#include <RoJoST7735S.h> //Destión de display
#include <RoJoM5ToFHAT.h> //Gestión de sensor ToF
#include <SPIFFS.h>
#include <WiFi.h>

//Definimos los detalles del punto de acceso
const char* wifiSSID = "HappyFaces";
const char* wifiPassword = "12345678";

//Definición de objetos globales
RoJoM5ToFHAT tof;
RoJoST7735S display;
byte lastFace=255; //Cara anterior=ninguna
const uint32_t cyclesCheckFaceMax=7000; //0.1s aprox, ciclos para comprobar selección de cara
uint32_t cyclesCheckFace=cyclesCheckFaceMax; //Contador de ciclos para selección de cara
byte cyclesSelectedFace=0; //Contador de ciclos de cara seleccionada
uint16_t points[4]; //Puntos de cada cara
const byte pinBuzzer=32;
WiFiServer server(80); //Creamos un servidor wifi en el puerto 80
const String pointsFile="/points.dat";

//Recupera archivo de puntos (si existe)
void loadPoints() {
  File file = SPIFFS.open(pointsFile);
  if(!file) { //Si el archivo no existe...
    for(byte i=0;i<4;i++) points[i]=0; //Todos los valores a cero
  } else { //El archivo existe...
    file.readBytes((char*)points,2*4); //Leemos directamente a la matriz
    file.close();
  }
}

//Guarda puntos en archivo
void savePoints() {
  File file = SPIFFS.open(pointsFile,"w");
  file.write((byte*)points,2*4); //Guardamos la matriz directamente
  file.close();
}

void setup() {
  pinMode(pinBuzzer,OUTPUT); //Definimos el pin de buzzer como salida
  digitalWrite(pinBuzzer,LOW); //Buzzer en silencio
  display.begin(); //Inicialización por defecto de display para M5StickC
  display.rotation(3); //Botón a la izda
  tof.begin(); //Inicializamos el sensor de distancia
  SPIFFS.begin(); //Inicialización de sistema de archivos
  loadPoints(); //Recuperamos puntos (si se puede)
  //Creamos el punto de acceso wifi
  WiFi.mode(WIFI_AP); //Modo punto de acceso
  WiFi.softAP(wifiSSID,wifiPassword); //Nombre y contraseña
  server.begin(); //Arrancamos el servidor web
}

//Devuelve la cara seleccionada. 255=ninguna
byte getFace() {
  uint16_t d=tof.get(); //Obtenemos la distancia
  byte face=255; //Cara seleccionada. Por defecto ninguna
  if(d>=49) { //Si supera el rango mínimo...
    face=(d-49)/57;
    if(face>3) face=255; //Si supera el número máximo de caras...no es válido
  }
  return face;
}

//Gestiona la cara seleccionada
void manageFace(byte face) {
  if(face!=lastFace) { //Si se ha cambiado la selección...
    lastFace=face;
    cyclesSelectedFace=0; //Reseteamos el contador de cara seleccionada
    //Si se ha seleccionado una cara...se muestra
    if(face<=3) display.drawSprite("/face"+String(face)+".spr");
    //Si no hay nada seleccionado...se borra la pantalla
    else display.clear();
  }
  else { //Si no se ha cambiado la selección...
    if(face<=3) { //Si hay una cara seleccionada...
      if(cyclesSelectedFace>=20) { //Si hemos completado la selección...
        if(cyclesSelectedFace==20) { //Si acabamos de completar la selección...
          digitalWrite(pinBuzzer,HIGH); //Buzzer activado
          display.drawSprite("/ok.spr",80); //Mostramos el Ok
          points[face]++; //Contabilizamos la selección
          savePoints(); //Guardamos nuevas puntuaciones
          cyclesSelectedFace++; //La próxima no se redibujará el Ok
          digitalWrite(pinBuzzer,LOW); //Buzzer en silencio
        } 
      } else { //Si no hemos completado la selección...
        //Dibujamos un bloque más en la barra de progreso
        display.block(103,68-3*cyclesSelectedFace,136,66-3*cyclesSelectedFace,{0,0,255});
        cyclesSelectedFace++;
      }
    }
  }
}

//Lee caracteres del cliente hasta encontrar una cadena, detectar la cadena
//endText o alcanzar el tiempo de time out.
//El parámetro endText es optativo.
//La longitud de los parámetros findText y endText puede ser distinta.
//Devuelve true si ha encontrado la cadena.
bool _clientFindString(WiFiClient *client,uint16_t timeOut,String findText,String endText="") {
  uint32_t waitTime=millis()+timeOut; //Calculamos el máximo tiempo de espera
  String buffer=""; //Buffer de texto recibido
  byte bufferLen=findText.length(); //Calculamos el tamaño del buffer
  if(endText.length()>bufferLen) bufferLen=endText.length();
  //Inicializamos el buffer a su tamaño correcto
  for(byte c=0;c<bufferLen;c++) buffer+="@";
  while(millis()<waitTime) { //Mientras no hayamos consumido todo el tiempo permitido...
    while((*client).available()) { //Mientras haya caracteres pendientes por recibir...
      //Leemos el siguiente carácter y lo añadimos al buffer
      buffer=buffer.substring(1)+(char)(*client).read();
      //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
      if(buffer.substring(bufferLen-findText.length())==findText) return true;
      if(endText.length()) { //Si tenemos cadena de fin...
        //Si el final del buffer coincide con la cadena de fin...no lo hemos encontrado
        if(buffer.substring(bufferLen-endText.length())==endText) return false;
      }
    }
  }
  //TimeOut. No lo hemos encontrado
  return false;
}

//Devuelve el texto recibido hasta encontrar la cadena de marca de fin.
//Si no encuentra la marca de fin o se alcanza el tiempo límite devuelve
//una cadena vacía.
String _clientReadString(WiFiClient *client,uint16_t timeOut,String endText) {
  uint32_t waitTime=millis()+timeOut; //Calculamos el máximo tiempo de espera
  String answer=""; //Variable en la que guardaremos la respuesta
  while(millis()<waitTime) { //Mientras no hayamos consumido todo el tiempo permitido...
    while((*client).available()) { //Mientras haya caracteres pendientes por recibir...
      answer+=(char)(*client).read(); //Leemos el siguiente carácter y lo añadimos a la respuesta
      //Si el final de la cadena coincide con la marca de fin...
      if(answer.substring(answer.length()-endText.length())==endText) {
         //...devolvemos la respuesta sin incluir la marca de fin
        return answer.substring(0,answer.length()-endText.length());
      }
    }
  }
  //No hemos encontrado la marca de fin
  return "";
}

//Gestión del servidor web
void manageWeb() {
  WiFiClient client = server.available(); //Anotamos si hay algún cliente
  if(client) { //Si hay alguno...
    while (client.connected()) { //Mientras el cliente esté conectado...
      //Este es un ejemplo del formato de la información que podemos recibir:
        //GET /mypath HTTP/1.1
        //Content-type: text/plain
        //host: myserver.domain.com:80
        //content-length: 49
        //x-newrelic-id: VwAOU1RRGGAFUFZUAwQE
        //x-newrelic-transaction: PxRVBVYCY1VTV1ZTAgJSVkYdUFIOFQZOEloMAVtcAwFWAwhSVFRQVFIUG0MEWwkGCVEFABVs
        //Connection: close
        //
        //Texto contenido en el mensaje
      //Sólo tendremos en cuenta valor del path de la cabecera del mensaje
      //Lo encontramos en la primera línea
      //Leeremos el mensaje hasta encontrar el prefijo al path y como medida de
      //seguridad incluimos el sufijo del path para no perder tiempo en leer
      //el mensaje completo cuando hay errores.
      //Si hemos encontrado el prefijo del path...
      if(_clientFindString(&client,1500,"GET /","HTTP/")) {
        String path=_clientReadString(&client,1500," HTTP/"); //...leemos el path
        //Dependiendo del path tomaremos acciones
        //Si tenemos que resetear la puntuación...lo hacemos 
        if(path=="reset") for(byte i=0;i<4;i++) points[i]=0; //Puntos a cero
        //No tenemos en cuenta ninguna otra opción de path
        //No tenemos en cuenta el resto del mensaje
        //Respondemos con a la petición web (código html)
        String html="HTTP/1.1 200 OK\nContent-Type: text/html\n\n <!doctype html><html><h1>Happy Faces Web Server</h1>";
        for(byte i=0;i<4;i++) html+="<p>Face#"+String(i)+"="+String(points[i])+"</p>";
        html+="<p><a href=\"/\"><button>REFRESH</button></a>&nbsp;<a href=\"/reset\"><button>RESET</button></a></p></html>";
        client.println(html);
      }
      client.stop(); //Cortamos la conexión
    }
  }
}

void loop() {
  if(--cyclesCheckFace==0) { //Si hay que comprobar la cara seleccionada...
    cyclesCheckFace=cyclesCheckFaceMax;
    manageFace(getFace()); //Obtenemos cara y la gestionamos
  }
  manageWeb(); //Gestionamos servidor web
}
