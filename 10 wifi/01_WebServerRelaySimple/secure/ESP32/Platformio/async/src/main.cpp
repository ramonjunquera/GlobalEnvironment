/*
  Autor: Ramón Junquera
  Fecha: 20190515
  Descripción:
    Ejemplo de gestión de servidor web seguro (SSL)
    La información de entrada la procesaremos según se vaya recibiendo (byte a byte).
    El proceso de la información será asíncrono: cada nueva conexión detectada será
    gestionada por un proceso distinto que correrá en paralelo al programa principal.

    El programa principal siempre se ejecuta en el core 1.
    Lanzaremos todos los procesos de gestión de conexión al core 0.

    Cuando se detecta una nueva conexión cliente, se lanza un proceso en el core 0.
    El proceso gestina la conexión en paralelo con el programa principal y con otros
    posibles procesos similares.
    No necesitamos referenciar al proceso una vez lanzado. Cuando finaliza se elimina
    a si mismo.
    Antes de hacerlo, liberamos la memoria del objeto de conexión para evitar memory leak.
*/

#include <Arduino.h>
#include <RoJoWiFiServerSecure32.h>

//Definimos los detalles del punto de acceso
const char* wifiClientSSID = "xxx";
const char* wifiClientPassword = "xxx";
//Archivo de certificado
const char* secureServerCertificateFile="/autocert.crt";
//Archivo de clave privada
const char* secureServerPriveteKeyFile="/private.key";
//Puerto de escucha del servidor
const uint16_t port=443;
//Definimos el pin del led
const byte pinLed=LED_BUILTIN;
//Definimos cuándo está encendido
const bool ledON=false;
 
//Creamos un servidor wifi seguro en el puerto indicado
RoJoWiFiServerSecure32 server(port);

String html()
{
  //Devuelve el texto html de la página web que devolvemos
  //Una cabecera, el estado actual del led y 3 botones de ON, OFF y ESTADO

  String answer = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection:close\n\n<!doctype html><html><h1>ESP32 Simple Secure Web Server</h1><p>Estado: ";
  answer+=digitalRead(pinLed)==ledON?"encendido":"apagado";
  answer+="<p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a>&nbsp;<a href=\"/\"><button>ESTADO</button></a></p></html>";
  return answer;
}

void manageConn(void *parameter)
{
  //Función llamada por un proceso que gestiona una conexión cliente
  //El parámetro es un puntero a una conexión cliente

  //Obtenemos el parámetro en formato correcto
  RoJoWiFiClientSecure32 *client=(RoJoWiFiClientSecure32 *)parameter;

  //Este es un ejemplo del formato de la información que podemos recibir:
    //GET /mypath HTTP/1.1
    //Content-type: text/plain
    //host: myserver.domain.com
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

  //Si encontramos el prefijo del path...
  if(client->readFindString("GET /","HTTP/"))
  {
    //...leemos el path
    String path=client->readString(" HTTP/");
    //Mostramos el path detectado
    Serial.println("Path="+path);
    //Dependiendo del path tomaremos acciones
    //Si tenemos que encender...lo encendemos
    if(path=="on") digitalWrite(pinLed,ledON);
    //Si tenemos que apagar...lo apagamos
    else if(path=="off") digitalWrite(pinLed,!ledON);
    //No tenemos en cuenta ninguna otra opción de path
    //No tenemos en cuenta el resto del mensaje
  }
    
  //Respondemos con la misma web
  client->write(html());
  //Cortamos la conexión
  client->stop();
  //Borramos el objeto cliente
  delete client;

  //Hemos terminado. Borramos la tarea
  vTaskDelete(NULL);
}

void setup()
{ 
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Modo conexión a punto de acceso
  WiFi.begin(wifiClientSSID,wifiClientPassword); //Solicitamos conexión

  Serial.print("Connecting.");
  while (WiFi.waitForConnectResult()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Ok"); //Conexión establecida
  Serial.println(WiFi.localIP()); //Mostramos dirección IP asignada

  //El pin del led es de salida
  pinMode(pinLed,OUTPUT);
  //Inicialmente está apagado
  digitalWrite(pinLed,!ledON);

  //Arrancamos el servidor
  byte errorCode=server.begin(secureServerCertificateFile,secureServerPriveteKeyFile);
  Serial.println("Server started with error: "+String(errorCode));
}

void loop(void)
{
  //Anotamos si hay algún cliente
  RoJoWiFiClientSecure32 *client=server.available();
  //Si tenemos algún cliente...
  if(client)
  {
    //Lanzamos la tarea
    //- manageConn: nombre de la función
    //- "": nombre de la tarea (no importa)
    //- 2000: tamaño de pila
    //- (void*)&param[t]: puntero a parámetro
    //- 1: prioridad baja
    //- NULL: puntero del objeto de tarea (no importa porque se borra sola)
    //- 0: core en el que se ejecuta. Usaremos el que no tiene el programa principal
    xTaskCreatePinnedToCore(manageConn,"",2000,(void*)client,1,NULL,0);
  }
}
