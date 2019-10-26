/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Uso de funcions random
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  En el siguiente ejemplo se hace uso de las funciones random8 y random16 que se 
  incluyen en la librería FastLED.
  Sirven para generar números aleatorio de un byte o enteros hasta cierto límite.

  El programa muestra parte del espectro de colores y dependiendo de la función
  random8 decide si debe encender un led en blanco.

  Como los colores de todos los leds se actualizan contínuamente, el tiempo
  que permatece encendido el led en blanco es muy poco.
*/

//Librería de gestión
#include "FastLED.h"

//Definimos el número de leds que tiene la matriz
#define NUM_LEDS 64
//Definimos el pin de datos
#define DATA_PIN 3

//Creamos un array con tantos elementos como leds para que contenga el color de cada 
//uno de ellos
CRGB leds[NUM_LEDS];

//Variable en la que guardaremos el color inicial (formato HUE)
byte hue=0;

void setup()
{
  //Indicamos que utilizamos chips compatibles con NEOPIXEL. Uno de ellos es el WS2812
  //La conectividad es de un sólo pin de datos
  //Notificamos cuálserá el pin de datos
  //Hacemos referencia al array en el que guardamos los colores de cada led
  //Y por último indicamos el número total de leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  //Fijamos el brillo a 30
  LEDS.setBrightness(30);
}

void loop()
{
  //Mostramos parte de la gama de colores comenzando por la referencia de la variable hue, al mismo
  //tiempo que la aumentamos en 1 para su próximo uso
  fill_rainbow(leds,NUM_LEDS,hue++,1);
  //Si un valor aleatorio de 8 bits = 1 byte = [0,255], es inferior a cierto valor...
  if( random8()<40)
  {
    //...entonces encenderemos aleatoriamente un led en blanco para que brille más que los demás
    //random16(max genera un valor aleatorio entero en el rango [0,max-1]
    //Seleccionamos un led al azar y le asignamos el color blanco
    leds[ random16(NUM_LEDS) ] = CRGB::White;
  }
  //Mostramos cambios
  FastLED.show();
  //Esperamos un momento
  delay(10);
}


