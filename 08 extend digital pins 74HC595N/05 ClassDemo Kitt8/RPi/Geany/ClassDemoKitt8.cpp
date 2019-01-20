/*
 * Autor: Ramón Junquera
 * Tema: extensor de pines digitales 74HC595N
 * Objetivo: Aprender el funcionamiento del chip 74HC595N
 * Material: Raspberry Pi, breadboard, 8x led, chip 74HC595N, cables
 * Descripción:
 * Ejercicio basado en el ejemplo para Arduino.
 * 
 * El chip 74HC595N sirve para aumentar el número de pines digitales
 * disponibles.
 * Concretamente es capaz de proporcionar 8 nuevos pines digitales de
 * salida. La ventaja es que para su control sólo necesitamos 3 pines.
 * Esta operación también se denomina 8-bit shift register.
 * 
 * El consumo del chip es muy bajo.
 * Soporta un máximo de 7V. Por lo tanto trabaja perfectamente tanto con
 * 5 como con 3.3V.
 * 
 * En este proyecto conectaremos un 74HC595N a la placa a través de los
 * 3 pines de gestión, para controlar 8 leds, que encenderemos
 * secuencialmente de uno en uno.
 * 
 * El chip tiene 8 patas por cada lado con la siguiente nomenclatura:
 * 
 *   VCC  Q0  DS OE STCP SHCP MR  Q7S
 *    │   │   │   │   │   │   │   │
 *   ███████████████████████████████
 *   ███████████████████████████████
 *    ███████ 74HC595N █████████████
 *   ███████████████████████████████
 *   ███████████████████████████████
 *    │   │   │   │   │   │   │   │
 *    Q1  Q2  Q3  Q4  Q5  Q6  Q7  GND  
 * 
 * Los pines marcados desde Q0 a Q7 son los pines de salida que
 * conectamos con el ánodo de los leds.
 * Los pines 8 (GND) y 13 (OE) los conectamos directamente a tierra.
 * Los pines 10 (MR) y 16 (VCC) los conectamos a VCC.
 * 
 * Los pines de gestión son:
 *   SHCP : clock
 *   STCP : latch
 *   DS   : data
 * 
 * Estos son los únicos pines que debemos conectar a la placa.
 * 
 * Los leds están ordenados de manera ascendente. El primer led (el de
 * la izquierda) corresponde con el primero de los pines expandidos
 * (Q0), el segundo con Q1, ...
 * Como el programa va encendiendo secuencialmente los pines expandidos,
 * veremos que el led encendido se mueve de izquierda a derecha.
 * 
 * Para utilizar este chip no es necesario cargar ninguna librería
 * especial.
 * Es suficiente con la función "shiftOut" que se encarga de enviar un
 * byte a través de un par de pines, uno hace de reloj y el otro es el
 * de datos.
 * Puesto que sólo se puede enviar un byte, en él se guarda el estado de
 * cada uno de los 8 leds.
 * Cada led corresponde a un bit.
 * El pin latch (seguro o pestillo en castellano) nos sirve para
 * indicarle al chip cuándo debe hacer caso a la información que se le
 * envía por los pines de datos y reloj.
 * Por eso, justo antes de utilizar la función shiftOut, "quitamos el
 * pestillo" y lo volvemos a activar al terminar.
 * 
 * El chip no contempla la lectura (input) de los nuevos pines
 * digitales, ni la escritura analógica.
 * 
 * Se incluye esquema del circuito en Fritzing.
 * 
 * 
 * Autor: Ramón Junquera
 * Tema: chip de extensión de pines digitales 74HC595N
 * Objetivo: Demostración de librería con leds y un chip
 * Material: breadboard, cables, 8 leds, chip 74HC595N, placa RPi
 * 
 * Descripción:
 * Haremos el efecto Kitt con 8 leds. Para ello necesitamos un sólo chip.
 * Creamos la configuración de los leds que se encenderán en binario y
 * pedimos que se muestre.
 * Después calculamos cuál será el siguiente led a encender.
 * 
 * Resultado:
 * Efecto Kitt con 8 leds
 */

#include <Arduino.h>
#include "RoJo74HC595N.cpp"

using namespace std;

int main(int argc, char **argv)
{
	//Definición de constantes globales
	const unsigned char pinLatch = 20;
	const unsigned char pinClock = 16;
	const unsigned char pinData = 21;
	
	//Declaramos el objeto que gestionará nuestro display
	//Como parámetros le pasamos los pines de conexión del 74HC595N
	//Y el número de chips conectados en cascada (=1)
	RoJo74HC595N display = RoJo74HC595N(pinData,pinLatch,pinClock,1);
	
	byte activeLed=0;
	int8_t delta=1;

    //Bucle infinito
    while (1)
    {
		//Calculamos en binario la configuración de leds que deben estar activos
		//Solo encendemos uno cada vez
		display.show(1<<activeLed);
		//Calculamos cuál será el siguiente led encendido
		activeLed+=delta;
		//Si hemos llegdo a un extremo...cambiamos de dirección
		if(activeLed==0 || activeLed==7) delta=-delta;
		//Esperamos un momento
		delay(100);
    }
}

