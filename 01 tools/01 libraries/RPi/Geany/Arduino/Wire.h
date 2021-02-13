/*
 * Autor: Ramón Junquera
 * Descripción: Gestión chip BCM2835 de Raspberry Pi 3. Emulación de
 *   librería Wire de Arduino
 * Versión: 20210210
 * 
 * Funciones I2C:
 *   void begin(byte i2cPort)
 *   void setClock(int clockFrequency)
 *   void beginTransmission(byte deviceId)
 *   byte endTransmission()
 *   uint32_t write(byte *buffer,uint32_t length)
 *   bool writeBufferLen(uint32_t len)
 *   bool writeBufferAdd(byte *source,uint32_t len)
 *   bool writeBufferAdd(byte value)
 *   void writeBufferReset()
 *   bool writeBufferSend()
 *   byte requestFrom(byte deviceId,byte length)
 *   byte read()
 *   byte available()
 *   void end()
 * 
 * Notas:
 *   RPi no permite pausar una comunicación I2C.
 *   Dentro de un mismo beginTransmission-endTransmission, el segundo
 *   write que se haga, se considerará una transmisión nueva.
 *   No la continuación de la anterior.
 *   Para paliear el inconveniente, se ha creado la famila de métodos
 *     de writeBuffer* que permite crear un buffer de escritura en el
 *     que se puede ir añadiendo información para después enviarlo
 *     completo.
 */
 
#ifndef RoJoArduinoWire_h
#define RoJoArduinoWire_h

using namespace std;

#include <Arduino.h>

//Funciones I2C

typedef enum {
	CS0=0, //CS0
	CS1=1, //CS1
	CS2=2, //Ambos pines están unidos
	CS_NONE=3 //Controlado por soft, no por hard.
} arduinoSpiChipSelect;
typedef enum {
	SPI_CLOCK_DIV2=2,
	SPI_CLOCK_DIV4=4,
	SPI_CLOCK_DIV8=8,
	SPI_CLOCK_DIV16=16,
	SPI_CLOCK_DIV32=32,
	SPI_CLOCK_DIV64=64,
	SPI_CLOCK_DIV128=128
} arduinoSpiClockDivider;

//Clase de emulación de librería Wire en Arduino
class WireArduino {
	//Clase simulación de Wire en Arduino para la gestión de I2C en
	//Raspberry Pi 3

	//Declaración de variables y métodos privados
	private:
	
	byte _i2cPort=-1; //Puerto I2C utilizado
	uint32_t cdiv; //Clock divider
	volatile uint32_t* dlen; //Puntero a registro BSC Master Data Length
    volatile uint32_t* fifo; //Puntero a registro BSC Master Data FIFO 
    volatile uint32_t* status; //Puntero a registro BSC Master Status
    volatile uint32_t* control; //Puntero a registro BSC Master Control
    byte *_readBuffer; //Array de buffer de lectura
    byte _pendingToRead=0; //Número de caracteres pendientes de leer del buffer de lectura
    byte _readBufferLen=0; //Tamaño del buffer de lectura
    byte *_writeBuffer; //Array de buffer de escritura
    uint32_t _writeBufferLen=0; //Tamaño del buffer de escritura
    uint32_t _writeBufferPos=0; //Posición de inserción de buffer de escritura
    
    //Define el puerto I2C que se utilizará
    void setPort(byte i2cPort) {
		//Raspberry Pi tiene tres puertos I2C maestros.
		//Estos son los pines y gpio utilizados para cada puerto y su uso:
		
		//               ---SDA-- ---SCL--
		//  i2cPort port pin gpio pin gpio Uso
		//  ------- ---- -------- -------- ----------------------------------
		//     0    BSC0  27   0   28   1  reservado para placas de expansión
		//     1    BSC1   3   2    5   3  disponible para su uso libre
		//     2    BSC2  --  --   --  --  reservado para la interfaz HDMI
		
		//Raspberry Pi 1 sólo tenía el puerto BSC0.
		//La versión 2 y 3 utilizan el puerto BSC1 como estándar, pero
		//el BSC0 sigue funcionando perfectamente.
		
		//Si no se ha inicializado el I2C...terminamos;
		if(_i2cPort<0) return;
		//I2C está inicializado
		
		//Anotamos en la variable privada el puerto indicado
		_i2cPort=i2cPort;
		//Definimos los pines que vamos a utilizar de pendiendo del puerto
		//y calculamos los punteros a los registros
		//Si es puerto es el 1...
		if(i2cPort) {
			_pinMode(2,4); //SDA = GPIO 2 = pin 3
			_pinMode(3,4); //SCL = GPIO 3 = pin 5
			dlen = bcm2835_bsc1 + 2;
			fifo = bcm2835_bsc1 + 4;
			status = bcm2835_bsc1 + 1;
			control = bcm2835_bsc1;
		} else { //El puerto es el 0
			_pinMode(0,4); //SDA = GPIO 0 = pin 27
			_pinMode(1,4); //SCL = GPIO 1 = pin 28
			dlen = bcm2835_bsc0 + 2;
			fifo = bcm2835_bsc0 + 4;
			status = bcm2835_bsc0 + 1;
			control = bcm2835_bsc0;
		}
	}
	
	//Declaración de métodos públicos:
	public:
	
	//Inicialización de las conexiones i2c
	//Se indica el puerto I2C que vamos a utilizar: 0 o 1
	//Por defecto el 1 que es el más habitual
	//Siempre se puede cambiar con setPort
	void begin(byte i2cPort=1) {
		//Si no se ha inicializado el chip BCM2835...terminamos;
		if(bcm2835_st == MAP_FAILED) return;
		//El chip BCM2835 se ha inicializado
		
		//Simulamos ya se ha inicializado anteriormente
		_i2cPort=1;
		//Por defecto la velocidad de comunicación es de 100kbit/s
		setClock(100000);
		//Dijamos el puerto I2C indicado
		setPort(i2cPort);
	}
	
	//Fija la velocidad de conexión
	void setClock(uint32_t clockFrequency) {
		//Según la función de Arduino, los posibles valores para 
		//clockFrequency son:
		//   10.000 low speed mode
		//  100.000 standard mode
		//  400.000 fast mode
		//1.000.000 fast mode plus
		//3.400.000 high speed mode
		
		//Raspberry Pi 3 tiene un reloj a 400MHz
		//En la función actual se permite cualquier frecuencia de reloj
		//desde 0 a 400.000.000.
		//De todas maneras el protocolo i2c tiene como velocidad estándar
		//100kbit/s.
		//Con los ajustes adecuados, podemos utilizarlo a 400kbit/s.
		//Estiste un estándar aun más rápido a 5Mbit/s, pero esta placa
		//no está preparada para soportarlo.
		//Las frecuencias de reloj para estas velocidades son:
		//  100kbit/s = 100.000
		//  400kbit/s = 400.000
		
		//Si no se ha inicializado el I2C...terminamos;
		if(_i2cPort<0) return;
		//I2C está inicializado
		
		//Dependiendo del modo utilizado BCM0 o BCM1, calcularemos la
		//dirección del registro en la que escribiremos el valor del
		//divisor del reloj.
		volatile uint32_t* paddr = (volatile uint32_t *)((_i2cPort?bcm2835_bsc1:bcm2835_bsc0)+5);
		
		//Calculamos el divisor del reloj para conseguir la frecuencia
		//deseada.
		//Recordemos que una Raspberry Pi 3 tiene un reloj a 400MHz.
		cdiv = 400000000/clockFrequency;
		//Guardamos el clock divider en el registro
		*paddr = cdiv;
	}
	
	//Abre la conexión I2C con un dispositivo
	void beginTransmission(byte deviceId) {
		//Si no se ha inicializado el I2C...terminamos;
		if(_i2cPort<0) return;
		//i2c está inicializado
		
		//Calculamos la dirección del registro que guarda el identificador
		//del dispositivo, dependiendo del modo
		volatile uint32_t* paddr = (volatile uint32_t *)((_i2cPort?bcm2835_bsc1:bcm2835_bsc0)+3);
		//Guardamos el identificador del dispositivo
		*paddr = deviceId;
	}
	
	//Cierra la transmisión I2C
	//Devuelve el mensage de error. 0=sin error
	byte endTransmission() {
		//Nada especial para cerrar una transmisión
		//Se mantiene por compatibilidad con Arduino
		//Todo correcto siempre
		return 0;
	}
	
	//Envía un número determinado de bytes por i2c
	//Devuelve el número de bytes enviados
	uint32_t write(byte *buffer,uint32_t length) {
		//Nota. En RPi no se puede pausar la transmisión.
		//Esto significa que la segunda de dos llamadas consecutivas
		//no será continuación de la primera.
		
		//Si no se ha inicializado el I2C...terminamos sin transmitir;
		if(_i2cPort<0) return 0;
		//I2C está inicializado
		
		//Definición de variables
		uint32_t remaining = length; //Número de bytes pendientes de ser enviados
		uint32_t nextPos = 0; //Siguiente posición del buffer a enviar
		
		//Clear FIFO
		//Informamos que queremos vaciar la cola FIFO.
		//No utilizamos la nomenclatura *control!=0x20; porque sino da un
		//warning diciendo que la variable no se utiliza
		*control = *control | 0x20;
		
		//Clear Status
		*status = 0x00000302;
		//Guardamos el tamaño de bytes a enviar en el registro
		*dlen=length;
		//Llenamos la cola con los primeros valores
		//El tamaño del buffer de la cola FIFO de I2C es de 16 bytes
		//Mientras haya bytes pendientes por enviar y no hayamos llenado
		//la cola...
		while(remaining && (nextPos<16)) {
			//Añadimos el byte que corresponde a la cola FIFO
			//Después aumentamos el índice del siguiente byte a enviar
			*fifo = buffer[nextPos++];
			//Ya queda un byte menos por enviar
			remaining--;
		}
		//Activamos el dispositivo I2C y comenzamos a transferir
		*control = 0x00008080;
		//Mientras no se haya completado la transferencia
		while(!(*status & 0x00000002)) {
			//Mientras quede algo por transferir y la cola acepte más datos...
			while (remaining && (*status & 0x00000010)) {
				//Añadimos un nuevo byte a la cola FIFO
				//Después aumentamos el índice del siguiente byte a enviar
				*fifo = buffer[nextPos++];
				//Ya queda un byte menos por enviar
				remaining--;
			}
		}
		//Hemos terminado la transferencia
		//Es posible que hayamos tenido errores
		
		//Indicamos que hemos terminado
		*control |= 0x00000002;
		//Devolvemos el número de bytes enviados
		return length-remaining;
	}
	
	//Define el tamaño del buffer de escritura
	//Devuelve true si lo consigue
	bool writeBufferLen(uint32_t len) {
		if(!len) return 0;
		if(_writeBuffer) free(_writeBuffer);
		_writeBuffer=(byte*)malloc(len);
		_writeBufferLen=0;
		_writeBufferPos=0;
		if(_writeBuffer) _writeBufferLen=len;
		return _writeBufferLen>0;
	}
	
	//Añade al buffer de escritura otro buffer
	//Devuelve true si lo consigue
	bool writeBufferAdd(byte *source,uint32_t len) {
		if(_writeBufferPos+len>_writeBufferLen) return false;
		memcpy(&_writeBuffer[_writeBufferPos],source,len);
		_writeBufferPos+=len;
		return true;
	}
	
	//Añade un byte al buffer de escritura
	//Devuelve true si lo consigue
	bool writeBufferAdd(byte value) {
		return writeBufferAdd(&value,1);
	}
	
	//Borra el contenido del buffer de escritura
	void writeBufferReset() {
		_writeBufferPos=0;
	}
	
	//Envía el buffer de escritura por I2C
	//Devuelve true si lo consigue
	bool writeBufferSend() {
		return _writeBufferPos==write(_writeBuffer,_writeBufferPos);
	}
	
	//Solicita un número de bytes determinados a un dispositivo
	//que después serán leidos con read()
	//Devuelve el número de bytes recibidos
	byte requestFrom(byte deviceId,byte length) {
		//Si aun quedaba algo por leer...borramos el buffer de lectura
		if(_pendingToRead) free(_readBuffer);
		//Creamos un nuevo buffer de lectura tan grande como el solicitado
		_readBuffer=(byte*)malloc(_readBufferLen);
		if(!_readBuffer) return 0; //Si no hay memoria...terminamos
		//Anotamos que el nuevo tamaño del buffer de lectura y el
		//número de caracteres pendientes por leer tiene el mismo valor
		//que el número de caracteres solicitado
		_readBufferLen=_pendingToRead=length;
		//Abrimos transmisión con el dispositivo
		beginTransmission(deviceId);
		//Llenamos el buffer con datos leidos del dispositivo
		//bcm2835_i2c_read((char*)readBuffer,length);
		
		//Caracteres pendientes por leer
		uint32_t remaining = length;
		uint32_t i = 0;
		
		//Clear FIFO
		//Informamos que queremos vaciar la cola FIFO.
		//No tilizamos la nomenclatura *control!=0x20; porque sino da un
		//warning diciendo que la variable no se utiliza
		*control = *control | 0x20;
		//Clear Status
		*status = 0x00000302;
		//Guardamos el tamaño de bytes a recibir en el registro
		*dlen=length;
		//Activamos el dispositivo i2c y comenzamos a recibir
		*control = 0x00008081;
		//Mientras no se haya completado la transferencia
		while(!(*status & 0x00000002)) {
			//Mientras la cola FIFO contenga datos...
			while (*status & 0x00000020) {
				//Leemos un nuevo byte de la cola FIFO
				//Después disminuimos el índice del siguiente byte a recibir
				_readBuffer[i++] = *fifo;
				//Ya queda un byte menos por recibir
				remaining--;
			}
		}
		//Hemos terminado la transferencia
		//Es posible que hayamos tenido errores

		//Mientras nos queden datos por recibir y haya datos en la cola...
		while (remaining && (*status & 0x00000020)) {
			//Leemos un nuevo byte de la cola FIFO
			//Después disminuimos el índice del siguiente byte a recibir
			_readBuffer[i++] = *fifo;
			//Ya queda un byte menos por recibir
			remaining--;
		}
		
		//Si hubo algún error general o de timeout...
		if (*status & 0x00000300) {
			//...anotamos que no hay nada que leer 
			_pendingToRead=0;
			//borramos el buffer de lectura
			delete[] _readBuffer;
		}
		//Si aun quedan datos por recibir...
		else if (remaining) {
			//Solo tendremos disponibles algunos de los datos esperados:
			//el esperado menos los que no se han recibido
			//Anotamos este valor como datos pendientes de leer y
			//tamaño total del buffer de lectura
			_pendingToRead=_readBufferLen=length-remaining;
		}
		//Si se han recibido todos los datos solicitados sin errrores
		//ya se ha anotado en un inicio que el tamaño del buffer de
		//lectura y el número de bytes pendientes por leer corresponden
		//con el tamaño esperado
		
		//Indicamos que hemos terminado
		*control |= 0x00000002;
		//Devolvemos el número de bytes recibidos y pendientes de leer
		return _pendingToRead;
	}
	
	//Devuelve el siguiente byte del buffer de lectura
	byte read() {
		//Si no hay nada que leer...devolvemos carácter 0
		if(!_pendingToRead) return 0;
		//Hay algo que leer
	
		//Anotamos el carácter siguiente
		byte answer=_readBuffer[_readBufferLen-_pendingToRead];
		//Reducimos el número de caracteres pendientes de leer
		//Si ya no hay más...borramos el buffer
		if(!--_pendingToRead) delete[] _readBuffer;
		//Devolvemos el carácter siguiente
		return answer;
	}
	
	//Devuelve el número de bytes pendientes por leer del buffer de
	//lectura
	byte available() {
		return _pendingToRead;
	}
	
	//Finalizamos el uso del puerto I2C
	void end() {
		//Si no se ha inicializado el I2C...terminamos
		if(_i2cPort<0) return;
		//I2C está inicializado
		
		//Restauramos el modo INPUT a los pines utilizados por el puerto
		if(_i2cPort) { //El puerto es el 1
			_pinMode(2,0); //SDA = GPIO 2 = pin 3
			_pinMode(3,0); //SCL = GPIO 3 = pin 5
		}
		else { //El puerto es el 0
			_pinMode(0,0); //SDA = GPIO 0 = pin 27
			_pinMode(1,0); //SCL = GPIO 1 = pin 28
		}
		//I2C queda sin configurar
		_i2cPort=-1;
		
		if(_readBuffer) {
			free(_readBuffer);
			_readBufferLen=0;
		}
		if(_writeBuffer) {
			free(_writeBuffer);
			_writeBufferLen=0;
		}
	}
};

//Creamos el objeto Wire
WireArduino Wire;

#endif

