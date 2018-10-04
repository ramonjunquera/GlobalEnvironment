/*
  Autor: Ramón Junquera
  Fecha: 20181004
  Tema: Sistema de archivos de ESP32
  Objetivo: Descripción y lectura
  Material: placa ESP32

  Descripción:
  Como cualquier placa ESP, la familia ESP32 también tiene memoria para ser utilizada
  como sistema de archivos.
  En ESP8266 se gestiona a través del objeto SPIFFS que se crea al incluir la 
  librería FS.h.
  En ESP32 el objeto SPIFFS se crea al incluir la librería SPIFFS.h.
  
  El desarrollo actual es una modificación del ejemplo original.
  En él se puede comprobar cómo se pueden utilizar la mayoría de funcionalidades para la
  gestión del sistema de archivos.

  Nota: Inicialmente o al pasar a utilizar la librería oficial es necesario formatearla.
  Sólo se debe hacer una vez.
  Es suficiente con descomentar la línea correspondiente.
  
  Resultado:
  Ejemplos de funcionalidades del sistema de archivos
*/

#include <Arduino.h>
#include <SPIFFS.h> //Librería de gestión de SPIFFS para ESP32

void listDir(const char * dirname, uint8_t levels=0)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SPIFFS.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  file.close();
}

void createDir(const char * path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (SPIFFS.mkdir(path)) Serial.println("Dir created");
  else Serial.println("mkdir failed");
}

void removeDir(const char * path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (SPIFFS.rmdir(path)) Serial.println("Dir removed");
  else Serial.println("rmdir failed");
}

void writeFile(const char * path, const char * message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = SPIFFS.open(path,"w");
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) Serial.println("File written");
  else Serial.println("Write failed");
  file.close();
}

void appendFile(const char * path, const char * message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = SPIFFS.open(path,"a");
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) Serial.println("Message appended");
  else Serial.println("Append failed");
  file.close();
}

void readFile(const char * path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = SPIFFS.open(path);
  //Los archivos por defecto se abren como sólo lectura. Es lo mismo que:
  //File file = fs.open(path,"r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
  Serial.println();
}

void deleteFile(const char * path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (SPIFFS.remove(path)) Serial.println("File deleted");
  else Serial.println("Delete failed");
}

void renameFile(const char * path1,const char * path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (SPIFFS.rename(path1, path2)) Serial.println("File renamed");
  else Serial.println("Rename failed");
}

void setup()
{
  //Abrimos puerto serie para mostrar los mensajes de debug
  Serial.begin(115200);

  //Sólo la primera vez que se utiliza es necesario formatearlo
  //Tarda aproximadadmente 30 segundos
  //SPIFFS.format();

  //Si no se puede montar el sistema de archivos... 
  if (!SPIFFS.begin())
  {
    Serial.println("Error al montar sistema de archivos");
    return; //Terminamos función setup() y pasamos a loop()
  }
  
  listDir("/");
  createDir("/mydir");
  listDir("/");
  removeDir("/mydir");
  listDir("/");
  writeFile("/hello.txt","Hello ");
  appendFile("/hello.txt", "World!\n");
  readFile("/hello.txt");
  deleteFile("/foo.txt"); //No se puede borrar un archivo inexistente
  renameFile("/hello.txt", "/foo.txt");
  readFile("/foo.txt");
  deleteFile("/foo.txt");
  listDir("/");
  SPIFFS.end();
}

void loop()
{

}
