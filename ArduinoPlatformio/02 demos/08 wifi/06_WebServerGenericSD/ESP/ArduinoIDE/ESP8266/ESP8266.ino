/*
  Autor: Ramón Junquera
  
  Tema: Lector SD
  
  Objetivo: Aprender a utilizar el adaptador de tarjetas micro SD
  
  Material adicional: breadboard, cables, módulo WaveShare MicroSD Card, ESP-12E, resistencia de 10 Kohmios

  Descripción:
  Aprovechando que sabemos utilizar un lector de tarjetas SD conectado a nuestro ESP-12E, iremos un poco 
  más lejos. Utilizaremos la SD como almacenamiento de la página web a publicar a través de la conexión
  WiFi.
  La SD albergará tanto el archivo principal (index.htm) como todas las referencias que pueda contener
  (imágenes, vídeos, archivos css, etc).
  El programa se encarga de atender todas las peticiones, seleccionar el archivo correcto, leerlo y
  enviarlo.

  El programa es una variación de:
  https://github.com/esp8266/Arduino/blob/d218c4ead3df50ac9cbdfa7144698850a03f2066/hardware/esp8266com/esp8266/libraries/ESP8266WebServer/examples/SDWebServer/SDWebServer.ino
  Las diferencias con el programa original son:
  - Se han añadido comentarios para facilitar su comprensión.
  - Se han eliminado las referencias al mDNS porque daban errores de redundacia de definición de variables.
  - Se ha corregido el error del pin CD del lector SD.
  - Simplificaciones varias para evitar nomenclaturas y variables innecesarias

  El ciruito es idéntico al ejercicio anterior.
  
  Puesto que en la tarjeta SD de deben guardar todos los elementos que contiene la página web, se incluye
  en el proyecto una carpeta llamada SD con un ejemplo de una página que contiene una foto.

  Resultado:
  El servidor web se basa en el contenido de la tarjeta SD
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <SD.h>

//Definición de constantes globales
const char* ssid = "xxx"; //Nombre del punto de acceso al que nos conectaremos
const char* password = "xxx"; //Contraseña del punto de acceso

//Definición de variables globales
ESP8266WebServer server(80); //Crearemos un servidor que escuche el puerto 80/tcp
static bool hasSD = false;
File uploadFile; //Variable de archivo con la que hacemos las lecturas

void returnOK()
{
  //Indica que la conexión va a finalizar correctamente. No hay ningún mensaje de error que enviar
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "");
}

void returnFail(String msg)
{
  //Indica que la conexión va a finalizar incorrectamente. Hay un mensaje de error que enviar
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSdCard(String path)
{
  //Envía el archivo indicado
  //Devuelve false ante cualquier error

  //Inicialmente suponemos que el archivo será de texto
  String dataType = "text/plain";

  //Si el path finaliza con /...
  //Esto ocurre cuando no se ha indicado el elemento a leer
  if(path.endsWith("/"))
  {
    //...cuando no se indica un elemento tomaremos por defecto el archivo index.htm
    path += "index.htm";
  }

  //Si el archivo tiene extensión .src...
  if(path.endsWith(".src")) 
  {
    //...le quitaremos la extensión
    path = path.substring(0, path.lastIndexOf("."));  
  }
  else
  {
    //Si el archivo tiene extensión .htm
    if(path.endsWith(".htm")) 
    {
      //...se trata de un archivo de tipo html
      dataType = "text/html";
    }
    else
    {
      //Si el archivo tiene extensión .css
      if(path.endsWith(".css"))
      {
        //..se trata de un archivo de tipo css (estilos)
        dataType = "text/css";
      }
      else
      {
        //Si el archivo tiene extensión .js...
        if(path.endsWith(".js"))
        {
          //...se trata de un archivo de javascript
          dataType = "application/javascript";
        }
        else
        {
          //Si el archivo tiene extensión .png...
          if(path.endsWith(".png"))
          {
            //...se trata de un archivo gráfico
            dataType = "image/png";
          }
          else
          {
            //Si el archivo tiene extensión .gif...
            if(path.endsWith(".gif"))
            {
              //...se trata de un archivo gráfico
              dataType = "image/gif";
            }
            else
            {
              //Si el archivo tiene extensión .jpg...
              if(path.endsWith(".jpg"))
              {
                //...se trata de un archivo gráfico
                dataType = "image/jpeg";
              }
              else
              {
                //Si el archivo tiene extensión .ico...
                if(path.endsWith(".ico"))
                {
                  //...se trata de un icono
                  dataType = "image/x-icon";
                }
                else
                {
                  //Si el archivo tiene extensión .xml...
                  if(path.endsWith(".xml"))
                  {
                    //...se trata de un archivo de xml
                    dataType = "text/xml";
                  }
                  else
                  {
                    //Si el archivo tiene extensión .pdf...
                    if(path.endsWith(".pdf"))
                    {
                      //...se trata de un documento pdf
                      dataType = "application/pdf";
                    }
                    else
                    {
                      //Si el archivo tiene extensión .zip...
                      if(path.endsWith(".zip"))
                      {
                        //...se trata de un archivo comprimido
                        dataType = "application/zip";
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  //Abrimos el archivo indicado
  File dataFile = SD.open(path.c_str());
  //Si descubrimos que el archivo es realmente una carpeta...
  if(dataFile.isDirectory())
  {
    //...le añadimos la barra y el nombre del archivo por defecto
    path += "/index.htm";
    //Además supondremos que se trata de un archivo de html
    dataType = "text/html";
    //Y volvemos a abrir el archivo
    dataFile = SD.open(path.c_str());
  }

  //Si no se ha podido abrir el archivo...
  if (!dataFile)
  {
    //...hemos terminado. Devolvemos un error
    return false;
  }
  
  if (server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  //Si el archivo tiene un tamaño distinto del que esperábamos...
  //Se envía el archivo y si el número de bytes enviados no coincide con el tamaño del archivo...
  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
    //...es que no se ha podido enviar el archivo entero. Informamos del error.
    Serial.println("Sent less data than expected!");
  }

  //Hemos terminado con el archivo. Lo cerramos
  dataFile.close();
  //Todo ha salido bien
  return true;
}

void handleFileUpload()
{
  if(server.uri() != "/edit")
  {
    return;
  }
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START)
  {
    if(SD.exists((char *)upload.filename.c_str()))
      {
        SD.remove((char *)upload.filename.c_str());
      }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    Serial.print("Upload: START, filename: ");
    Serial.println(upload.filename);
  }
  else
  {
    if(upload.status == UPLOAD_FILE_WRITE)
    {
      if(uploadFile)
      {
        uploadFile.write(upload.buf, upload.currentSize);
      }
      Serial.print("Upload: WRITE, Bytes: ");
      Serial.println(upload.currentSize);
    }
    else
    {
      if(upload.status == UPLOAD_FILE_END)
      {
        if(uploadFile) uploadFile.close();
        Serial.print("Upload: END, Size: ");
        Serial.println(upload.totalSize);
      }
    }
  }
}

void deleteRecursive(String path)
{
  //Borra recursivamente todos el contenido a partir de un path de la tarjeta SD

  //Creamos la variable de archivo y le asignamos el path indicado
  File file = SD.open((char *)path.c_str());
  //Si el path indicado es una carpeta en vez de un archivo...
  if(!file.isDirectory())
  {
    //...cerramos el archivo
    file.close();
    //Eliminamos la carpeta
    SD.remove((char *)path.c_str());
    //Hemos terminado
    return;
  }
  //El path apunta a un archivo

  //Hacemos que la variable de archivo apunte a la primera entrada de la carpeta
  file.rewindDirectory();
  //Repetimos indefinidamente...
  while(true)
  {
    //Tomamos nota de la siguiente entrada de la carpeta
    File entry = file.openNextFile();
    //Si no hay más entradas...
    if (!entry)
    {
      //Hemos terminado. Salimos del bucle
      break;
    }
    //Añadimos el nombre de la entrada al path indicado
    String entryPath = path + "/" +entry.name();
    //Si la entrada en una carpeta...
    if(entry.isDirectory())
    {
      //Cerramos la entrada
      entry.close();
      //Y llamamos de nuevo a ésta misma función con el path nuevo para que la borre
      deleteRecursive(entryPath);
    }
    else
    {
      //La entrada es un archivo
      //Cerramos la entrada
      entry.close();
      //Eliminamos el archivo de la SD
      SD.remove((char *)entryPath.c_str());
    }
    //Para que no se sature el sistema, se entrega el control temporalmente para que re realicen las
    //tareas de fondo
    yield();
  }

  //Hemos consegido vaciar la carpeta del path indicado
  //Borramos la carpeta
  SD.rmdir((char *)path.c_str());
  //Cerramos el archivo
  file.close();
}

void handleDelete()
{
  //Borra el path indicado en el servidor en la SD
  
  //Si el servidor no tiene argumentos...
  if(server.args() == 0)
  {
    //Informamos del error
    returnFail("BAD ARGS");
    //Hemos terminado
    return;
  }
  //El servidor tiene algún argumento. Tomamos nota del primero
  String path = server.arg(0);
  //Si el path es raíz o no existe el path...
  if(path == "/" || !SD.exists((char *)path.c_str()))
  {
    //...en ningún caso se podrá eliminar. Error. Lo notificamos
    returnFail("BAD PATH");
    //Hemos terminado
    return;
  }
  //Pedimos que se borre todo el contenido del path indicado
  deleteRecursive(path);
  //Todo ha salido bien. Fin de conexión
  returnOK();
}

void handleCreate()
{
  //Crea un archivo en la SD

  //Si el servidor no tiene argumentos...
  if(server.args() == 0)
  {
    //Informamos del error
    returnFail("BAD ARGS");
    //Hemos terminado
    return;
  }
  //El servidor tiene algún argumento. Tomamos nota del primero
  String path = server.arg(0);
  //Si el path es raíz o no existe el path...
  if(path == "/" || SD.exists((char *)path.c_str()))
  { 
    //...en ningún caso se podrá eliminar. Error. Lo notificamos
    returnFail("BAD PATH");
    //Hemos terminado
    return;
  }
  //Si el path contiene un punto...
  if(path.indexOf('.') > 0)
  {
    //..es porque se trata de un archivo.
    //Abrimos un archivo para escrituta
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    //Si hemos podido crear el archivo...
    if(file)
    {
      //...escribimos su contenido
      file.write((const char *)0);
      //Lo cerramos
      file.close();
    }
  }
  else //El path no contiene un punto
  {
    //Se trata de una carpeta
    //Creamos la carpeta en la SD
    SD.mkdir((char *)path.c_str());
  }
  //Toho ha salido bien
  returnOK();
}

void printDirectory()
{
  if(!server.hasArg("dir"))
  {
    returnFail("BAD ARGS");
    return;
  }
  String path = server.arg("dir");
  if(path != "/" && !SD.exists((char *)path.c_str()))
  {
    returnFail("BAD PATH");
    return;
  }
  File dir = SD.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory())
  {
    dir.close();
    returnFail("NOT DIR");
    return;
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");
  WiFiClient client = server.client();
  
  server.sendContent("[");
  for (int cnt = 0; true; ++cnt)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      break;
    }

    String output;
    if (cnt > 0)
    {
      output = ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\"";
    output += "}";
    server.sendContent(output);
    entry.close();
 }
 server.sendContent("]");
 dir.close();
}

void handleNotFound()
{
  //Gestiona una solicitud desconocida

  //Si la SD está conectada y el path existe...
  if(hasSD && loadFromSdCard(server.uri()))
  {
    //...todo parece correcto. No se trata de una solicitud desconocida. Hemos terminado
    return;
  }
  //Componemos el mensaje de error
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++)
  {
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print(message);
}

void setup(void)
{
  //Abrimos el puerto serie
  Serial.begin(115200);
  //Intentamos conectar con el punto de acceso WiFi
  WiFi.begin(ssid, password);
  //Informamos que estamos intentando conectar
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  //Esperamos un máximo de 0.5*20=10 segundos a conectar
  //Pero comprobamos si tenemos conexión cada medio segundo
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20)
  {
    //wait 10 seconds
    delay(500);
  }
  //Si el contador ha llegado al máximo previsto...
  if(i == 21)
  {
    //...es porque no hemos podido conectar. Informamos
    Serial.print("Could not connect to");
    Serial.println(ssid);
    //Y como esto es lo más importante, no hacemos nada más
    //Nos quedamos aquí para siempre
    while(1)
    {
      delay(500);
    }
  }
  //Hemos podido conectar con el punto de acceso
  //Informamos de nuestra dirección IP
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  //Dependiendo de la petición recibida, definimos las funciones a las que llamamos
  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on("/edit", HTTP_POST, [](){ returnOK(); });
  //Si la petición es desconocida, también definimos la función que lo gestionará
  server.onNotFound(handleNotFound);
  //Si se solicita subir un archivo, definimos la función que lo gestionará
  server.onFileUpload(handleFileUpload);
  
  //Arrancamos el servidor e informamos
  server.begin();
  Serial.println("HTTP server started");

  //Si hemos detectado el lector de tarjetas SD...
  if (SD.begin(SS))
  {
    //...informamos
     Serial.println("SD Card initialized.");
     //Tomamos nota de que existe
     hasSD = true;
  }
}

void loop(void){
  //Gestionamos las peticiones del servidor
  server.handleClient();
}

