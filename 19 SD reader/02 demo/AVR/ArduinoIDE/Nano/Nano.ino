/*
  Autor: Ramón Junquera
  Tema: Tarjeta SD
  Versión: 20180123
  Objetivo: Aprender a utilizar escribir y leer en tarjetas SD
  Material: breadboard, cables, lector SD, Arduino Nano
  Descripción:
  Creamos funciones específicas para cada una de las acciones habituales.
  Las funciones informan de su estado por el puerto serie.

  La lista completa de comandos que admite esta librería se puede consultar en:
  https://www.arduino.cc/en/Reference/SD
  De todas maneras este es un resumen:

  SD.begin()
  SD.exists()
  SD.mkdir()
  SD.open()
  SD.remove()
  SD.rmdir()
  File.available()
  File.close()
  File.flush()
  File.peek()
  File.position() : devuelve la posición de lectura/escritura del archivo abierto
  File.print()
  File.println()
  File.seek() : mueve el puntero de lectura/escritura a la posición indicada
  File.size() : devuelve el tamaño total de un archivo
  File.read()
  File.write()
  File.isDirectory()
  File.openNextFile()
  File.rewindDirectory()
  
  Notas:
  - No podremos asegurarnos que se ha escrito algo en un archivo hasta que lo cerremos o hasta que
    le obligemos a vaciar el contenido del buffer de escritura con el comando flush().
  - Cuando cerramos un archivo, automáticamente se hace un flush
  - La librería no soporta nombres largos. Sólo formato 8.3
  - Se pueden abrir varios archivos simultáneamente, siempre con variables distintas. Máximo 4

  Resultado:
  Escribimos en un archivo y lo leemos a continuación
*/

#include <Arduino.h>
#include <SD.h> //Gestión de lector SD

void _printDirectory(File *dir,byte depth)
{
  //Muestra el contenido de una carpeta
  //Función recursiva
  //Para evitar que en cada llamada se cree una copia del objeto File, sólo recibimos su puntero
  //Esto ahorra memoria

  //Bucle infinito
  while(1)
  {
    //Tomamos nota de la primera entrada de la carpeta
    File entry =  (*dir).openNextFile();
    //Si no tiene entradas...hemos terminaso
    if (!entry) return;
    //Escribimos tantas tabulaciones como profundidad tenga
    for (byte i=0;i<depth;i++) Serial.print(F("  "));
    //Mostramos el "codo"
    Serial.print(F("\\-- "));
    //Mostramos el nombre de la entrada
    Serial.print(entry.name());
    //Si la entrada es una carpeta...
    if (entry.isDirectory())
    {
      //Le añadimos una barra al final para distinguirla de un archivo
      Serial.println(F("/"));
      //Mostramos el contenido de la carpeta, aumentando en uno la profundidad
      _printDirectory(&entry,depth+1);
    }
    else //La entrada es un archivo...
    {
      //Mostramos el separador
      Serial.print(F(" : "));
      //Mostramos el tamaño del archivo en bytes
      Serial.println(entry.size());
    }
    //Hemos terminado de utilizar la entrada. La cerramos
    entry.close();
  }
}

void printDirectory(const char *path)
{
  //Muestra el contenido de una carpeta
  //La carpeta debe existir

  Serial.print(F("Showing folder "));
  Serial.println(path);
  //Anotamos la entrada que representa la carpeta (path)
  File root=SD.open(path);
  //Si la entrada existe...
  if(root)
  {
    //Si la entrada es una carpeta...
    if(root.isDirectory())
    {
      //...mostramos el contenido de la carpeta
      _printDirectory(&root,0);
    }
    else //La entrada no es una carpeta
    {
      Serial.println(F("Error. This is not a folder"));
    }
    //Cerramos la entrada
    //Si no se hace esto se irán quedando abiertos los archivos
    //El límite es de 4 archivos abiertos simultáneamente, por lo tanto, al quinto
    //uso sin cerrar, no dará error, pero permitirá abrir nuevos archivos
    root.close();
  }
  else //La entrada no existe
  {
    Serial.println(F("Folder doesn't exist"));
  }
}

void createDir(const char *path)
{
  //Crea una nueva carpeta
  
  Serial.print(F("Creating Dir: "));
  Serial.println(path);
  if (SD.mkdir(path)) Serial.println("Dir created");
  else Serial.println("mkdir failed");
}

void removeDir(const char *path)
{
  //Elimina una carpeta
  //La carpeta debe estar vacía
  
  Serial.print(F("Removing Dir: "));
  Serial.println(path);
  if (SD.rmdir(path)) Serial.println("Dir removed");
  else Serial.println("rmdir failed");
}

void writeFile(const char *path, const char *message)
{
  //Escribe un texto en un archivo
  //La carpeta que contiene el archivo debe existir
  
  Serial.print(F("Writing file: "));
  Serial.println(path);

  //Abrimos el archivo en modo lectura/escritura
  File file = SD.open(path,FILE_WRITE);
  //Si no hemos podido abrir el archivo...
  if (!file)
  {
    Serial.println(F("Failed to open file for writing"));
    return;
  }
  //Hemos podido abrir el archivo
  //Si hems podido escribir el texto al inicio del archivo...informamos
  if (file.print(message)) Serial.println(F("File written"));
  //Si no hemos podido...informamos
  else Serial.println(F("Write failed"));
  //Hemos terminado de usar el archivo. Lo cerramos
  file.close();
}

void appendFile(const char *path, const char *message)
{
  //Añade un texto al final de un archivo
  //El archivo debe existir
  
  //La librería SD no tiene el modo append
  //Tenemos que abrir el archivo en modo de escritura y mover la
  //posición de escritura al final

  Serial.print(F("Appending to file: "));
  Serial.print(path);
  //Abrimos el archivo en modo lectura/escritura
  File file = SD.open(path,FILE_WRITE);
  //Si no hemos podido abrir el archivo...
  if (!file)
  {
    Serial.println(F("Failed to open file for appending"));
    return;
  }
  //Hemos podido abrir el archivo

  //Obtenemos su tamaño
  unsigned long fileSize=file.size();
  //Movemos la posición de lectura/escritura dentro del archivo al último carácter/byte
  file.seek(fileSize);
  //Si hemos podido escribir el texto al final del archivo...informamos
  if (file.print(message)) Serial.println(F("Message appended"));
  //Si no hemos podido...informamos
  else Serial.println(F("Append failed"));
  //Hemos terminado de usar el archivo. Lo cerramos
  file.close();
}

void readFile(const char *path)
{
  //Lee un archivo y lo envía al puerto serie
  //El archivo debe existir
  
  Serial.print(F("Reading file: "));
  Serial.println(path);
  //Abrimos el archivo
  //Los archivos por defecto se abren como sólo lectura. Es lo mismo que:
  //File file = SD.open(path,FILE_READ);
  File file = SD.open(path);
  //Si no hemos podido abrir el archivo
  if (!file)
  {
    Serial.println(F("Failed to open file for reading"));
    return;
  }
  Serial.println(F("----start----"));
  while (file.available()) Serial.write(file.read());
  file.close();
  Serial.println(F("\n----end----"));
}

void deleteFile(const char *path)
{
  //Borra un archivo
  //El archivo debe existir
  
  Serial.print(F("Deleting file: "));
  Serial.println(path);
  //Si lo hemos podido eliminar...informamos
  if (SD.remove(path)) Serial.println(F("File deleted"));
  //Sino, también
  else Serial.println(F("Delete failed"));
}

void renameFile(const char *path1,const char *path2)
{
  //Renombra un archivo
  //El archivo debe existir
  //La librería SD no tiene la función rename
  //La única solución es crear un nuevo archivo, copiar el contenido del antiguo
  //al nuevo y borrar el antiguo :-(
  
  Serial.print(F("Renaming file "));
  Serial.print(path1);
  Serial.print(F(" to "));
  Serial.println(path2);
  //Creamos el nuevo archivo en modo de escritura
  File newFile = SD.open(path2,FILE_WRITE);
  //Si no hemos podido...
  if (!newFile)
  {
    Serial.println(F("Failed to open file for writing"));
    return;
  }
  //Abrimos el archivo antiguo en modo de lectura
  File oldFile = SD.open(path1);
  //Si no hemos podido...
  if (!oldFile)
  {
    Serial.println(F("Failed to open file for reading"));
    newFile.close();
    return;
  }
  //Copiamos el contenido de manera binaria
  while(oldFile.available()) newFile.write(oldFile.read());
  //Cerramos los archivos
  oldFile.close();
  newFile.close();
  //Eliminamos el archivo antiguo
  SD.remove(path1);
  Serial.println(F("File renamed"));
}

void setup()
{
  //Abrimos puerto serie para mostrar los mensajes de debug
  Serial.begin(115200);

  //Si no se puede montar el sistema de archivos... 
  if (!SD.begin())
  {
    Serial.println("Error al montar sistema de archivos");
    return; //Terminamos función setup() y pasamos a loop()
  }

  printDirectory("/");
  createDir("/mydir");
  printDirectory("/");
  removeDir("/mydir");
  printDirectory("/");
  writeFile("/hello.txt","Hello ");
  printDirectory("/");
  appendFile("/hello.txt", "World!");
  printDirectory("/");
  readFile("/hello.txt");
  deleteFile("/foo.txt"); //No se puede borrar un archivo inexistente
  renameFile("/hello.txt", "/foo.txt");
  printDirectory("/");
  readFile("/foo.txt");
  deleteFile("/foo.txt");
  printDirectory("/");

  createDir("/mydir");
  writeFile("/mydir/hello.txt","Hi!");
  printDirectory("/");
  printDirectory("/mydir");
  deleteFile("/mydir/hello.txt");
  removeDir("/mydir");
  printDirectory("/");

  //Hemos terminado de utilizar el lector SD
  SD.end();
}

void loop()
{
  //No tenemos nada especial que repetir
}
