/*
 * Autor: Ramón Junquera
 * Versión: 20220218
 * Descripción:
 *   Gestión chip BCM2835 de Raspberry Pi 3.
 *   Emulación de librería LittleFS de familia ESP8266 (acceso a sistema de
 *   archivos).
 *   En LittleFS todos los nombres de archivo deben comenzar por /
 *   Y todos ellos se guardan en la raíz, puesto que no hay carpetas.
 *   La librería permitirá mantener la misma nomenclatura.
 *   Se simulará el sistema de archivo con el contenido de la carpeta
 *   "data" que se encuentre bajo el archivo ejecutado.
 */

#ifndef RoJoArduinoLittleFS_h
#define RoJoArduinoLittleFS_h

#include <iostream> //Para mensajes cout
#include <sys/stat.h> //Para mkdir

using namespace std;

#include <Arduino.h>

//Funciones LittleFS

typedef enum {
  SeekSet=0,
  SeekCur=1,
  SeekEnd=2
} arduinoSeekMode;

class File {
  private:
	FILE *_f;
	
  public:

	//Constructor
	File() {
	  //No tenemos archivo asignado
      _f=nullptr;
	}
	
	//Constructor para apertura de archivo
	File(string filename,string options) {
	  _f=fopen(filename.c_str(),options.c_str());
	}
	
	//Lee un byte desde un archivo
	byte read() {
      return static_cast<byte>(fgetc(_f));
	}
	
	//Lee cierto número de bytes desde un archivo a memoria
	void readBytes(char *buffer,uint32_t len) {
	  fread(buffer,1,len,_f);
	}
	void read(byte *buffer,uint32_t len) {
	  fread(buffer,1,len,_f);
	}
	
	//Operación ! (negación)
	bool operator!() {
	  //Devolveramos la negación del puntero del archivo
	  //El objetivo es que se pueda comparar la negación del objeto File para
	  //comprobar si se ha podido abrir el archivo.
	  return !_f;
	}
	
	//Cierra el archivo
	void close() {
	  fclose(_f);
	  //Aunque el archivo se haya cerrado correctamente, el puntero
	  //sigue apuntando a la misma dirección. La borramos
      _f=nullptr;
	}
	
	//Escribe un número de bytes de un buffer
	void write(byte *buffer,uint32_t len) {
	  fwrite(buffer,1,len,_f);
	}
	
	//Escribe un byte 
	void write(byte value) {
	  fputc(value,_f);
	}
	
	//Definimos el offset de lectura de un archivo
	void seek(uint32_t len) {
	  //Siempre contamos desde el principio
      fseek(_f,static_cast<int32_t>(len),SEEK_SET);
	}
};

//Llama directamente a la función mkdir
int _mkdir(const char *path, __mode_t mode) {
  //Creamos la función para que sea llamada desde la clase que definiremos a continuación
  //que al tener un método con el mismo nombre (mkdir) no puede llamarla porque la confunde
  //con el propio método
  return mkdir(path,mode);
}

//Llama directamente a la función rmdir
int _rmdir(const char *path) {
  //Creamos la función para que sea llamada desde la clase que definiremos a continuación
  //que al tener un método con el mismo nombre (rmdir) no puede llamarla porque la confunde
  //con el propio método
  return rmdir(path);
}

//Clase de objeto LittleFS
class LittleFSclass {
  public:
	
    //Inicializa el objeto
    bool begin(bool formatOnFail=false) {
      //Nada especial que hacer aquí
      //El método es declarado para guardar compatibilidad
      return true | formatOnFail;
	}

	void end() {
	  //Nada especial que hacer aquí
      //El método es declarado para guardar compatibilidad
	}
	
	//Abre un archivo
	File open(string filename,string options) {
	  //Creamos el objeto File abriendo el archivo y lo devolvemos
	  return File("data"+filename,options);
	}
	
	//Elimina un archivo
	//Devuelve true si se ha conseguido eliminar
	bool remove(string filename) {
	  //En Linux la función remove devuelve 0 cuando se consigue borrar
	  //Puesto que el método se llama remove y la función para
	  //eliminar archivos también, si no ponemos el prefijo std::
	  //pensará que estamos llamando al mismo método y cuando se
	  //llene la pila de llamadas cíclicas, el programa dará un error.
	  return !std::remove(("data"+filename).c_str());
	}

    //Existe un archivo?
	bool exists(string path) {
      return access(path.c_str(),F_OK)!=-1;
	}

	//Crear directorio
	bool mkdir(string path) {
	  //Llamamos la función _mkdir que hemos definido hace un momento
	  //Esa función llama al mkdir original
	  //No podemos llamar directamente al mkdir original desde aquí porque
	  //lo confunde con el propio método que estamos escribiendo
	  return _mkdir(path.c_str(),0777)!=-1;
	}

	//Borrar directorio
	bool rmdir(string path) {
	  //Llamamos la función _rmdir que hemos definido hace un momento
	  //Esa función llama al rmdir original
	  //No podemos llamar directamente al rmdir original desde aquí porque
	  //lo confunde con el propio método que estamos escribiendo
	  return _rmdir(path.c_str())!=-1;
	}
};

//Creamos el objeto LittleFS
LittleFSclass LittleFS;

#endif

