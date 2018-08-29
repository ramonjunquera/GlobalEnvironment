/*
 * Autor: Ramón Junquera
 * Fecha: 20180829
 * Tema: Elementos analógicos
 * Objetivo: Escritura de pines analógicos
 * Material: Raspberry Pi, breadboard, led
 * Descripción:
 * Haremos variar la intensidad de un led progresivamente desde apagado
 * hasta el máximo y después a la inversa.
 * 
 * Resultado:
 * El led se enciende y apaga progresivamente.
 */

#include <Arduino.h>

int main(int argc, char **argv)
{
    //Declaración de constantes globales
    const byte pinLed=16;
    
    //Declaración de variables globales
    byte level=0; //Nivel PWM
    int8_t delta=1; //Dirección
    
    while(1)
    {
		//Escribimos el nivel PWM en el pin del led
        analogWrite(pinLed,level);
        //Calculamos el valor del nivel del próximo ciclo
        level+=delta;
        //Si hemos llegado a alguno de los extremos...cambiamos de dirección
        if(level==0 || level==0xFF) delta=-delta;
        //Esperamos un momento
        delay(5);
	}
}

