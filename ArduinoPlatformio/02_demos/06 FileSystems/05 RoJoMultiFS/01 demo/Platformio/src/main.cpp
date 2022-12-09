/*
  Autor: Ramón Junquera
  Fecha: 20221209
  Tema: Librería RoJoMultiFS
  Objetivo: Tests

  Descripción:
  La librería RoJoMultiFS permite trabajar con distintos sistemas de archivos pero
  con una misma interface, simplificando y unificando las llamadas.
  Se presentan varios ejemplos de uso.

  Nota:
  - Este ejemplo no tendrá un comportamiento correcto cuando se utilice con un sistema
    de archivos SPIFFS, porque no permite el uso de directorios.

  Recordatorio:
  - M5Stack Fire no tiene problemas para leer tarjetas SD antiguas (4Gb).
  - Un lector SD standard tiene muchos problemas para leer tarjetas antiguas debido a que
    necesitan más consumo que las nuevas.
  
  Resultado:
  Ejemplos de funcionalidades del sistema de archivos
*/

#include <Arduino.h>
#include <RoJoMultiFS.h>


//Muestra el contenido de una carpeta (función privada)
void _listDir(String path) {
  RoJoDir dir=RoJoFS.openDir(path);
  while(dir.next()) {
    String newPath=path+((path=="/")?"":"/")+dir.fileName();
    Serial.print(newPath);
    if(dir.isDirectory()) { 
      Serial.println(" <DIR>");
      _listDir(newPath);
    } else Serial.println(" ("+String(dir.fileSize())+")");
  }
}

//Muestra el contenido de una carpeta
void listDir(String path="/") {
  Serial.println("\nMostrando directorio "+path);
  _listDir(path);
}

//Crea una nueva carpeta
//- Ejemplo de path: /dir1
void createDir(String path) {
  Serial.println("\nCreando directorio: "+path);
  if(RoJoFS.mkdir(path)) Serial.println("Directorio creado");
  else Serial.println("Error creando directorio");
}

//Elimina una carpeta
//La carpeta debe estar vacía
//- Ejemplo de path: /dir1
void removeDir(String path,bool force=false) {
  Serial.println("\nEliminando directorio: "+path);
  if(force) Serial.println("Se fuerza su vaciado previo");
  if(RoJoFS.rmdir(path,force)) Serial.println("Directorio eliminado");
  else Serial.println("Error eliminando directorio");
}

//Crea un archivo con un texto en su interior
void writeFile(String fileName,String message) {
  Serial.println("\nCreando archivo: "+fileName+", con texto: "+message);
  File file=RoJoFS.open(fileName,"w");
  if (!file) {
    Serial.println("Error al crear el archivo");
    return;
  }
  if (file.print(message)) Serial.println("Archivo creado");
  else Serial.println("Error de escritura");
  file.close();
}

//Añade un texto al final de un archivo
void appendFile(String fileName,String message) {
  Serial.println("\nAñadiendo a archivo: "+fileName+", el texto: "+message);
  File file=RoJoFS.open(fileName,"a");
  if (!file) {
    Serial.println("Error al abrir el archivo");
    return;
  }
  if (file.print(message)) Serial.println("Texto añadido");
  else Serial.println("Error al añadir texto");
  file.close();
}

//Muestra el contenido de un archivo
void readFile(String fileName) {
  Serial.println("\nLeyendo archivo: "+fileName);
  File file=RoJoFS.open(fileName);
  if (file) { //Si hemos podido abrir el archivo...
    Serial.print("Leido desde archivo: ");
    while (file.available()) Serial.write(file.read());
    file.close();
    Serial.println();
  } else Serial.println("Error al abrir el archivo");
}

//Borra un archivo
void deleteFile(String fileName) {
  Serial.println("\nBorrando archivo: "+fileName);
  if (RoJoFS.remove(fileName)) Serial.println("Archivo borrado");
  else Serial.println("Error al borrar archivo");
}

//Renombra un archivo
void renameFile(String fileName1,String fileName2) {
  Serial.println("\nRenombrando archivo "+fileName1+" a "+fileName2);
  if (RoJoFS.rename(fileName1,fileName2)) Serial.println("Archivo renombrado");
  else Serial.println("Error al renombrar");
}

void setup() {
  Serial.begin(115200); //Abrimos puerto serie para mostrar los mensajes de debug
  delay(5000); //Esperamoa a que Serial esté preparado
  Serial.println();

  if (!RoJoFS.begin()) {
    Serial.println("Error al montar sistema de archivos");
    return; //Terminamos función setup() y pasamos a loop()
  }
  Serial.println("Sistema de archivos Ok");

  //Demo creación, lectura, adición, renombrado y eliminación de archivos en raíz
  listDir("/");
  writeFile("/hello1.txt","Hi!");
  writeFile("/hello2.txt","Hello ");
  listDir("/");
  deleteFile("/hello1.txt"); //Borrado de archivo existente
  deleteFile("/foo.txt"); //No se puede borrar un archivo inexistente
  readFile("/hello2.txt");
  appendFile("/hello2.txt","World");
  renameFile("/hello2.txt", "/foo.txt");
  readFile("/foo.txt");
  deleteFile("/foo.txt");

  //Demo creación y eliminación de directorio
  listDir("/x23"); //Directorio inexistente
  listDir("/");
  createDir("/mydir");
  listDir("/");
  removeDir("/mydir");
  listDir("/");

  //Demo creación, lectura, adición, renombrado y eliminación de archivos en directorio
  createDir("/mydir");
  writeFile("/mydir/hello1.txt","Hi!");
  writeFile("/mydir/hello2.txt","Hello ");
  listDir("/");
  deleteFile("/mydir/hello1.txt"); //Borrado de archivo existente
  deleteFile("/mydir/foo.txt"); //No se puede borrar un archivo inexistente
  readFile("/mydir/hello2.txt");
  appendFile("/mydir/hello2.txt","World");
  renameFile("/mydir/hello2.txt", "/mydir/foo.txt");
  readFile("/mydir/foo.txt");
  listDir("/mydir");

  //Demo de eliminación normal y forzada de un directorio
  removeDir("/mydir"); //No se puede borrar un directorio que no esté vacío
  removeDir("/mydir",true); //Borramos el directorio forzando su vaciado
  listDir("/");

  RoJoFS.end();
}

void loop() {
  //Nada especial que hacer aquí
  delay(100);
}