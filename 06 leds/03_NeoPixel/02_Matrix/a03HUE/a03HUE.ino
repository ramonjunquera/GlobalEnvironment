/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Control del color con hue
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  El color de un led habitualmente viene dado por las intensidades de cada uno de los
  colores básicos RGB.
  Así definimos la cantidad de cada color básico que tiene el color final, que será
  la mezcla.
  Si ponemos mucha cantidad de todos los colores, el resultado final será más brillante.

  Hay otro método de definición de color llamado HSV o HSB.
  También tiene 3 comoponentes que se definen así:
  HUE = referencia del color. Sus valores (desde 0 a 255) recorren toda la gama de colores 
    posible. El color final coincide con el inicial. Es cíclico.
  Saturation = saturación. Es la cantidad de color. Sin saturación sólo hay tonos grises.
  Value = valor o brillo. Cuanto más alto, más brillante.

  En el siguiente ejemplo recorreremos la gama de colores que proporciona hue de manera cíclica.
  Pintaremos los leds secuencialmente. Cada uno con una ligera variación de color respecto al anterior.
  Cuando lleguemos al último o al primero, cambiaremos el sentido de llenado.
  Para darle un efecto de desvanecimiento, cada vez que pintemos un nuevo color en un led,
  disminuiremos el brillo en un 2% para todos los leds.

  El brillo podríamos disminuirlo multiplicando por 0.98 a cada uno de los canales de color que
  que tiene cada led, pero gracias a la librería, ya tenemos creada una función que hace esto: nscale8()
  El parámetro es representa el nivel respecto a 256 con el que se quedará el brillo final.
  En nuestro caso es de 250 que repreenta un 250/256=0.02=2%
*/

//Librería de gestión
#include "FastLED.h"

//Definimos el número de leds que tiene la matriz
#define NUM_LEDS 64
//Definimos el pin de datos
#define DATA_PIN 3
//Dirección de encendido de leds
int delta=1;
//Led procesado
int led=0;

//Creamos un array con tantos elementos como leds para que contenga el color de cada 
//uno de ellos
CRGB leds[NUM_LEDS];

//Variable en la que guardaremos el color (formato HUE)
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
  //Fijamos el color del led procesado a máxima saturación y brillo
  //al mismo tiemp que incrementamos el valor del color para la próxima vez
  leds[led]=CHSV(hue++, 255, 255);
  //Mostramos cambios
  FastLED.show();
  //Recorremos todos los leds y reducimos su brillo en un 2% (2%=250/256)
  for(int i=0;i<NUM_LEDS;i++) leds[i].nscale8(250);
  //Calculamos el próximo led a procesar
  led+=delta;
  //Si llegamos al mínimo o máximo...cambiamos de dirección
  if(led==0 || led==NUM_LEDS-1) delta=-delta;
  //Esperamos un momento
  delay(10);
}


