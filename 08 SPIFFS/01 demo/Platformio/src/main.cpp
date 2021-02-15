/*
  Autor: Ramón Junquera
  Fecha: 20190716
  Tema: SPIFFS
  Objetivo: Tests
  Material: placa ESP

  Descripción:
  SPIFFS es un sistema de almacenamiento de archivos propio de los dispositivos ESP.
  No se permiten crear carpetas.
  Un archivo con nombre /folder1/name.txt no estaría metido en la carpeta folder1, sino
  que todo ello serái el nombre del archivo.
  
  Aunque la todas las placas ESP utilizan el objeto SPIFFS para gestionar el espacio
  de archivos integrado en el microprocesador, éste se guarda en librerías distintas:
  ESP8266 -> FS.h
  ESP32   -> SPIFFS.h

  Se presentan varios ejemplos de uso.

  El sistema de listado de archivos difiere de ESP32 a ESP8266.

  Nota:
  Inicialmente es necesario formatearla.
  Sólo se debe hacer una vez.
  Es suficiente con descomentar la línea correspondiente.
  
  Resultado:
  Ejemplos de funcionalidades del sistema de archivos
*/

#include <Arduino.h>
#ifdef ESP32 //Si es un ESP32...
  #include <SPIFFS.h> 
#elif defined(ESP8266) //Si es un ESP8266...
  #include <FS.h>
#else
  #error Device unknown
#endif

//Muestra los archivos que contiene SPIFFS
void listDir()
{
  //Depende del procesador
  Serial.println("\nListing directory:");
  #ifdef ESP32 //Si es un ESP32...
    //En ESP32 no es necesario el parámetro de modo de apertura. Si no se indica se supone que es "r"
    //Aquí lo incluimos para guardar compatibilidad con ESP8266
    File root = SPIFFS.open("/","r");
    if (!root)
    {
      Serial.println("Failed to open directory");
      return;
    }
    File file = root.openNextFile();
    while (file)
    {
      Serial.println(String(file.name())+" ("+String(file.size())+")");
      file = root.openNextFile();
    }
    file.close();
  #else //Si es un ESP8266...
    Dir dir = SPIFFS.openDir("/");
    while(dir.next()) Serial.println(String(dir.fileName())+" ("+String(dir.fileSize())+")");
  #endif
}

void writeFile(String fileName,String message)
{
  Serial.println("\nWriting file: "+fileName+", with: "+message);

  File file = SPIFFS.open(fileName,"w");
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) Serial.println("File written");
  else Serial.println("Write failed");
  file.close();
}

void appendFile(String fileName,String message)
{
  Serial.println("\nAppending to file: "+fileName+", with: "+message);

  File file = SPIFFS.open(fileName,"a");
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) Serial.println("Message appended");
  else Serial.println("Append failed");
  file.close();
}

void readFile(String fileName)
{
  Serial.println("\nReading file: "+fileName);

  //En ESP32 no es necesario el parámetro de modo de apertura. Si no se indica se supone que es "r"
  //Aquí lo incluimos para guardar compatibilidad con ESP8266
  File file = SPIFFS.open(fileName,"r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  while (file.available()) Serial.write(file.read());
  file.close();
  Serial.println();
}

void deleteFile(String fileName)
{
  Serial.println("\nDeleting file: "+fileName);
  if (SPIFFS.remove(fileName)) Serial.println("File deleted");
  else Serial.println("Delete failed");
}

void renameFile(String fileName1,String fileName2)
{
  Serial.println("\nRenaming file "+fileName1+" to "+fileName2);
  if (SPIFFS.rename(fileName1,fileName2)) Serial.println("File renamed");
  else Serial.println("Rename failed");
}

void setup()
{
  //Abrimos puerto serie para mostrar los mensajes de debug
  Serial.begin(115200);
  delay(5000);

  //Sólo la primera vez que se utiliza es necesario formatearlo
  //Tarda un mínimo de 30 segundos
  //SPIFFS.format();

  //Si no se puede montar el sistema de archivos... 
  if (!SPIFFS.begin())
  {
    Serial.println("Error al montar sistema de archivos");
    return; //Terminamos función setup() y pasamos a loop()
  }
  Serial.println("SPIFFS ok");
  
  listDir();
  writeFile("/hello.txt","Hello ");
  listDir();
  readFile("/hello.txt");
  appendFile("/hello.txt","World!");
  readFile("/hello.txt");
  deleteFile("/foo.txt"); //No se puede borrar un archivo inexistente
  renameFile("/hello.txt", "/foo.txt");
  listDir();
  readFile("/foo.txt");
  deleteFile("/foo.txt");
  listDir();
  
  SPIFFS.end();
}

void loop()
{
  //Nada especial que hacer aquí
  delay(1);
}