/*
 * Autor: Ramón Junquera
 * Descripción: Gestión chip BCM2835 de Raspberry Pi 3. Emulación de
 *   librería SPI de Arduino
 * Versión: 20171104
 * 
 * Funciones SPI:
 *   void begin()
 *   void end()
 *   void setClockDivider(arduinoSpiClockDivider divider)
 *   void setDataMode(byte mode)
 *   void setBitOrder(arduinoOrder bitOrder)
 *   void chipSelect(arduinoSpiChipSelect CS)
 *   byte transfer(byte d)
 *   void transfer(byte *buffer,uint32_t length,bool writeOnly)
 *   uint16_t transfer16(uint16_t d)
 *   void writeBytes(byte *buffer,uint32_t length)
 *   void setHwCs(bool status)
 * 	 void beginTransaction(SPISettings *spiSettings)
 *   void endTransaction()
 */

#ifndef RoJoArduinoSPI_h
#define RoJoArduinoSPI_h

using namespace std;

#include <Arduino.h>

//Funciones SPI

typedef enum
{
	CS0=0, //CS0
	CS1=1, //CS1
	CS2=2, //Ambos pines están unidos
	CS_NONE=3 //Controlado por soft, no por hard.
} arduinoSpiChipSelect;
typedef enum
{
	SPI_CLOCK_DIV2=2,
	SPI_CLOCK_DIV4=4,
	SPI_CLOCK_DIV8=8,
	SPI_CLOCK_DIV16=16,
	SPI_CLOCK_DIV32=32,
	SPI_CLOCK_DIV64=64,
	SPI_CLOCK_DIV128=128
} arduinoSpiClockDivider;
typedef enum
{
	SPI_MODE0=0,
	SPI_MODE1=1,
	SPI_MODE2=2,
	SPI_MODE3=3
} arduinoSpiMode;


//Clase de emulación de objeto SPISettings
class SPISettings
{
	//Guarda las propiedades de una conexión SPI
	//Definido para guardar compatibilidad con Arduino
	//Sólo almacena las propiedades en variables públicas
	
	//Declaración de métodos públicos:
	public:
	
	uint32_t freq;
	arduinoOrder order;
	arduinoSpiMode mode;
	
	SPISettings(uint32_t spiFreq, arduinoOrder spiOrder, arduinoSpiMode spiMode)
	{
		//Constructor para almacenar los parámetros
		
		//Guardamos los parámetros en las variables públicas
		freq=spiFreq;
		order=spiOrder;
		mode=spiMode;		
	}
	
	SPISettings()
	{
		//Constructor estándard para declaración de objeto
	}
};


//Clase de emulación de librería SPI de Arduino
class SpiArduino
{
	//Clase simulación de SPI.h en Arduino para la gestión de SPI en
	//Raspberry Pi 3

	//Declaración de variables privadas
	private:
	bool enabled = false; //Aun no se ha activado
	
	//Declaración de métodos públicos:
	public:
	
	void begin()
	{
		//Inicialización de las conexiones SPI
		
		//Si no se ha inicializado el chip BCM2835...terminamos;
		if(bcm2835_st == MAP_FAILED) return;
		//El chip BCM2835 se ha inicializado
		
		//Configuramos los pines que vamos a utilizar
		_pinMode(7,4); //CE1 = GPIO 7 = pin 26
		_pinMode(8,4); //CE0 = GPIO 8 = pin 24
		_pinMode(9,4); //MISO = GPIO 9 = pin 21
		_pinMode(10,4); //MOSI = GPIO 10 = pin 19
		_pinMode(11,4); //CLK = GPIO 11 = pin 23

		//Inicializamos los registros de SPI, pines CS
		volatile uint32_t* paddr = bcm2835_spi0;
		*paddr = 0; //Inicializamos el registro a cero
		*paddr = 0x30; //Limpiamos las colas FIFO de recepción y envío
		//Hemos terminado de activar el protocolo SPI
		enabled=true;
	}
	
	void end()
	{
		//Finaliza las conexiones SPI
		
		//Volvemos a dejar los pines en modo entrada
		_pinMode(7,0); //CE1
		_pinMode(8,0); //CE0
		_pinMode(9,0); //MISO
		_pinMode(10,0); //MOSI
		_pinMode(11,0); //CLK
	}
	
	void setClockDivider(arduinoSpiClockDivider divider)
	{
		//Asigna la frecuencia de comunicación en función del divisor
		//de reloj.
		//Se guarda compatibilidad con los valores de Arduino, pero las
		//frecuencias no son las mismas.
		
		//La variable divider puede tener los siguientes valores:
		
		//Valor            Arduino RaspPi2 RaspPi3
		//SPI_CLOCK_DIV2      8MHz  125MHz  200MHz
		//SPI_CLOCK_DIV4      4MHz   63MHz  100MHz
		//SPI_CLOCK_DIV8      2MHz   31MHz   50MHz
		//SPI_CLOCK_DIV16     1MHz   16MHz   25MHz
		//SPI_CLOCK_DIV32   500KHz    8MHz   13MHz
		//SPI_CLOCK_DIV64   250KHz    4MHz    6MHz
		//SPI_CLOCK_DIV128  125KHz    2MHz    3MHz
		
		//Si no se ha activado el protocolo SPI...terminamos
		if(!enabled) return;
		//El protocolo SPI está activo
		
		//Asignamos el valor del divisor al registro
		volatile uint32_t* paddr = bcm2835_spi0 + 2;
		*paddr = divider;
	}
	
	void setDataMode(byte mode)
	{
		//Asigna el modo de datos
		//La variable mode puede tener los siguientes valores:
		
		//mode CPOL CPHA
		//  0    0    0
		//  1    0    1
		//  2    1    0
		//  3    1    1
    
		//Si no se ha activado el protocolo SPI...terminamos
		if(!enabled) return;
		//El protocolo SPI está activo
		
		//Aplicamos el modo indicado
		volatile uint32_t* paddr = bcm2835_spi0;
		*paddr = (*paddr & ~0b1100) | (mode << 2);
	}
	
	void setBitOrder(arduinoOrder bitOrder)
	{
		//Indicamos el orden de envío
		//El protocolo SPI de Raspberry Pi sólo soporta el orden MSBFIRST
		//Esta función sólo existe para guardar compatibilidad con Arduino
	}
	
	void chipSelect(arduinoSpiChipSelect CS)
	{
		//Raspberry Pi es capaz de trabajar con dos conexiones SPI
		//simultáneas gracias a que tiene dos pines CS (Chip Select).
		//Con esta función seleccionamos el chip (pin) con el que
		//trabajamos.
		//Por defecto se trabaja con CS0
		
		//CS      Observaciones
		//------- ---------------------------------
		//CS0     Activado CS0. Opción por defecto.
		//CS1     Activado CS1
		//CS2     Ambos pines están unidos
		//CS_NONE Controlado por soft, no por hard.
		
		//Si no se ha activado el protocolo SPI...terminamos
		if(!enabled) return;
		//El protocolo SPI está activo

		//Fijamos el Chip Select en el registro
		volatile uint32_t* paddr = bcm2835_spi0;
		*paddr = (*paddr & ~0b11) | CS;
	}
	
	void transfer(byte *buffer,uint32_t length,bool writeOnly)
	{
		//Envía y recibe un número determinado de bytes por SPI
		//Si writeOnly == true, no se modificará la información del buffer
		//Si es false, se devolverá la información recibida en el propio
		//buffer
		
		//Definición de variables
		volatile uint32_t* paddr = bcm2835_spi0; //Dirección registro CS
		volatile uint32_t* fifo = bcm2835_spi0 + 1; //Dirección registro cola FIFO
		byte receivedByte=0; //byte recibido
		uint32_t TXCnt=0; //Contador de bytes transferidos
		uint32_t RXCnt=0; //Contador de bytes recibidos

		//Incluimos esta línea sólo para que el compilador no de el 
		//warning de que la variable receivedByte no se utiliza
		TXCnt=receivedByte;

		//Limpiamos las colas FIFO de envío y recepción
		*paddr |= 0x00000030;
		//Activamos la transferencia
		*paddr |= 0x00000080;
		//Mientras no se haya recibido o transferido todo...
		while((TXCnt < length)||(RXCnt < length))
		{
			//Mientras la cola FIFO de transferencia acepte datos y no
			//hayamos transferido todo...
			while(((*paddr & 0x00040000))&&(TXCnt < length))
			{
				//...escribimos en la cola FIFO de transaferencia el
				//byte que corresponde y aumentamos el contador para el
				//próximo
				*fifo=buffer[TXCnt++];
			}
			
			/* Rx fifo not empty, so get the next received bytes */
			//Mientras la cola FIFO de recepción contenga datos y no
			//hayamos recibido todo...
			while(((*paddr & 0x00020000))&&(RXCnt < length))
			{
				//Si sólo tenemos que enviar...
				if(writeOnly)
				{
					//...leemos la cola FIFO de recepción en una
					//variable que no usaremos
					receivedByte = *fifo;
				}
				else //Tenemos que enviar y recibir
				{
					//...leemos la cola FIFO de recepción en la posición
					//del buffer que le corresponde
					buffer[RXCnt] = *fifo;
				}
				//Aumentamos el contador de bytes recibidos
				RXCnt++;
			}
		}
		//Mientras la transferencia no esté finalizada...
		while (!(*paddr & 0x00010000));
		//Desactivamos la transferencia
		*paddr &= ~0x00000080;
	}
	
	byte transfer(byte d)
	{
		//Envía y recibe un byte por SPI
		
		//Utilizamos la función de transferencia de buffer
		transfer(&d,1,false);
		//Devolvemos el valor recibido
		return d;
	}
	
	uint16_t transfer16(uint16_t d)
	{
		//Envía y recibe un uint16_t por SPI
		
		//En RPi los bytes de un uint16_t están intercambiados respecto
		//a Arduino. Por lo tanto los tendremos que tratar de manera 
		//distinta
		
		//Intercambiamos los bytes del uint_16
		d=(d>>8)|(d<<8);
		//Utilizamos la función de transferencia de buffer
		transfer((byte*)&d,2,false);
		//Intercambiamos los bytes del uint_16 recibido
		d=(d>>8)|(d<<8);

		//Devolvemos el valor recibido
		return d;
	}
	
	void writeBytes(byte *buffer,uint32_t length)
	{
		//Sólo envía un número de bytes
		
		//Utilizamos la función de transferencia de buffer
		transfer(buffer,length,true);
	}
	
	void setHwCs(bool status)
	{
		//Indica si se desea utilizar el CS por hardware
		
		//En RPi siempre se utilizan los pines CS. No se pueden
		//desactivar.
		//La función se conserva sólo por compatibilidad
	}
	
	void beginTransaction(SPISettings spiSettings)
	{
		//Aplica los parámetros SPI indicados e inicia una transacción
		//En RPi no es necesario iniciar una transacción. Se puede
		//enviar información en cualquier momento.
		//El método se define para guardar compatibilidad con Arduino y
		//para aplicar una colección de propiedades SPI de una vez
		
		//pendiente probar
		
		//No tenemos una función que traduzca frecuencia a una de las
		//configuraciones de reloj posibles.
		//Lo adaptamos aquí
		
		//Calculamos el divisor exacto para la frecuencia solicitada
		byte divCalc=400000000/spiSettings.freq;
		
		//Los valores del divisor están restringidos a potencias de 2
		//Es posible que el divisor calculado no lo sea
		
		//Definimos el divisor que tendrá el valor correcto
		byte div=128;
		//Mientras no coincida el divisor correcto con el bit de mayor
		//peso del divisor exacto...desplazaremos el divisor correcto en
		//una posición a la derecha
		while(!(divCalc & div)) {div>>=1;}
		//Tenemos en div el divisor con el valor reconocido que mejor se
		//adapta a la frecuencia solicitada, siempre ajustando a la baja

		//Aplicamos el divisor obtenido
		setClockDivider((arduinoSpiClockDivider)div);
		
		//En RPi el orden no se tiene en cuenta porque sólo existe la
		//configuración de MSBFIRST. No hacemos nada
		
		//Fijamos el modo
		setDataMode(spiSettings.mode);
	}
	
	void endTransaction()
	{
		//Finaliza una transacción
		//En RPi no es necesario finalizar una transacción
		//El método se define para guardar compatibilidad con Arduino		
	}
};

//Creamos el objeto SPI
SpiArduino SPI;

#endif

