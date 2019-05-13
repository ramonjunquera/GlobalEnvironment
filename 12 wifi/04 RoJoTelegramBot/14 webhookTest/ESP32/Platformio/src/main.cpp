/*
  Pendiente:
  - Asigna IP fija y mostrarla

  Tema: webhook en Telegram
  Versión: 20190512
  Autor: Ramón Junquera
  Material: ESP32
  Descripción:
    Ejemplo para testear webhook de Telegram
    Creremos un servidor seguro para recepcionar las llamadas
    webhook que genera el servidor de Telegram cuando nuestro
    bot recibe un nuevo mensaje.
  Requisitos previos:
    - Generar una clave privada y un certificado de al menos 1024bits
    - Subir los archivos a SPIFFS para que queden disponibles
      para la librería
    - Crear una dirección dns dinámica para nuestra dirección
      pública de Internet
    - Asignar una dirección ip fija a nuestro dispositivo
      (seleccionable por código)
    - Redirigir el puerto utilizado desde el router a nuestro
      dispositivo
    - Creat un bot de Telegram
    - Activar su webhook a la dirección pública de nuestro
      servidor y al puerto elegido
  Funcionamiento:
    Se enviará al puerto serie la información de apertura de 
    sesiones y su contenido íntegro.
    Si se detecta como cabecera "Content-Length", se intentará
    leer su valor. Se intentará localizar el final de las cabeceras.
    Se leerá el contenido del mensaje y se enviará la confirmación
    de recepción.
  Notas:
    - No utilizaremos multitarea. Las conexiones serán síncronas.
      Por eso se debe limitar el número de webhook concurrentes
      a uno.
*/

#include <Arduino.h>
#include <RoJoWiFiServerSecure32.h>

//Definimos los detalles del punto de acceso
const char* wifiClientSSID = "xxx";
const char* wifiClientPassword = "xxx";
//Archivo de certificado
const char* secureServerCertificateFile="/autocert.crt";
//Archivo de clave privada
const char* secureServerPrivateKeyFile="/private.key";
//Puerto de escucha del servidor
const uint16_t port=8443;
//Cadena de confirmación de recepción
const String receivedOK="HTTP/1.1 200 OK\r\n\r\n";
//Dirección ip fija
const IPAddress fixIP=IPAddress(192,168,1,105);
 
//Creamos un servidor wifi seguro en el puerto indicado
//Dejaremos los valores por defecto correspondientes a
//máximo número de conexiones simultáneas (por defecto 1)
//y tiempo de timeout
RoJoWiFiServerSecure32 server(port);

bool _printRead(RoJoWiFiClientSecure32 *client,byte *b)
{
  //Sólo llama al método read de RoJoWiFiClientSecure32
  //y devuelve su resultado.
  //Pero si el resultado es correcto, lo envía al puerto serie

  //Leemos el siguiente byte de la conexión cliente y
  //anotamos su resultado
  bool answer=client->read(b);
  //Si se ha podido leer...lo enviamos por el puerto serie
  if(answer) Serial.print((char)*b);
  //Devolvemos si se ha podido leer el siguiente byte
  return answer;
}

bool _clientFindString(RoJoWiFiClientSecure32 *client,String findText,String endText="")
{
  //Lee caracteres del cliente hasta encontrar la cedena findText o
  //detectar la cadena endText.
  //El parámetro endText es optativo
  //La longitud de los parámetros findText y endText puede ser distinta
  //Devuelve true si ha encontrado la cadena

  //Buffer de texto recibido
  String buffer="";
  //Carácter leido
  byte b;
  //Calculamos el tamaño del buffer
  byte bufferLen=findText.length();
  if(endText.length()>bufferLen) bufferLen=endText.length();

  //Inicializamos el buffer a su tamaño correcto
  for(byte c=0;c<bufferLen;c++) buffer+="@";
  //Mientras hayamos podido leer un caráter...
  while(_printRead(client,&b))
  {
    //Añadimos el carácter leido al buffer
    buffer=buffer.substring(1)+(char)b;
    //Si el final del buffer coincide con la cadena que estamos buscando...lo hemos encontrado!
    if(buffer.substring(bufferLen-findText.length())==findText) return true;
    //Si tenemos cadena de fin...
    if(endText.length())
    {
      //Si el final del buffer coincide con la cadena de fin...no lo hemos encontrado
      if(buffer.substring(bufferLen-endText.length())==endText) return false;
    }
  }
  //Hemos procesado todo el texto disponible y no lo hemos encontrado
  return false;
}

uint32_t _clienParseInt(RoJoWiFiClientSecure32 *client)
{
  //Parsea caracteres numéricos
  //Se detiene cuando encuentra un carácter no numérico o cuando
  //se han leido todos los datos pendientes
  
  //Variable en la que guardaremos la respuesta
  uint32_t answer=0;
  //Carácter leido
  byte b;

  //Mientras hayamos podido leer un carácter...
  while(_printRead(client,&b))
  {
    //Si es un carácter numérico...
    if(b>='0' && b<='9')
    {
      //...lo tomamos como correcto lo añadimos a la respuesta
      //El código ASCII del '0' es el 48
      answer=answer*10+b-48;
    }
    else //No es un carácter numérico
    {
      //Devolvemos el resultado
      return answer;
    }
  }
  //Hemos leido todos los caracteres pendientes.
  //Devolvemos lo que hayamos podido encontrar
  return answer;
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

  //Asignamos una dirección IP fija para poder localizarlo
  WiFi.config(fixIP,WiFi.gatewayIP(),WiFi.subnetMask());

  Serial.println(WiFi.localIP()); //Mostramos dirección IP asignada

  //Arrancamos el servidor
  byte errorCode=server.begin(secureServerCertificateFile,secureServerPrivateKeyFile);
  Serial.println("Server started with error: "+String(errorCode));
}

void loop(void)
{
  //Anotamos si hay algún nuevo cliente
  RoJoWiFiClientSecure32 *client=server.available();
  //Si tenemos alguno...
  if(client)
  {
    /* Ejemplo de formato de mensaje recibido
POST / HTTP/1.1
Host: myserver.mydomain
Content-Type: application/json
Content-Length: 326
Connection: keep-alive
Accept-Encoding: gzip, deflate

{"update_id":500150329,
"message":{"message_id":10606,"from":{"id":3396975,"is_bot":false,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","language_code":"es"},"chat":{"id":3396975,"first_name":"Ramon","last_name":"Junquera","username":"ramonjunquera","type":"private"},"date":1557012314,"text":"Hola"}}
    */

    byte b; //Byte leido
    uint16_t len=0; //Tamaño de los datos recibidos

    Serial.println("------start------"+String(millis()));
    //Buscamos la etiqueta del tamaño de datos en las cabeceras
    //Si no la encontramos, no miraremos más que el principio
    //de los datos.
    //Si la encontramos...
    if(_clientFindString(client,"Content-Length: ","{"))
    {
      //Leemos el valor de tamaño de datos
      len=_clienParseInt(client);
      //Buscamos el final de las cabeceras
      _clientFindString(client,"\n\r\n");
      //Leemos todos los datos
      for(uint16_t i=0;i<len;i++)
      {
        //Si tenemos menos datos de los esperados...no miramos más
        if(!_printRead(client,&b)) break;
      }
      //Hemos leido todos los datos
    }
    Serial.println("\n------end------"+String(len));
    //Si hemos recibido algo...damos confirmación de recepción
    if(len>0) client->write(receivedOK);
    //Cortamos la conexión
    client->stop();
    //Borramos el objeto cliente
    delete client;
  }
}
