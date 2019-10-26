/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Control del brillo
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  Ejemplo de cómo se puede fijar el brillo de la matriz.
  El brillo puede tomar valores desde 0 a 255 (byte).
  Para comprobar mejor su variación, encenderemos todos los leds con colores
  aleatorios y variaremos el brillo.

  Verificamos que los led son muy brillantes y que con un brillo de 30 es más que suficiente.
*/

//Librería de gestión
#include <FastLED.h>

//Definimos el número de leds que tiene la matriz
#define NUM_LEDS 64
//Definimos el pin de datos
#define DATA_PIN 3

//Creamos un array con tantos elementos como leds para que contenga el color de cada 
//uno de ellos
CRGB leds[NUM_LEDS];

//Variable en la que guardaremos el nivel de brillo actual
byte brightness=0;
//Dirección de brillo
int delta=-1;

void setup()
{
  //Inicializamos la semilla de números aleatorios
  randomSeed(analogRead(0));
  //Indicamos que utilizamos chips compatibles con NEOPIXEL. Uno de ellos es el WS2812
  //La conectividad es de un sólo pin de datos
  //Notificamos cuálserá el pin de datos
  //Hacemos referencia al array en el que guardamos los colores de cada led
  //Y por último indicamos el número total de leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  //Recorremos todos los leds y les asignamos un color aleatorio
  for(byte led=0;led<NUM_LEDS;led++) leds[led]=CRGB(random(256),random(256),random(256));
  //Mostramos le configuracióna actual
  FastLED.show();
}

void loop()
{
  //Pasamos al siguiente nivel de brillo
  brightness+=delta;
  //Si hemos llegado al valor de rebote...
  if(brightness==0)
  {
    //...cambiamos la dirección
    delta=-delta;
    //y pasamos al siguiente valor. Esto evita que cuando delta es creciente (+1)
    //pasemos de máximo brillo a mínimo por un momento
    brightness+=delta;
  }
  //Aplicamos nivel de brillo
  FastLED.setBrightness(brightness);
  //Mostramos cambios
  FastLED.show();
  //Esperamos un momento
  delay(10);
}

