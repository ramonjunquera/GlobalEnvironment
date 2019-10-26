/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Conocimiento de la matriz y ejemplo blink
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  Utilizaremos por primera vez la matriz de leds RGB de 8x8 CJMCU-64.
  Su conectividad es muy simple. Sólo tiene 3 pines.
    DIN (datos)
    +5V (potencia)
    GND (tierra)
  Se pueden conectar varios encadenandolos, si unimos el pin DOUT del primero con el DIN del segundo.

  El pin de datos de puede conectar a cualquier pin de Arduino y se tratará como pin digital.

  El chip que contiene la matriz para controlar los leds es el WS2812.

  Para gestionarlo utilizaremos la librería FastLED.
  Ésta es una de las que tenermos disponibles en el gestor de librerías. Sólo tenemos que buscarla.
  También podemos descargarla de http://fastled.io/ en la que se encuentra toda la documentación
  original.

  La librería está preparada para trabajar con distintos tipos de chips. Algunos necesitan pines
  adicionales (CLK_PIN). En nuestro caso, no hará falta.

  La matriz está compuesta de 84 leds RGB modelo 5050.
  Cada led RGB está compuesto por 3 leds simples.
  Cada led de un color puede tener 256 niveles de intensidad.
  La gama total de colores que puede representar un led RBG es de 256^3=16.777.216 colores.
  Realmente la matriz de 8x8 es una tira de leds continua que se ha dispuesto en 8 filas.
  
  El consumo máximo de la matriz puede alcanzar los 2A.
  Esta intensidad es más de lo que una placa Arduino es capaz de dar. Por lo tanto es muy 
  aconsejable utilizar una fuente de alimentación externa para dar energía a la matriz.
  Arduino sólo se encargará del pin de datos.
  No olvidemos conectar las tierras de la fuenta de alimentación y Arduino para evistar problemas.

  De todas maneras, la matriz es muy brillante. Y el valor es el máximo cuando todos los leds están
  encendidos al mismo tiempo, a la máxima intensidad y en color blanco (todo encendidos).

  Habitualmente no todos entán encendidos y podemos bajar al brillo a menos del 30% y se seguirá
  viendo perfectamente. Esto hace que su consumo se reduzca lo suficiente como para no tener
  que utilizar fuentes de alimentación externas.
 
  El proyecto actual es el más simple.
  Lo que hace es encender cada uno de los leds en color blanco secuancialmente.

  Se incluye el componente de Fritzing en la carpeta del proyecto.
*/

//Librería de gestión
#include <FastLED.h>

//Definimos el número de leds que tiene la matriz
#define NUM_LEDS 64
//Definimos el pin de datos
#define  DATA_PIN 3

//Creamos un array con tantos elementos como leds para que contenga el color de cada 
//uno de ellos
CRGB leds[NUM_LEDS];

void setup()
{
  //Indicamos que utilizamos chips compatibles con NEOPIXEL. Uno de ellos es el WS2812
  //La conectividad es de un sólo pin de datos
  //Notificamos cuálserá el pin de datos
  //Hacemos referencia al array en el que guardamos los colores de cada led
  //Y por último indicamos el número total de leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  //Recorremos todos los leds
  for(byte led=0;led<NUM_LEDS;led++)
  {
    //El color del led procesado será blanco
    leds[led] = CRGB::White;
    //Mostramos le configuracióna actual
    FastLED.show();
    //Esperamos un momento
    delay(100);
    //Apagamos el led indicando que su nuevo color es negro
    leds[led] = CRGB::Black;
    //Mostramos la configuración actual (todos los leds apagados)
    FastLED.show();
  } 
}

