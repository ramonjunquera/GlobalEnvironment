/*
  Autor: Ramón Junquera
  Versión: 20230216
  Descripción:
    Gestión de sistema de archivos de RPi.
	Se toma la carpeta data como raíz.
*/

#ifndef RoJoRPiFS_h
#define RoJoRPiFS_h

#include <Arduino.h>
#include <filesystem> //Gestión de directorios

const String _rootDir="data"; //Nombre de la carpeta que utilizaremos como raíz

typedef enum {
  SeekSet=0,
  SeekCur=1,
  SeekEnd=2
} arduinoSeekMode;

class File {
  private:
	  FILE *_f;
		String _fileName; //Path
		time_t (*_callBackTimeStamp)()=NULL;

  public:
		//Constructor
		File() {
			_f=nullptr; //No tenemos archivo asignado
		}
		
		//Constructor para apertura de archivo
		//cb es la función de callback para fijar la fecha del archivo al hacer close
		File(string filename,string options,time_t (*cb)()=NULL) {
			_fileName=filename;
			_callBackTimeStamp=cb;
			_f=fopen(filename.c_str(),options.c_str());
		}

		//Lee un byte desde un archivo
    ::byte read() {
			return static_cast<::byte>(fgetc(_f));
		}
		
		//Lee cierto número de bytes desde un archivo a memoria
		void readBytes(char *buffer,uint32_t len) {
			fread(buffer,1,len,_f);
		}
		void read(::byte *buffer,uint32_t len) {
			fread(buffer,1,len,_f);
		}
		
		//Operación ! (negación)
		//Devuelve true si el archivo NO está abierto
		bool operator!() {
			return !_f;
		}

    //Operación conversión a bool (inverso de operator!)
		//Devuelve true si el archivo está abierto
		operator bool() const {
			return _f;
		}
		
		//Cierra el archivo
		void close() {
			fclose(_f);
			//Aunque el archivo se haya cerrado correctamente, el puntero
			//sigue apuntando a la misma dirección. La borramos
			_f=nullptr;
			if(_callBackTimeStamp) { //Si tenemos definida función de callback para timestamp...
			  //En RPi el timesptamp se fija una vez que se ha cerrado el archivo
				//Obtenemos la fecha en formato time_t y ajustamos
				uint64_t t1=(_callBackTimeStamp()+12009080074)*1000000000;
				//El puntero de t será del tipo file_time_type. Después tomamos su valor
				filesystem::file_time_type t2=*((filesystem::file_time_type *) &t1);
			  filesystem::last_write_time(_fileName,t2);
			}
		}
		
		//Escribe un número de bytes de un buffer
		void write(::byte *buffer,uint32_t len) {
			fwrite(buffer,1,len,_f);
		}
		
		//Escribe un byte 
		void write(::byte value) {
			fputc(value,_f);
		}

		//Escribe un texto
		//Devuelve true si lo consigue
		bool print(String value) {
			return fprintf(_f,value.c_str())>=0;
		}

		//Queda algo pendiente por leer?
		bool available() {
			//No existe la función available. Sólamente feof().
			//Para saber si hemos llegado al final del archivo debemos leer un byte más, entonces se refrescará el flag eof.
			//Pero esta acción aumenta en uno la posición de lectura/escritura y no interesa.
			//La solución es tomar nota de la posición actual, hacer la lectura del siguiente byte, comprobar si ya hemos
			//llegado al final del archivo y recuperar la posición original.
			fpos_t currentPos; //Variable para guardar la posición actual
			fgetpos(_f,&currentPos); //Obtenemos la posición actual
			fgetc(_f); //Leemos (y perdemos) el siguiente byte
			bool res=!feof(_f); //Obtenemos la respuesta
			fsetpos(_f,&currentPos); //Recuperamos la posición original
			return res; //Devolvemos la respuesta
		}
		
		//Definimos el offset de lectura de un archivo
		void seek(uint32_t len) {
			//Siempre contamos desde el principio
			fseek(_f,static_cast<int32_t>(len),SEEK_SET);
		}

    //Devuelve la fecha de última escritura del archivo
		time_t getLastWrite() {
			filesystem::file_time_type t1=filesystem::last_write_time(_fileName);
			//Es necesario convertir filesystem::file_time_type a time_t
			std::chrono::_V2::system_clock::time_point t2=std::chrono::time_point_cast<std::chrono::system_clock::duration>(t1-filesystem::file_time_type::clock::now()+std::chrono::system_clock::now());
			return std::chrono::system_clock::to_time_t(t2);
		}
};

class Dir {
  private:
    filesystem::directory_iterator _it;
    String _path;
		bool _firstTime=true;
		String _2digits(int v) { //Pasa int a String con 2 dígitos
			String res=String(v);
			if(res.length()<2) res="0"+res;
			return res;
		}
  public:
    Dir(String path="") {
			_path=_rootDir+path;
		}
		bool next() {
			if(_firstTime) {
				error_code e;
				//Llamamos a método con parámetro de código de error para que no produzca una excepción
				//ante un path inexistente
				_it=filesystem::directory_iterator(_path,e);
				_firstTime=false;
			} else _it++;
			if(_it != filesystem::directory_iterator()) return true;
			return false;
		}
		String fileName() {
			return _it->path().filename();
		}
		uint32_t fileSize() {
			return _it->file_size();
		}
		time_t fileCreationTime() {
			filesystem::file_time_type t1=_it->last_write_time();
			//Es necesario convertir filesystem::file_time_type a time_t
			std::chrono::_V2::system_clock::time_point t2=std::chrono::time_point_cast<std::chrono::system_clock::duration>(t1-filesystem::file_time_type::clock::now()+std::chrono::system_clock::now());
			return std::chrono::system_clock::to_time_t(t2);
		}
		bool isDirectory() {
			return _it->is_directory();
		}
};

//Clase de objeto de gestión de sistema de archivos
class RPiFSclass {
  private:
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
		//cb es la función de callback para fijar la fecha del archivo al hacer close
		File open(string filename,string options="r",time_t (*cb)()=NULL) {
			//Creamos el objeto File abriendo el archivo y lo devolvemos
			return File(_rootDir+filename,options,cb);
		}
	
		//Elimina un archivo
		//Devuelve true si se ha conseguido eliminar
		bool remove(string filename) {
			//En Linux la función remove devuelve 0 cuando se consigue borrar
			//Puesto que el método se llama remove y la función para
			//eliminar archivos también, si no ponemos el prefijo std::
			//pensará que estamos llamando al mismo método y cuando se
			//llene la pila de llamadas cíclicas, el programa dará un error.
			return !std::remove((_rootDir+filename).c_str());
		}

    //Existe un archivo?
		bool exists(string path) {
			return access((_rootDir+path).c_str(),F_OK)!=-1;
		}

		//Crear directorio
		//Devuelve true si lo consigue
		bool mkdir(string path) {
			path=_rootDir+path;
			bool res=false; //Respuesta
			//Si el directorio ya existe no se considerará un error
			if(!filesystem::exists(path)) res=filesystem::create_directory(path);
			else res=filesystem::is_directory(path);
			return res;
		}

		//Borrar directorio
		//Devuelve true si lo consigue
		bool rmdir(string path,bool force=false) {
			path=_rootDir+path;
			bool res=false;
			if(filesystem::exists(path)) { //Si existe la entrada...
				if(filesystem::is_directory(path)) { //Si es un directorio...
					if(force) res=filesystem::remove_all(path);
					else { //No se fuerza su vaciado previo...
						error_code e;
						//Llamamos a método con parámetro de código de error para que no produzca una excepción
						res=filesystem::remove(path,e);
					}
				}
			}
			return res;
		}
   
    //Renombrar archivo
		//Devuelve true si lo consigue
		bool rename(String pathFrom,String pathTo) {
			return !std::rename((_rootDir+pathFrom).c_str(),(_rootDir+pathTo).c_str());
		}

		Dir openDir(String path) {
			Dir d(path);
			return d;
		}
};

#endif