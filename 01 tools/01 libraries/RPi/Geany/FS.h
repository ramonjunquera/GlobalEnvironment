/*
 * Autor: Ramón Junquera
 * Descripción: Gestión chip BCM2835 de Raspberry Pi 3. Emulación de
 *   librería FS de familia ESP8266 (acceso a sistema de archivos)
 * Versión: 20171025
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
	File()
	{
		//Constructor tradicional
		//No tenemos archivo asignado
		_f=NULL;
	}
	
	File(string filename,string options)
	{
		//Constructor para apertura de archivo
		_f = fopen(filename.c_str(),options.c_str());
	}
	
	void readBytes(char *buffer,uint32_t len)
	{
		//Lee un número de bytes a un buffer
		fread(buffer,1,len,_f);
	}
	
	bool operator!()
	{
		//Operación ! (negación)
		//Devolveramos la negación del puntero del archivo
		//El objetivo es que se pueda compara negar el objeto File para
		//comprobar si se ha podido abrir el archivo.
		return !_f;
	}
	
	void close()
	{
		//Cierra el archivo
		fclose(_f);
		//Aunque el archivo se haya cerrado correctamente, el puntero
		//sigue apuntando a la misma dirección. La borramos
		_f=NULL;
	}
	
	void write(byte *buffer,uint32_t len)
	{
		//Escribe un número de bytes de un buffer
		fwrite(buffer,1,len,_f);
	}
	
	void seek(uint32_t len,arduinoSeekMode mode)
	{
		//Definimos el modo de posicionamiento en Linux
		//Por defecto, desde el principio
		int linuxMode = SEEK_SET;
		if(mode==SeekCur)
		  linuxMode=SEEK_CUR;
		else if(mode==SeekEnd)
		  linuxMode=SEEK_END;
		fseek(_f,len,linuxMode);
	}
};

//Clase de objeto SPIFFS
class SPIFFSclass
{
	public:
	void begin()
	{
		//Inicializa el objeto
		
		//Nada especial que hacer aquí
		//El método es declarado para guardar compatibilidad
	}
	
	File open(string filename,string options)
	{
		//Abre un archivo
		
		//Creamos el objeto File abriendo el archivo y lo devolvemos
		return File(filename,options);
	}
	
	bool remove(string filename)
	{
		//Elimina un archivo
		//Devuelve true si se ha conseguido eliminar
		
		//En Linux la función remove devuelve 0 cuando se consigue borrar
		//Puesto que el método se llama remove y la función para
		//eliminar archivos también, si no ponemos el prefijo std::
		//pensará que estamos llamando al mismo método y cuando se
		//llene la pila de llamadas cíclicas, el programa dará un error.
		return !std::remove(filename.c_str());
	}
};

//Creamos el objeto SPIFFS
SPIFFSclass SPIFFS;

#endif

