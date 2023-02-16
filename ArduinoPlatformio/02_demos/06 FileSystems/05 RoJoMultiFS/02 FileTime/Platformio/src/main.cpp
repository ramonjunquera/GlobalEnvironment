/*
  Autor: Ramón Junquera
  Fecha: 20230216
  Tema: Librería RoJoMultiFS
  Objetivo: Escritura de archivos con fecha determinada

  Descripción:
    Se crea un archivo con una fecha determinada. Se comprueba.
    Después se duplica el archivo y se ve que el nuevo también tiene la misma fecha.
    Esto significa que:
    - Se puede indicar una fecha determinada para un archivo
    - Se puede leer la fecha de un archivo y asignarlo a otro
*/

#include <Arduino.h>
#include <RoJoMultiFS.h>
//#include <time.h>

time_t _fileDate; //Fecha de escritura de los archivos

//Muestra el contenido de una carpeta (función privada)
void _listDir(String path) {
  RoJoDir dir=RoJoFS.openDir(path);
  while(dir.next()) {
    String newPath=path+((path=="/")?"":"/")+dir.fileName();
    Serial.print(newPath+" "+RoJoDir::fileTimeString(dir.fileTime())+" ");
    if(dir.isDirectory()) { 
      Serial.println("<DIR>");
      _listDir(newPath);
    } else Serial.println("("+String(dir.fileSize())+")");
  }
}

//Muestra el contenido de una carpeta
void listDir(String path="/") {
  Serial.println("\nMostrando directorio "+path);
  _listDir(path);
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

//Función de obtención de fecha para escritura de archivos
time_t fileDateFunc() { //Para LittleFS
  return _fileDate; //Simplemente devolvemos el valor de la variable global
}

//Fija la hora de la función de escritura de archivos
void setFileDate(time_t t) {
  #if defined(ESP8266) && RoJoFileSystem==0
    //La librería SD de ESP8266 tiene un error
    //Al escribir la fecha de un archivo, siempre duplica el tiempo de los segundos.
    //Aquí lo corregimos, poniendo sólo la meitad de los segundos indicados.
    struct tm *t1=localtime(&t);
    t-=t1->tm_sec/2;
  #endif
  _fileDate=t;
}

//Copia un archivo
//Devuelve true si lo consigue
bool copyFile(String fileName1,String fileName2) {
  Serial.println("\nCopiando archivo "+fileName1+" a "+fileName2);
  if(!RoJoFS.exists(fileName1)) {
    Serial.println("Error. El archivo origen no existe.");
    return false;
  }
  File file1=RoJoFS.open(fileName1);
  if(!file1) {
    Serial.println("Error abriendo archivo origen.");
    return false;
  }
  #ifdef ARDUINO_ARCH_AVR //Arduino
    setFileDate(0); //La librería SD de Arduino no lee fechas
  #else //ESP & RPi
    setFileDate(file1.getLastWrite()); //Fijamos la hora del archivo destino, la misma que la del origen
  #endif
  File file2=RoJoFS.open(fileName2,"w");
  if(!file2) {
    Serial.println("Error abriendo archivo destino.");
    file1.close();
    return false;
  }
  while(file1.available()) file2.write(file1.read());
  file2.close();
  file1.close();
  Serial.println("Copia correcta");
  return true;
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

  RoJoFS.setTimeCallback(fileDateFunc); //Fijamos función de fecha de archivos

  listDir("/");
  //Fijamos fecha de archivos a 14/02/2023 12:34:56
  struct tm t={0};
  t.tm_year=2023-1900; //yy
  t.tm_mon=2-1; //mm
  t.tm_mday=14; //dd
  t.tm_hour=12; //hh
  t.tm_min=34; //mm
  t.tm_sec=56; //ss
  setFileDate(mktime(&t));
  
  writeFile("/test1.txt","Hola");
  setFileDate(0); //Quitamos cualquier fecha existente
  listDir("/");
  copyFile("/test1.txt","/test2.txt");
  listDir("/");
  readFile("/test2.txt");
  deleteFile("/test2.txt");
  deleteFile("/test1.txt");
  listDir("/");

  RoJoFS.end();
}

void loop() {
  //Nada especial que hacer aquí
  delay(100);
}