/*
  Autor: Ramón Junquera
  Fecha: 20171231
  Tema: Comunicaciones wifi
  Objetivo: Crear un servidor web genérico que guarda los archivos en local
  Material: placa ESP8266

  Descripción:
  Ejemplo de servidor web genérico que almacena sus archivos en SPIFFS.
  Está basado en el ejemplo FSBrowser y contiene algunas modificaciones.

  Creamos un punto de acceso con seguridad.
  Creamos un servidor web.
  Cuando creamos un punto de acceso, el procesador tiene por defecto la
  dirección IP 192.168.4.1.
  Según se conectan más dispositivos, se les asignas IPs consecutivas.
  Todas las direcciones pertenecen a la misma clase C.

  Utilizaremos la librería DNSServer.h para evitar utilizar direcciones IP.
  Asignaremos un nombre al servidor para facilitar su localización.
  
  Nota:
  Antes de ejecutar el programa es necesario subir la carpeta data a SPIFFS.  
*/

#include <ESP8266WebServer.h> //Librería de gestión de wifi del ESP8266 (mejor que la estándar)
#include <ESP8266mDNS.h> //Librería de gestión de nombres locales
#include <FS.h> //Gestión de SPIFFS

#define DBG_OUTPUT_PORT Serial

//Nombre del punto de acceso
const char *ssid = "ESP";
//Contraseña para el punto de acceso
const char *password = "ESP2018ESP";

//Nombre que asignaremos al servidor
//Siempre se le debe añadir el sufijo .local
//Quedaría como http://esp.local
const char *myHostname = "esp";
//Mensaje html para responder cuando no se encuentre un archivo
//Es un mensaje que redirige a la página por defecto
const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"3; url=http://esp.local/index.html\" /></head><body><p>redirecting...</p></body>";

//Creamos una instancia de un servidor escuchando por el puerto 80 llamado server
ESP8266WebServer server(80);

//Creamos un objeto que MDNSResponder que nos permite asignar un nombre a la ip del
//dispositivo. Será más cómodo llamarlo.
MDNSResponder mdns;

String getContentType(String filename)
{
  //Devuelve el tipo de archivo dependiendo de la extensión
  
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path)
{
  //Gestiona la petición de un archivo
  
  Serial.println("handleFileRead: " + path);
  //Si el path finaliza con / (si no tiene archivo)...
  //...le añadiremos el nombre del archivo por defecto: index.html
  if(path.endsWith("/")) path +="index.html";
  //Obtenemos el tipo de archivo dependiendo de su extensión
  String contentType = getContentType(path);
  
  //Si existe el archivo...
  if(SPIFFS.exists(path))
  {
    //Abrimos el archivo como sólo lectura
    File file = SPIFFS.open(path, "r");
    //Enviamos el contenido como respuesta
    size_t sent = server.streamFile(file,contentType);
    //Cerramos el archivo
    file.close();
    //Se ha enviado correctamente
    return true;
  }
  //El archivo indicado no existe
  return false;
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  //Si no se ha podido inicializar correctamente el sistema de archivos...
  if (!SPIFFS.begin())
  {
    Serial.println("Error al inicializar el sistema de archivos");
    while(1) {yield();}; //Terminamos
  }
  Serial.println("Sistema de archivos inicializado correctamente");
  //Hemos podido inicializar el sistema de archivos

  //Modo punto de acceso
  WiFi.mode(WIFI_AP);
  //Asignamos nombre y contraseña al punto de acceso
  WiFi.softAP(ssid,password);

  //Intentamos iniciar el servicio de dns local
  //A nuestro dispositivo le asignaremos el nombre "esp.local"
  //Que estará asociado a la dirección ip que se le ha asignado
  //Siempre se la añade el dominio .local porque sólo sirve para dominios
  //locales. Como el WINS de Windows.
  //Si la asignación de nombre a ip se ha completado correctamente...
  if (mdns.begin("esp",WiFi.localIP()))
  {
    //...informamos
    Serial.println("Nombre asignado correctamente");
  }
  else
  {
    Serial.println("Error al asignar el nombre al servidor");
  }
  
  //Crearemos algunos eventos del servidor web que llamarán a distintas funciones

  //Aquí crearemos los eventos que llaman a funciones cuando se detecten ciertos paths
  //en la URL
  //Por ejemplo
  //server.on("/off",functionOff);

  //Cuando el path de la URL no coincide con ninguno de los procesados...
  server.onNotFound([](){
    //Si no podemos gestionar (entregar) el archivo solicitado...
    if(!handleFileRead(server.uri()))
      //...informamos. Error 302
      server.send(302,"text/html",metaRefreshStr);
  });
  
  //Iniciamos el servidor web
  server.begin();
  Serial.println("Servidor web iniciado");
}
 
void loop(void)
{
  //Sólo nos preocuparemos de que se gestionen las conexiones cliente que podamos recibir
  server.handleClient();
}
