/*
  Autor: Ramón Junquera
  Fecha: 20190324
  Tema: Simple Web Server
  Objetivo: Ejemplo sencillo de servidor web para ESP32
  Material adicional: Wemos LOLIN32

  Crearemos un servidor web.
  Será muy simple, porque sólo tiene una página web estática.
  La página sólo muestra un título y dos botones.
  El servidor tiene en cuenta el path solicitado.
  Se reconocen dos valores en el path: /on y /off para encender o apagar el led integrado.
  Cualquier otro valor para el path no tiene tiene función.
  Tampoco se tiene en cuenta el contenido de la solicitud.

  Puesto que en ESP32 no existe el equivalente a la librería ESP8266WebServer.h de ESP8266,
  tendremos que procesar manualmente tanto la recepción de la petición como la repuesta.

  El puerto del servidor es configurable.
  Un ejemplo de URL de llamada pordría ser: http://192.168.1.139:8266
*/

#include "Arduino.h"
#include <WiFi.h>

//Definimos los detalles del punto de acceso
const char* wifiClientSSID = "xxx";
const char* wifiClientPassword = "xxx";
//Definimos el pin del led
const byte pinLed=LED_BUILTIN;
//Definimos cuándo está encendido
const bool ledON=false;
//Puerto de escucha del servidor
const uint16_t port=8266;
//Definimos la respuesta de nuestro servidor web
const String webPage = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n <!doctype html><html><h1>ESP32 Simple Web Server</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a></p></html>";

//Creamos un servidor wifi en el puerto indicado
WiFiServer server(port);

bool _clientFindString(WiFiClient *client,uint16_t timeOut,String findText,String endText="")
{
  //Lee caracteres del cliente hasta encontrar una cadena, detectar la cadena endText o alcanzar el tiempo de time out
  //El parámetro endText es optativo
  //La longitud de los parámetros findText y endText puede ser distinta
  //Devuelve true si ha encontrado la cadena

  //Calculamos el máximo tiempo de espera
  uint32_t waitTime=millis()+timeOut;  
  //Buffer de texto recibido
  String buffer="";
  //Calculamos el tamaño del buffer
  byte bufferLen=findText.length();
  if(endText.length()>bufferLen) bufferLen=endText.length();

  //Inicializamos el buffer a su tamaño correcto
  for(byte c=0;c<bufferLen;c++) buffer+="@";
  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<waitTime)
  {
    //Mientras haya caracteres pendientes por recibir...
    while((*client).available())
    {
      //Leemos el siguiente carácter y lo añadimos al buffer
      buffer=buffer.substring(1)+(char)(*client).read();
      //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
      if(buffer.substring(bufferLen-findText.length())==findText) return true;
      //Si tenemos cadena de fin...
      if(endText.length())
      {
        //Si el final del buffer coincide con la cadena de fin...no lo hemos encontrado
        if(buffer.substring(bufferLen-endText.length())==endText) return false;
      }
    }
  }
  //TimeOut. No lo hemos encontrado
  return false;
}

String _clientReadString(WiFiClient *client,uint16_t timeOut,String endText)
{
  //Devuelve el texto recibido hasta encontrar la cadena de marca de fin
  //Si no encuentra la marca de fin o se alcanza el tiempo límite devuelve una cadena vacía
  
  //Calculamos el máximo tiempo de espera
  uint32_t waitTime=millis()+timeOut;
  //Variable en la que guardaremos la respuesta
  String answer="";

  //Mientras no hayamos consumido todo el tiempo permitido...
  while(millis()<waitTime)
  {
    //Mientras haya caracteres pendientes por recibir...
    while((*client).available())
    {
      //Leemos el siguiente carácter y lo añadimos a la respuesta
      answer+=(char)(*client).read();
      //Si el final de la cadena coincide con la marca de fin...
      if(answer.substring(answer.length()-endText.length())==endText)
      {
         //...devolvemos la respuesta sin incluir la marca de fin
        return answer.substring(0,answer.length()-endText.length());
      }
    }
  }
  //No hemos encontrado la marca de fin
  return "";
}

void setup()
{
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Modo conexión a punto de acceso
  WiFi.begin(wifiClientSSID,wifiClientPassword); //Solicitamos conexión

  //El pin del led es de salida
  pinMode(pinLed,OUTPUT);
  //Inicialmente está apagado
  digitalWrite(pinLed,!ledON);

  Serial.print("Connecting");
  while (WiFi.waitForConnectResult()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Ok"); //Conexión establecida
  Serial.println(WiFi.localIP()); //Mostramos dirección IP asignada

  //Arrancamos el servidor
  server.begin();

  Serial.println("setup end");
}

void loop()
{
  //Anotamos si hay algún cliente
  WiFiClient client = server.available();
  //Si hay alguno...
  if(client)
  {
    Serial.println("Hay algún cliente conectado");

    //Mientras el cliente esté conectado...
    while (client.connected())
    {
      //Este es un ejemplo del formato de la información que podemos recibir:
        //GET /mypath HTTP/1.1
        //Content-type: text/plain
        //host: myserver.domain.com:8266
        //content-length: 49
        //x-newrelic-id: VwAOU1RRGGAFUFZUAwQE
        //x-newrelic-transaction: PxRVBVYCY1VTV1ZTAgJSVkYdUFIOFQZOEloMAVtcAwFWAwhSVFRQVFIUG0MEWwkGCVEFABVs
        //Connection: close
        //
        //Texto contenido en el mensaje
      //Sólo tendremos en cuenta valor del path de la cabecera del mensaje
      //Lo encontramos en la primera línea
      //Leeremos el mensaje hasta encontrar el prefijo al path y como medida de seguridad
      //incluimos el sufijo del path para no perder tiempo en leer el mensaje completo cuando hay errores
      //Si hemos encontrado el prefijo del path...
      if(_clientFindString(&client,1500,"GET /","HTTP/"))
      {
        //...leemos el path
        String path=_clientReadString(&client,1500," HTTP/");
        Serial.println("Path="+path);
        //Dependiendo del path tomaremos acciones
        //Si tenemos que encender...lo encendemos
        if(path=="on") digitalWrite(pinLed,ledON);
        //Si tenemos que apagar...lo apagamos
        else if(path=="off") digitalWrite(pinLed,!ledON);
        //No tenemos en cuenta ninguna otra opción de path
        //No tenemos en cuenta el resto del mensaje
        //Respondemos con la web (código html)
        client.println(webPage);
      }
      //Cortamos la conexión
      client.stop();
    }
    Serial.println("Cliente desconectado");
  }
}