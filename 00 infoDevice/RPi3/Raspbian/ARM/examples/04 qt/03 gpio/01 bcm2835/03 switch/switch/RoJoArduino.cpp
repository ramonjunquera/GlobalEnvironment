/*
 * Autor: Ramón Junquera
 * Descripción: Funciones Arduino para bcm2835 v1.00
 * 
 * Necesarios los siguientes parámetros para Build:
 *   g++ -Wall -std=c++11 -o "%e" "%f" -lbcm2835
 */
 
#ifndef RoJoArduino_h
#define RoJoArduino_h

#include <iostream>
#include <chrono> //Para millis y micros
#include "bcm2835.c"

#define PI 3.14159265

using namespace std;

//Definición de tipos de datos
typedef unsigned char byte;
typedef enum
{
    MSBFIRST=true,
    LSBFIRST=false
} arduinoOrder;
typedef enum
{
    INPUT=0,
    OUTPUT=1,
    INPUT_PULLUP=2,
    INPUT_PULLDOWN=3
} arduinoMode;
typedef enum
{
	CS0=0, //CS0
	CS1=1, //CS1
	CS2=2, //Ambos pines están unidos
	CS_NONE=3 //Controlado por soft, no por hard.
} arduinoChipSelect;
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

#define digitalWrite(pin,value) bcm2835_gpio_write(pin,value)
#define digitalRead(pin) bcm2835_gpio_lev(pin)
#define arduinoOpen() bcm2835_init()
#define arduinoClose() bcm2835_close()

void pinMode(byte pin,arduinoMode mode)
{
	switch(mode)
	{
		case INPUT:
			bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(pin,BCM2835_GPIO_PUD_OFF);
			break;
		case OUTPUT:
			bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_OUTP);
			break;
		case INPUT_PULLUP:
			bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(pin,BCM2835_GPIO_PUD_UP);
			break;
		case INPUT_PULLDOWN:
			bcm2835_gpio_fsel(pin,BCM2835_GPIO_FSEL_INPT);
			bcm2835_gpio_set_pud(pin,BCM2835_GPIO_PUD_DOWN);
			break;
	}
}

void shiftOut(byte pinData,byte pinClock,arduinoOrder bitOrder,byte value)
{
	if(bitOrder==MSBFIRST)
	{
		for(int i=7;i>=0;i--)
		{
			digitalWrite(pinData,value & (1 << i));
			digitalWrite(pinClock,HIGH);
			digitalWrite(pinClock,LOW);
		}
	}
	else
	{
		for(int i=0;i<8;i++)
		{
			digitalWrite(pinData,value & (1 << i));
			digitalWrite(pinClock,HIGH);
			digitalWrite(pinClock,LOW);
		}
	}
}

const chrono::high_resolution_clock::time_point time0 = chrono::high_resolution_clock::from_time_t(0);

unsigned long long millis()
{
	chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(now-time0).count();
}

unsigned long long micros()
{
	chrono::high_resolution_clock::time_point now = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::microseconds>(now-time0).count();
}

//Clase de emulación de librería Wire en Arduino
class WireArduino
{
	//Clase simulación de Wire en Arduino para la gestión de i2c

	private: int errorId=1;
	
	public: void begin()
	{
		//Inicialización de las conexiones i2c
		
		//Indicamos la velocidad de transmisión (400KHz)
		bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_148);
	}
	
	public: void beginTransmission(byte deviceId)
	{
		//Abre la conexión i2c con un dispositivo
		
		//Indicamos el identificador del dispositivo
		bcm2835_i2c_setSlaveAddress(deviceId);
		//Abrimos conexión
		//Si bcm2835_i2c_begin es correcto (==1)
		//guardamos en errorId 0 porque no hay errores
		errorId=(bcm2835_i2c_begin()==1?0:1);
	}
	
	public: byte endTransmission()
	{
		//Cierra la transmisión i2c
		//Devuelve el mensage de error. 0=sin error
		
		//Si no hubo errores...cerramos conexión
		if(!errorId) bcm2835_i2c_end();
		//Devolvemos el código de error
		return errorId;
	}
	
	public: void write(byte *buffer,int length)
	{
		//Envía un número determinado de bytes por i2c
		
		//Si no hay errores...lo envía
		if(!errorId) bcm2835_i2c_write((char *)buffer,length);
	}
};

//Creamos el objeto Wire
WireArduino Wire;

//Clase de emulación de librería SPI en Arduino
class SpiArduino
{
	public: int begin()
	{
		//Inicialización de las conexiones SPI
		//Devuelve 0 ante cualquier error
		
		return bcm2835_spi_begin();
	}
	
	public: void end()
	{
		//Finaliza las conexiones SPI
		
		bcm2835_spi_end();
	}
	
	public: void setClockDivider(arduinoSpiClockDivider SPI_CLOCK_DIV)
	{
		//Asigna la frecuencia de comunicación en función del divisor
		//de reloj.
		//Se guarda compatibilidad con los valores de Arduino, pero las
		//frecuencias no son las mismas.
		
		//La variable SPI_CLOCK_DIV puede tener los siguientes valores:
		
		//Valor            Arduino RaspPi2 RaspPi3
		//SPI_CLOCK_DIV2      8MHz  125MHz  200MHz
		//SPI_CLOCK_DIV4      4MHz   63MHz  100MHz
		//SPI_CLOCK_DIV8      2MHz   31MHz   50MHz
		//SPI_CLOCK_DIV16     1MHz   16MHz   25MHz
		//SPI_CLOCK_DIV32   500KHz    8MHz   13MHz
		//SPI_CLOCK_DIV64   250KHz    4MHz    6MHz
		//SPI_CLOCK_DIV128  125KHz    2MHz    3MHz
		
		//La librería bcm2835 permite divisores hasta el 65536.
		//Su tabla es:
		//Valor                             Raspberry Pi 2  Raspberry Pi 3
		//BCM2835_SPI_CLOCK_DIVIDER_65536   3.814697260kHz   6.10351560kHz
		//BCM2835_SPI_CLOCK_DIVIDER_32768   7.629394531kHz  12.20703125kHz
		//BCM2835_SPI_CLOCK_DIVIDER_16384  15.258789060kHz  24.41406250kHz
		//BCM2835_SPI_CLOCK_DIVIDER_8192   30.517578130kHz  48.82812500kHz
		//BCM2835_SPI_CLOCK_DIVIDER_4096   61.035156250kHz  97.65625000kHz
		//BCM2835_SPI_CLOCK_DIVIDER_2048  122.070312500kHz 195.31250000kHz
		//BCM2835_SPI_CLOCK_DIVIDER_1024  244.140625000kHz 390.62500000kHz
		//BCM2835_SPI_CLOCK_DIVIDER_512   488.281250000kHz 781.25000000kHz
		//BCM2835_SPI_CLOCK_DIVIDER_256   976.562500000kHz   1.56250000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_128     1.953125000MHz   3.12500000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_64      3.906250000MHz   6.25000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_32      7.812500000MHz  12.50000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_16 	   15.625000000MHz  25.00000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_8      31.250000000MHz  50.00000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_4      62.500000000MHz 100.00000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_2     125.000000000MHz 200.00000000MHz
		//BCM2835_SPI_CLOCK_DIVIDER_1       3.814697260kHz   6.10351560kHz
		
		//Aprovechamos que ambas enumeraciones son de tipo entero y que
		//su valor es el del divisor. Gracias a eso, asignamos el
		//divisor directamente.
		bcm2835_spi_setClockDivider(SPI_CLOCK_DIV);
	}
	
	
	public: void setDataMode(byte SPI_MODE)
	{
		//Asigna el modo de datos
		//La variable SPI_MODE puede tener los siguientes valores:
		
		//SPI_MODE CPOL CPHA
		//      0    0    0
		//      1    0    1
		//      2    1    0
		//      3    1    1
    
		bcm2835_spi_setDataMode(SPI_MODE);
	}
	
	public: void setBitOrder(arduinoOrder bitOrder)
	{
		//Indicamos el orden de envío
		
		//bitOrder puede tener los siguientes valores:
		//MSBFIRST=true
		//LSBFIRST=false
		
		bcm2835_spi_setBitOrder(bitOrder?BCM2835_SPI_BIT_ORDER_MSBFIRST:BCM2835_SPI_BIT_ORDER_LSBFIRST);
	}
	
	public: void transfer(byte d)
	{
		//Envía un byte por SPI
		
		bcm2835_spi_transfer(d);
	}
	
	public: void transfer(byte *buffer,int length,bool writeOnly)
	{
		//Envía y recibe un número determinado de bytes por SPI
		//Si writeOnly == true, no se modificará la información del buffer
		//Si es false, se devolverá la información recibida en el propio
		//buffer
		
		if(writeOnly)
		  bcm2835_spi_writenb((char *)buffer,length);
		else
		  bcm2835_spi_transfern((char *)buffer,length);
	}
	
	public: void chipSelect(arduinoChipSelect CS)
	{
		//Raspberry Pi es capaz de trabajar con dos conexiones SPI
		//simultáneas gracias a que tiene dos pines CS (Chip Select).
		//Con esta función seleccionamos el chip (pin) con el que
		//trabajamos.
		//Por defecto se trabaja con CS0
		
		//Valor              CS Observaciones
		//BCM2835_SPI_CS0     0 Opción por defecto.
		//BCM2835_SPI_CS1     1
		//BCM2835_SPI_CS2     2 Ambos pines están unidos
		//BCM2835_SPI_CS_NONE - Controlado por soft, no por hard.
		
		int value=0;
		switch(CS)
		{
			case CS0:
				value=BCM2835_SPI_CS0;
				break;
			case CS1:
				value=BCM2835_SPI_CS1;
				break;
			case CS2:
				value=BCM2835_SPI_CS2;
				break;
			case CS_NONE:
				value=BCM2835_SPI_CS_NONE;
				break;	
		}
		bcm2835_spi_chipSelect(value);
	}
};

//Creamos el objeto SPI
SpiArduino SPI;

#endif
