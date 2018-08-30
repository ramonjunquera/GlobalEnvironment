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

//Definición de constantes globales
const byte pinCount=8; //Número de pines/leds
const byte pinLeds[pinCount]={18,23,24,25,12,16,20,21};
//Definición de variables globales
int16_t levels[pinCount]; //Array de niveles para cada pin

void ApplyAndFade()
{
  //Aplica los niveles de todos los canales y los apaga un poco
  
  //Recorremos todos los canales utilizados y aplicamos sus niveles
  for(byte c=0;c<pinCount;c++) analogWrite(pinLeds[c],levels[c]);
  //Recorremos todos los canales utilizados...
  for(byte c=0;c<pinCount;c++)
  {
    //Reducimos el nivel del canal en un valor fijo
    levels[c]-=5;
    //Si hemos reducido demasiado, como mínimo lo dejaremos en 0
    if(levels[c]<0) levels[c]=0;
  }
}

int main(int argc, char **argv)
{
	//Declaración de variables locales
	byte channel=0; //Led procesado actualmente
	int8_t delta=1; //Dirección
	
	//Inicialmente todos los niveles están a cero
	for(byte c=0;c<pinCount;c++) levels[c]=0;
    
    while(1)
    {
		//Calculamos el valor del nuevo canal
		channel+=delta;
		//Si hemos llegado a un extremo...cambiamos la dirección
		if((channel==0) || (channel==pinCount-1)) delta=-delta;
		//Aplicamos el nivel máximo al canal afectado
		levels[channel]=0xFF;
		//Repetimos un bucle que sólo baja la intensidad de todos los
		//leds en cada ciclo
		for(byte z=0;z<14;z++)
		{
			//Aplicamos configuración actual
			ApplyAndFade();
			//Esperamos un momento
			delay(10);
		}
	}
}

