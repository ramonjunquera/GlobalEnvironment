/*
  Autor: Ramón Junquera
  Fecha: 20220107
  Tema: LittleFS
  Objetivo: Tests
  Material: placa ESP

  Descripción:
  LittleFS es el sistema de archivos que ha sustituido a SPIFFS.
  Es más rápido, permite carpetas anidadas y tiene la misma sintaxis para todos los 
  dispositivos.
*/

#include <Arduino.h>
#ifdef ESP8266
  #include <LittleFS.h>
#elif defined(ESP32)
  #include <RoJoLittleFS32.h>
#else
  #error Device unknown
#endif

//Muestra el contenido de una carpeta
void listDir(String path="/") {
  File root=LittleFS.open(path,"r");
  if(!root) {
    Serial.println("Error opening folder "+path);
    return;
  }
  if(!root.isDirectory()) {
    Serial.println("Error. "+path+" is not a folder");
    return;
  }
  File file=root.openNextFile();
  while(file) {
    String filename=file.name(); //Tomamos el nombre del nodo
    #ifdef ESP32
      //En ESP32 el name incluye el path completo que comienza por /
      //En ESP8266 sólo devuelve el nombre del objeto. La carpeta raíz es un String vacío.
      //Le quitaremos el path para guardar compatibilidad
      filename=filename.substring(filename.lastIndexOf('/')+1);
    #endif
    filename="/"+filename; //Añadimos / al comienzo del nombre
    if(path.length()>1) filename=path+filename; //Si no es la carpeta raíz...le añadimos el path
    if(file.isDirectory()) { //Si es una carpeta...
      Serial.println("<DIR> "+filename); //...lo mostramos
      listDir(filename); //Recorremos los nodos de esta carpeta
    } else Serial.println("      "+filename+" ("+String(file.size())+")"); //Si es un archivo...lo mostramos
    file=root.openNextFile();
  }
}

void writeFile(String fileName,String message) {
  Serial.println("\nWriting file: "+fileName+", with: "+message);
  File file=LittleFS.open(fileName,"w");
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) Serial.println("File written");
  else Serial.println("Write failed");
  file.close();
}

void readFile(String fileName) {
  Serial.println("\nReading file: "+fileName);
  File file=LittleFS.open(fileName,"r"); //Por defecto es de sólo lectura
  if(!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  while(file.available()) Serial.write(file.read());
  file.close();
  Serial.println();
}

void appendFile(String fileName,String message) {
  Serial.println("\nAppending to file: "+fileName+", with: "+message);
  File file=LittleFS.open(fileName,"a");
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) Serial.println("Message appended");
  else Serial.println("Append failed");
  file.close();
}

void deleteFile(String fileName) {
  Serial.println("\nDeleting file: "+fileName);
  if(LittleFS.remove(fileName)) Serial.println("File deleted");
  else Serial.println("Delete failed");
}

void renameFile(String fileName1,String fileName2) {
  Serial.println("\nRenaming file "+fileName1+" to "+fileName2);
  if(LittleFS.rename(fileName1,fileName2)) Serial.println("File renamed");
  else Serial.println("Rename failed");
}

void createFolder(String path) {
  Serial.println("\nCreating folder: "+path);
  if(LittleFS.mkdir(path)) Serial.println("Folder created");
  else Serial.println("Error creating folder");
}

//Crear folders anidados
//Crearemos las carpetas de izquierda a derecha, cada vez con un nivel de profundidad más
void createFolder2(String path) { 
  Serial.println("\nCreating folders: "+path);
  String path2Create="";
  while(path.length()>0 && path[0]=='/') {
    int pos=path.indexOf('/',1); //Anotamos siguiente /
    if(pos<0) pos=path.length(); //Si no hay más...tomaremos el resto del String
    path2Create+=path.substring(0,pos);
    path=path.substring(pos);
    createFolder(path2Create);
  }
}

//Elimina una carpeta
//Si el path tiene varios niveles, sólo elimina la última
void deleteFolder(String path) {
  Serial.println("\nDeleting folder: "+path);
  if(LittleFS.rmdir(path)) Serial.println("Folder deleted");
  else Serial.println("Error deleting folder");
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println();
  
  LittleFS.begin(); //Si no reconoce el formato, lo formatea automáticamente
  //LittleFS.format(); //El formateo forzado debe ir siempre después de la inicialización
  Serial.println("LittleFS mounted");
  listDir(); //Debería estar vacío
  writeFile("/hello.txt","Hello ");
  listDir();
  readFile("/hello.txt");
  appendFile("/hello.txt","World!");
  readFile("/hello.txt");
  deleteFile("/foo.txt"); //No se puede borrar un archivo inexistente
  renameFile("/hello.txt", "/foo.txt");
  listDir();
  readFile("/foo.txt");
  //Las carpetas anidadas se deben crear de una en una, comenzando desde la más alta
  createFolder("/folder1");
  createFolder("/folder1/folder11");
  listDir();
  //Esta función nos hace el trabajo de crear las carpetas desde la más alta
  createFolder2("/folder2/folder21");
  writeFile("/folder2/myFile.txt","bye");
  listDir();
  deleteFile("/foo.txt");
  //Cuando se elimina una carpeta vacía, si todas sus superiores también están
  //vacías, también se eliminan.
  deleteFolder("/folder1/folder11");
  listDir();
  deleteFolder("/folder2"); //No podemos borrar una carpeta con contenido
  //Queremos borrar la carpeta /folder2
  //Sólo borra la carpeta indicada porque la superior tiene archivos
  deleteFolder("/folder2/folder21");
  //Si borramos un archivo y dejamos la carpeta vacía y todas las carpetas
  //superiores también están vacías, también se borran las carpetas.
  deleteFile("/folder2/myFile.txt");
  listDir();
  LittleFS.end();
}

void loop() {
  //Nada especial que hacer aquí
  delay(100);
}