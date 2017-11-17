/*
  Autor: Ramón Junquera
  Tema: Display de segmentos de led de 4 dígitos
  Objetivo: Ejemplo de multiplexación
  Material adicional: breadboard, cables, 4-digit display, 4 resistencias de 220 ohmios, Arduino UNO
  Descripción:
  El ejemplo es muy similar al usado al gestional dos display de un dígito.
  Se ha cambiado el tamaño de los arrays que guardan los pines de los cátodos y el que tiene el número
  descompuesto en dígitos.
  También se ha tenido que cambiar el tiempo de espera para que no aprecie el parpadeo.
    
  Resultado:
  Vemos cómo se muestran los números del 0000 al 9999 sin percibir parpadeos
*/

#include <Arduino.h>
#include "RoJoDisplayDigit1.h"

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines a los que hemos conectado cada segmento
RoJoDisplayDigit1 display = RoJoDisplayDigit1(13,12,11,10,9,8,7,6);
//Array con los pines de los cátodos comunes de cada display
const byte catodePin[] = {2,3,4,5};
//Array en la que guardaremos los dígitos del número que queremos mostrar (decenas, unidades)
byte numbers[4];
//Definimos el tiempo en ms que se muestran un dígito en un display
const int delayTime = 5;  

void setup()
{
  //Configuramos los pines de los cátodos comunes como salida y los apagamos
  for(byte i=0;i<4;i++)
  {
    pinMode(catodePin[i],OUTPUT);
    digitalWrite(catodePin[i],HIGH);
  }
}

void loop()
{
  //Mostraremos los números del 0 al 99...
  for(uint16_t n=0;n<10000;n++)
  {
    //Calculamos los dígitos a mostrar
    numbers[0]=n%10; //Unidades
    numbers[1]=(n/10)%10; //Decenas
    numbers[2]=(n/100)%10; //Centenas
    numbers[3]=n/1000; //Unidades de millar
    //Repetimos un número determinado de veces el refresco de los dígitos en los displays
    for(byte r=0;r<4;r++)
    {
      //Recorremos las posiciones de los dígitos que componen el número a representar
      for(byte pos=0;pos<4;pos++)
      {
        //Activamos la configuración que muestra el número de la posición actual
        display.set(numbers[pos],false);
        //Activamos ese display
        digitalWrite(catodePin[pos],LOW);
        //Esperamos el tiempo indicado con los leds encendidos
        delay(delayTime);
        //Apagamos el display
        digitalWrite(catodePin[pos],HIGH);
      }
    }
  }
}

