/*
  Autor: Ramón Junquera
  Fecha: 20221130
  Tema: Demo de gestión de archivos y carpetas en RPi
  Objetivo: Tests
*/

#include <string>
#include <iostream> //Para mensajes cout
#include <filesystem> //Gestión de directorios
using namespace std;

const string rootPath="data";
const int rootPathLegth=rootPath.length();

void _listDir(string path) {
  if(!filesystem::exists(path)) cout << "Error. El directorio no existe" << endl;
  else { //Si la entrada existe...
    if(!filesystem::is_directory(path)) cout << "Error. No es un directorio" << endl;
    else { //Si la entrada es un directorio...
      /*
      //La manera más simple de mostrar el contenido de un directorio es la siguiente:
      //Usamos un simple bucle sobre los items de un iterator de directorio
      for (const filesystem::__cxx11::directory_entry &entry : filesystem::directory_iterator(path)) {
        string fullPath=entry.path(); //Obtenemos el path completo
        cout << fullPath.substr(rootPathLegth) << " "; //Le quitamos el prefijo del path de raíz
        if(entry.is_directory()) { //Si es un directorio...
          cout << "<DIR>" << endl; //Mostramos <DIR>
          _listDir(fullPath); //Listamos el directorio localizado
        } else cout << "(" << entry.file_size() << ")" << endl; //Si es un archivo mostramos su tamaño
      }
      */
      //El problema es que requiere de un bucle for que recorre todas las entradas del directorio
      //y que no podemos romper.
      //Para que se puedan hacer varias llamadas deberíamos reescribirlo así:
      //El recorrido por los items del directorio lo haremos con un "iterator" de directorios
      filesystem::directory_iterator it=filesystem::directory_iterator(path);
      while(it != filesystem::directory_iterator()) { //Mientras el iterator no sea un iterator vacío...
        string fullPath=it->path(); //Obtenemos el path completo
        cout << fullPath.substr(rootPathLegth) << " "; //Le quitamos el prefijo del path de raíz
        if(it->is_directory()) { //Si es un directorio...
          cout << "<DIR>" << endl; //Mostramos <DIR>
          _listDir(fullPath); //Listamos el directorio localizado
        } else cout << "(" << it->file_size() << ")" << endl; //Si es un archivo mostramos su tamaño
        it++; //Pasamos el siguiente item del iterator
      }
    }
  }
}

void _listDirOri(string path) {
  if(!filesystem::exists(path)) cout << "Error. El directorio no existe" << endl;
  else { //Si la entrada existe...
    if(!filesystem::is_directory(path)) cout << "Error. No es un directorio" << endl;
    else { //Si la entrada es un directorio...
      for (const filesystem::__cxx11::directory_entry &entry : filesystem::directory_iterator(path)) {
        string fullPath=entry.path();
        cout << fullPath.substr(rootPathLegth) << " ";
        if(entry.is_directory()) {
          cout << "<DIR>" << endl;
          _listDir(fullPath);
        } else cout << "(" << entry.file_size() << ")" << endl;
      }
    }
  }
}



//Muestra contenido de directorio
//Path en formato /dir
void listDir(string path) {
  cout << "\nMostrando directorio: " << path << endl;
  _listDir(rootPath+path);
}

//Crea un directorio
//- Ejemplo de path: /dir1
void createDir(string path) {
  cout << "\nCreando directorio: " << path << endl;
  const string fullPath=rootPath+path;
  bool res; //Resultado
  //Si el directorio ya existe no se considerará un error
  if(!filesystem::exists(fullPath)) res=filesystem::create_directory(fullPath);
  else res=filesystem::is_directory(fullPath);
  cout << (res?"Directorio creado":"Error creando directorio") << endl;
}

//Elimina un directorio
//- Ejemplo de path: /dir1
void removeDir(string path,bool force=false) {
  cout << "\nEliminando directorio: " << path << endl;
  const string fullPath=rootPath+path;
  if(!filesystem::exists(fullPath)) cout << "Error. El directorio no existe" << endl;
  else { //Si la entrada existe...
    if(!filesystem::is_directory(fullPath)) cout << "Error. No es un directorio" << endl;
    else { //Si la entrada es un directorio...
      if(force) {
        cout << "Se fuerza su vaciado previo" << endl;
        if(filesystem::remove_all(fullPath)) cout << "Directorio eliminado" << endl;
        else cout << "Error eliminando directorio" << endl;
      } else {
        error_code e;
        //Llamamos a método con parámetro de código de error para que no produzca una excepción
        if(filesystem::remove(fullPath,e)) cout << "Directorio eliminado" << endl;
        else cout << "Error eliminando directorio" << endl;
      }
    }
  }
}

void writeFile(string fileName,string message) {
  cout << "\nCreando archivo: " << fileName << ", con texto: " << message << endl;
  FILE *f=fopen((rootPath+fileName).c_str(),"w");
  if(!f) cout << "Error creando archivo" << endl;
  else { //Se ha podido crear el archivo
    if(fprintf(f,message.c_str())) cout << "Archivo creado" << endl;
    else cout << "Error de escritura" << endl;
    fclose(f);
  }
}

void appendFile(string fileName,string message) {
  cout << "\nAñadiendo a archivo: " << fileName << ", el texto: " << message << endl;
  FILE *f=fopen((rootPath+fileName).c_str(),"a");
  if(!f) cout << "Error abriendo archivo" << endl;
  else { //Se ha podido abrir el archivo
    if(fprintf(f,message.c_str())) cout << "Texto añadido" << endl;
    else cout << "Error de escritura" << endl;
    fclose(f);
  }
}

void readFile(string fileName) {
  cout << "\nLeyendo archivo: " << fileName << endl;
  FILE *f=fopen((rootPath+fileName).c_str(),"r");
  if(!f) cout << "Error abriendo archivo" << endl;
  else { //Se ha podido abrir el archivo
    while(1) {
      char c=fgetc(f);
      if(feof(f)) break;
      cout << c;
    }
    cout << endl;
    fclose(f);
  }
}

//Elimina un archivo
void deleteFile(string fileName) {
  cout << "\nBorrando archivo: " << fileName << endl;
  if(remove((rootPath+fileName).c_str())) cout << "Error al borrar archivo" << endl;
  else cout << "Archivo borrado" << endl;
}

//Renombrar archivo
void renameFile(string fileName1,string fileName2) {
  cout << "\nRenombrando archivo: " << fileName1 << " a "<< fileName2 << endl;
  if(rename((rootPath+fileName1).c_str(),(rootPath+fileName2).c_str())) cout << "Error al renombrar archivo" << endl;
  else cout << "Archivo renombrado" << endl;
}

int main(int argc, char **argv) {
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