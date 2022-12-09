/*
  Autor: Ramón Junquera
  Fecha: 20221207
  Tema: Demo de gestión de archivos y carpetas en RPi
  Objetivo: Tests
*/

#include <Arduino.h>
#include <RPiFS.h>

const String rootPath="data";
const int rootPathLegth=rootPath.length();
RPiFSclass RPiFS;

void _listDir(string path) {
  Dir dir=RPiFS.openDir(path);
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

//Crea un directorio
//- Ejemplo de path: /dir1
void createDir(string path) {
  Serial.println("\nCreando directorio: "+path);
  if(RPiFS.mkdir(path)) Serial.println("Directorio creado");
  else Serial.println("Error creando directorio");
}

//Elimina un directorio
//- Ejemplo de path: /dir1
void removeDir(string path,bool force=false) {
  Serial.println("\nEliminando directorio: "+path);
  if(force) Serial.println("Se fuerza su vaciado previo");
  if(RPiFS.rmdir(path,force)) Serial.println("Directorio eliminado");
  else Serial.println("Error eliminando directorio");
}

void writeFile(string fileName,string message) {
  Serial.println("\nCreando archivo: "+fileName+", con texto: "+message);
  File file=RPiFS.open(fileName,"w");
  if (!file) {
    Serial.println("Error al crear el archivo");
    return;
  }
  if (file.print(message)) Serial.println("Archivo creado");
  else Serial.println("Error de escritura");
  file.close();
}

void appendFile(string fileName,string message) {
  Serial.println("\nAñadiendo a archivo: "+fileName+", el texto: "+message);
  File file=RPiFS.open(fileName,"a");
  if (!file) {
    Serial.println("Error al abrir el archivo");
    return;
  }
  if (file.print(message)) Serial.println("Texto añadido");
  else Serial.println("Error al añadir texto");
  file.close();
}

void readFile(string fileName) {
  Serial.println("\nLeyendo archivo: "+fileName);
  File file=RPiFS.open(fileName);
  if (!file) {
    Serial.println("Error al abrir el archivo");
    return;
  }
  Serial.print("Leido desde archivo: ");
  while (file.available()) Serial.write(file.read());
  file.close();
  Serial.println();
}

//Elimina un archivo
void deleteFile(string fileName) {
  Serial.println("\nBorrando archivo: "+fileName);
  if (RPiFS.remove(fileName)) Serial.println("Archivo borrado");
  else Serial.println("Error al borrar archivo");
}

//Renombrar archivo
void renameFile(string fileName1,string fileName2) {
  Serial.println("\nRenombrando archivo "+fileName1+" a "+fileName2);
  if (RPiFS.rename(fileName1,fileName2)) Serial.println("Archivo renombrado");
  else Serial.println("Error al renombrar");
}
void setup() {
  //Abrimos puerto serie para mostrar los mensajes de debug
  Serial.begin(115200);
  Serial.println();

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
}

void loop() {
  //Nada especial que hacer aquí
  delay(100);
}