/*
 * Autor: Ramón Junquera
 * Tema: Gestión de interrupciones
 * Objetivo: Aprender a utilizar la gestión de interrupciones
 * Material adicional: breadboard, cables, interruptor de pulsador,
 *   placa RPi
 * 
 * Descripción:
 * La instrucción que nos permite relacionar una función con una
 * interrupción es: attachInterrupt.
 * La sintaxis correcta es:
 * attachInterrupt(pin, función de interrupción,tipo de interrupción)
 * Detallaremos cada uno de ellos:
 *   pin
 *     Es el pin para el que se creará la interrupción. Puede ser
 *     cualquiera.
 *   Función de interrupción.
 *     Es el nombre de una función que debemos tener definida, a la que
 *     se llamará cuando se de la situación de interrupción.
 *     La función no puede devolver valores, pero tendrá dos parámetros.
 *     Su sintaxis será la siguiente:
 *       void myFunc(byte pin,byte status)
 *     Donde pin corresponde al pin afectado y status al estado actual.
 *   Tipos de interrupción.
 *     Es la situación que provoca la interrupción.
 *     No se pueden definir dos tipos de interrupción distintas para la
 *     misma interrupción. Sólo tomará la última. Por ejemplo, si
 *     queremos que salte la interrupción cuando su pin asociado pase de
 *     estado HIGH a LOW, y además queremos que salte otra interrupción
 *     cuando pase de LOW a HIGH, no tendremos más remedio que definir
 *     una sóla función que sea llamada cuando cambie de estado sin
 *     importar a cual. Después ya gestionaremos el caso dentro de la
 *     misma función de interupción.
 *     Tenemos los siguientes tipos:
 *     - CHANGE. Cuando el pin cambia de estado.
 *     - RISING. Cuando el pin pasa de estado LOW a HIGH
 *     - FALLING. Cuando el pin pasa de estado HiGH a LOW
 * 
 * El comportamiento del flujo del programa dentro de una interrupción
 * idéntico al habitual puesto que la función de interrupción se lanza
 * en un thread independiente al del programa y corren en paralelo.
 * Sólo hay que tener en cuenta que:
 * - Sólo se puede ejecutar una función de interrupción concurrente para
 *   una misma interrupción.
 * 
 * Además de la instrucción attachInterrupt tenemos las siguientes
 * funciones útiles para trabajar con interrupciones:
 * - void detachInterrupt(byte pin) : Desactiva una interrupción concreta
 * 
 * El ejemplo actual aplica el estado inverso del pin del interruptor al
 * led.
 * Es inverso porque hemos activado las resistencias de pullup del pin
 * del interruptor y cuando no se pulsa su estado será HIGH.
 * La diferencia con la técnica de polling es que no estamos
 * contínuamente consultando el estado del interruptor.
 * Sólo lo hacemos cuando tenemos la certeza que ha cambiado.
 * Esto nos permite dedicar el tiempo de proceso a otras tareas.
 * 
 * Resultado:
 * Cuando pulsamos el interruptor, se enciende el led integrado.
 */

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = 21; //Pin del led integrado
const byte pinSwitch = 20; //pin al que está conectado el interruptor

void IntSwitch(byte pin,byte status)
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Aplicamos al led el estado inverso al del interruptor
  digitalWrite(pinLed,!digitalRead(pinSwitch));
}

int main(int argc, char **argv)
{
	//Configuramos el pin del led para que sea de salida
	pinMode(pinLed,OUTPUT);
	
	//Activamos las resistencias de pull-up para el pin de interruptor
	//Por defecto el pin del interruptor estará en HIGH y cuando se
	//pulse en LOW
	pinMode(pinSwitch,INPUT_PULLUP);
	
	//Definimos la interrupción
	//Será la correspondiente al pin del interruptor
	//Se llamará a la función IntSwitch
	//Ocurrirá cuando el estado del pin cambie
	attachInterrupt(pinSwitch,IntSwitch,CHANGE);
  
    //Bucle infinito
    while(1)
    {
		//Nada especial que hacer aquí
	}
}

