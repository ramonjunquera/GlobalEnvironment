/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Uso de función rainbow
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  La librería FastLED tiene funciones avanzadas para la gestión de la matriz.
  En el siguiente ejemplo se emplica el funcionamiento de la función fill_rainbow
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
  //Función: fill_rainbow
  //Parámetros:
  //  leds: matriz de elementos CRGB donde se guarda el color de cada led
  //  NUM_LEDS: número total de leds
  //  hue: referencia hue del color del primer led
  //  delta: diferencia de hue que se aplicará entre leds
  //    Una diferencia 0 hará que todos los leds de la matriz tengan el mismo color
  //    Una diferencia de 1 haráa que la variación de color entre el primer y último led
  //    sea de 64 unidades. No se verá la gama completa de colores
  //    Una diferencia de 4 haraá que se vea la gama de colores completa porque son 256 tonos
  //    y si aumentamos el color en 4 unidades por led, en 64 saltos hemos completado todos.

  //Mostramos toda la gama de colores comenzando por la referencia de la variable hue, al mismo
  //tiempo que la aumentamos en 1 para su próximo uso
  fill_rainbow(leds,NUM_LEDS,hue++,4);
  //Mostramos cambios
  FastLED.show();
  //Esperamos un momento
  delay(10);
}


