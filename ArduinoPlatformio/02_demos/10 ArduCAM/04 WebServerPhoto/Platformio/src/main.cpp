/*
  Autor: Ramón Junquera
  Fecha: 20220108
  Tema: ArduCAM
  Objetivo: Crear un servidor web que permita sacar fotos
  Material: placa ESP8266

  Descripción:
  Creamos un servidor web que permita sacar fotografías.

  Creamos un punto de acceso con seguridad.
  Creamos un servidor web.
  Cuando creamos un punto de acceso, el procesador tiene por defecto la
  dirección IP 192.168.4.1.
  Según se conectan más dispositivos, se les asignas IPs consecutivas.
  Todas las direcciones pertenecen a la misma clase C.

  Utilizaremos la librería DNSServer.h para evitar utilizar direcciones IP.
  Asignaremos un nombre al servidor para facilitar su localización.
  http://esp.local
  
  Nota:
  Antes de ejecutar el programa es necesario subir la carpeta data a LittleFS.  
*/

#include <ESP8266WebServer.h> //Librería de gestión de wifi del ESP8266 (mejor que la estándar)
#include <ESP8266mDNS.h> //Librería de gestión de nombres locales
#include <LittleFS.h> //Gestión de LittleFS
#include <RoJoArduCAM.h> //Librería de gestión de la cámara


const char *ssid = "ESP"; //Nombre del punto de acceso
const char *password = "ESP2022ESP"; //Contraseña para el punto de acceso

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

RoJoArduCAM camera; //Creamos el objeto de gestión de la cámara
const byte pinCS=D0; //Definimos pin CS

//Tabla de resoluciones
const char *resTable[9]={"160x120","176x144","320x240","352x288","640x480","800x600","1024x768","1280x1024","1600x1200"};

//Devuelve el tipo de archivo dependiendo de la extensión
String getContentType(String filename) {
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

//Gestiona la petición de un archivo
bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  //Si el path finaliza con / (si no tiene archivo)...
  //...le añadiremos el nombre del archivo por defecto: index.html
  if(path.endsWith("/")) path +="index.html";
  //Obtenemos el tipo de archivo dependiendo de su extensión
  String contentType = getContentType(path);

  if(LittleFS.exists(path)) { //Si existe el archivo...
    File file = LittleFS.open(path, "r"); //Abrimos el archivo como sólo lectura
    server.streamFile(file,contentType); //Enviamos el contenido como respuesta
    file.close(); //Cerramos el archivo
    return true; //Se ha enviado correctamente
  }
  return false; //El archivo indicado no existe
}

//Se ha solicitado actualizar la imagen
void takePhoto() {
  //Solicitamos que se tome una foto con la resolición actual
  Serial.println("Tomando foto");
  camera.takePhoto();
  uint32_t resX,resY; //Resolución de la imagen guardada
  //Guardamos la foto en el archivo photo.jpg
  //Repetimos mientras tengamos algún error
  Serial.print("Guardando foto");
  while(camera.savePhoto("photo",&resX,&resY)) {
    delay(40);
    yield();
    Serial.print(".");
  }
  Serial.println(".done"); //Imagen guardada
  handleFileRead("/index.html"); //Enviar de nuevo la página principal
}

//Escribe el archivo de home con el botón de la resolución indicada en otro color
void writeIndex(byte res) {
  File file=LittleFS.open("/index.html","w");
  file.print("<!DOCTYPE HTML><html><h1>ESP Photo Web Server</h1><p><a href=""photo""><button>Foto</button></a>&nbsp;<a href=""index.html""><button>Refresh</button></a><p>Resolucion<p>");
  for(byte i=0;i<=8;i++) {
    file.print("<a href=""" + String(i) + """><button");
    if(i==res) file.print(" style=""background-color:yellow""");
    file.print(">" + String(resTable[i]) + "</button></a>&nbsp;");
  }
  file.print("<p><img src=""photo.jpg"" /></html>");
  file.close();
}

//El path no coincide con ninguno de los esperados
void pathNotMatch() {
  //En vez de crear un evento nuevo para cada resolución, analizamos aquí
  //si el path coincide con alguna de las resoluciones permitidas
  String p=server.uri();
  if(p.length()==2) {
    if(p[0]=='/') {
      char r=p[1];
      if(r>='0' && r<='8') {
        //Tenemos un comando de cambio de resolución válido
        //Pasamos el carácter a valor numérico
        byte res=r-48;
        //Cambiamos a la resolución indicada
        Serial.println("Cambiando a resolución " + String(res));
        camera.setResolution(res);
        //Reescribimos el archivo index.html en el que resaltaremos el
        //botón con la resolución indicada
        writeIndex(res);
        //Enviar de nuevo la página principal
        handleFileRead("/index.html");
        //Hemos terminado
        return;
      }
    }
  }

  //Si no podemos gestionar (entregar) el archivo solicitado...redireccionamos a home
  if(!handleFileRead(p)) server.send(302,"text/html",metaRefreshStr);
}

void setup(void) {
  Serial.begin(115200);
  while(!Serial) delay(1);
  Serial.println();
  //Si no se ha podido inicializar correctamente el sistema de archivos...
  if (!LittleFS.begin()) {
    Serial.println("Error al inicializar el sistema de archivos");
    while(1) yield(); //Terminamos en bucle infinito
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
  //Si la asignación de nombre a ip se ha completado correctamente...informamos
  if (mdns.begin("esp",WiFi.localIP())) Serial.println("Nombre asignado correctamente");
  else Serial.println("Error al asignar el nombre al servidor");

  //Inicializamos la cámara con el pin CS conectado al D0
  byte errorCode = camera.begin(pinCS);
  //Si tenemos algún error en la inicialización...
  if(errorCode) {
    Serial.println("Error " + String(errorCode) + " al inicializar la cámara");
    while(1) yield(); //Terminamos con bucle infinito
  }
  Serial.println("Camara inicializada correctamente");
  //Hemos inicializado la cámara correctamente
  //Por defecto arranca en modo jpg con la resolución 2 = 320x240
  //Escribimos el arhivo de home (index.html) con el la resolución 2 resaltada
  writeIndex(2);
  
  //Crearemos algunos eventos del servidor web que llamarán a distintas funciones

  //Si se solicita sacar una nueva foto...
  server.on("/photo",takePhoto);

  //Cuando el path de la URL no coincide con ninguno de los procesados...
  server.onNotFound(pathNotMatch);
  
  //Iniciamos el servidor web
  server.begin();
  Serial.println("Servidor web iniciado");
}
 
void loop(void) {
  //Sólo nos preocuparemos de que se gestionen las conexiones cliente que podamos recibir
  server.handleClient();
}
