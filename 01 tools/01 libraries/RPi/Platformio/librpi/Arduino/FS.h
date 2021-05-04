/*
 * Autor: Ramón Junquera
 * Versión: 20210210
 * Descripción:
 *   Gestión chip BCM2835 de Raspberry Pi 3.
 *   Emulación de librería FS de familia ESP8266 (acceso a sistema de
 *   archivos).
 *   En SPIFFS todos los nombres de archivo deben comenzar por /
 *   Y todos ellos se guardan en la raíz, puesto que no hay carpetas.
 *   La librería permitirá mantener la misma nomenclatura.
 *   Se simulará el sistema de archivo con el contenido de la carpeta
 *   "data" que se encuentre bajo el archivo ejecutado.
 */

#ifndef RoJoArduinoFS_h
#define RoJoArduinoFS_h

//#include <stdio.h>
#include <iostream> //Para mensajes cout

using namespace std;

#include <Arduino.h>

//Funciones FS

typedef enum
{
    SeekSet=0,
    SeekCur=1,
    SeekEnd=2
} arduinoSeekMode;

class File
{
	private:
	FILE *_f;
	
	public:
	
	//Constructor
	File()
	{
		//No tenemos archivo asignado
		_f=NULL;
	}
	
	//Constructor para apertura de archivo
	File(string filename,string options)
	{
		_f=fopen(filename.c_str(),options.c_str());
	}
	
	//Lee un byte desde un archivo
	byte read()
	{
		return fgetc(_f);
	}
	
	//Lee cierto número de bytes desde un archivo a memoria
	void readBytes(char *buffer,uint32_t len)
	{
		fread(buffer,1,len,_f);
	}
	void read(byte *buffer,uint32_t len)
	{
		fread(buffer,1,len,_f);
	}
	
	//Operación ! (negación)
	bool operator!()
	{
		//Devolveramos la negación del puntero del archivo
		//El objetivo es que se pueda compara negar el objeto File para
		//comprobar si se ha podido abrir el archivo.
		return !_f;
	}
	
	//Cierra el archivo
	void close()
	{
		fclose(_f);
		//Aunque el archivo se haya cerrado correctamente, el puntero
		//sigue apuntando a la misma dirección. La borramos
		_f=NULL;
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
	void seek(uint32_t len)
	{
		//Siempre contamos desde el principio
		fseek(_f,len,SEEK_SET);
	}
};

//Clase de objeto SPIFFS
class SPIFFSclass
{
	public:
	
	//Inicializa el objeto
	bool begin()
	{
		//Nada especial que hacer aquí
		//El método es declarado para guardar compatibilidad
		return true;
	}
	
	//Abre un archivo
	File open(string filename,string options)
	{
		//Creamos el objeto File abriendo el archivo y lo devolvemos
		return File("data"+filename,options);
	}
	
	//Elimina un archivo
	//Devuelve true si se ha conseguido eliminar
	bool remove(string filename)
	{
		//En Linux la función remove devuelve 0 cuando se consigue borrar
		//Puesto que el método se llama remove y la función para
		//eliminar archivos también, si no ponemos el prefijo std::
		//pensará que estamos llamando al mismo método y cuando se
		//llene la pila de llamadas cíclicas, el programa dará un error.
		return !std::remove(("data"+filename).c_str());
	}
};

//Creamos el objeto SPIFFS
SPIFFSclass SPIFFS;

#endif

