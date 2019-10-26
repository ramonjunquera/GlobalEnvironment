/*
  Autor: Ramón Junquera
  
  Tema: Uso de led RGB matrix CJMCU-64 (WS2812 5050)
  
  Objetivo: Uso de funcions random
  
  Material adicional: breadboard, Arduino Nano, cables, RGB led matrix CJMCU-64

  Descripción:
  Crearemos una simulación de fuego con las siguientes reglas:
  - Según pasa el tiempo todo pierde calor
  - El calor se transmite a las posiciones contiguas
  - A veces saltan nuevas chispas que avivan el fuego
  - Hay una función que convierte el calor en color
*/

//Librería de gestión
#include "FastLED.h"

//Definimos el pin de datos
#define DATA_PIN 3

//Creamos el array que mantiene los colores de cada led
//Suponemos que es una matriz de 8x8
CRGB leds[64];
//Creamos el array del calor de cada led (x,y)
//El 0,0 está arriba a la izquierda
byte heat[8][8];

//Una de las líneas para hace la transmisión de calor
byte row[8];

//Valor máximo de pérdida de calor con el tiempo [0,255]
#define COOLING 14
//Por debajo del valor de SPARKLING saltará una chispa [0,255]
#define SPARKING 10

void setup()
{
  //Indicamos que utilizamos chips compatibles con NEOPIXEL. Uno de ellos es el WS2812
  //La conectividad es de un sólo pin de datos
  //Notificamos cuálserá el pin de datos
  //Hacemos referencia al array en el que guardamos los colores de cada led
  //Y por último indicamos el número total de leds
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, 64);
  //Fijamos el brillo a 30
  LEDS.setBrightness(30);
}

void loop()
{
  //Enfriamos un poco todo
  //Recorremos todas las filas
  for(byte y=0;y<8;y++)
  {
    //Recorremos todas las columnas
    for(byte x=0;x<8;x++)
    {
      //Reducimos el calor de un led en un valor aleatorio dependiente de la constante COOLING
      //Cuanto mayor sea, más rápido se enfriará y las llamas serán más cortas
      //Utilizamos la función qsub8 incluida en la librería porque nos ofrece la ventaja de que
      //nunca obtendrá valores por debajo de cero, por mucho que quitemos
      heat[x][y]=qsub8(heat[x][y],random8(0,COOLING));
    }
  }

  //El calor se propaga en horizontal y hacia arriba. No hacia abajo.
  //Por eso, el calor de una posición estará influido por el calor de la propia posición
  //el de las posiciones contiguas en horizontal y la inmediatamente inferior.
  //Puesto que es mas importante el calor propio que el de las celdas contiguas, tendremos
  //que darle más peso al calcular la media.
  //Puesto que el calor se transmite más rápido hacia arriba que en horizontal, también 
  //tendremos que darle más peso.
  //La distribución de pesos queda así:
  // 8 x calor propio
  // 4 x calor de posición inferior
  // 1 x calor de cada posición contigua en horizontal
  //Recorremos las 7 primeras las filas (las superiores)
  for(byte y=0;y<7;y++)
  {
    //Una posición intercambia calor con las contiguas en horizontal y la de abajo
    //Posición izquierda
    row[0]=(8*heat[0][y]+heat[1][y]+4*heat[0][y+1])/13; //No hay posición contigua a la izquierda
    //Recorremos las columnas centrales
    for(byte x=1;x<7;x++)
    {
      //Calculamos el calor medio
      row[x]=(heat[x-1][y]+8*heat[x][y]+heat[x+1][y]+4*heat[x][y+1])/14;
    }
    //Posición derecha
    row[7]=(8*heat[7][y]+heat[6][y]+4*heat[7][y+1])/13; //No hay contigua a la derecha
    //Copiamos le fila de calores promedio
    for(byte x=0;x<8;x++) heat[x][y]=row[x];
  }
  //También se propaga en la fila de abajo, con la diferencia que no hay fila inferior
  //Posición izquierda
  row[0]=(8*heat[0][7]+heat[1][7])/9; //No hay posición contigua a la izquierda
  //Recorremos las columnas centrales
  for(byte x=1;x<7;x++)
  {
    //Calculamos el calor medio
    row[x]=(heat[x-1][7]+8*heat[x][7]+heat[x+1][7])/10;
  }
  //Posición derecha
  row[7]=(8*heat[7][7]+heat[6][7])/9; //No hay contigua a la derecha
  //Copiamos le fila de calores promedio
  for(byte x=0;x<8;x++) heat[x][7]=row[x];

  //A veces saltan chispas que avivan el fuego
  //Recorremos todas las columnas
  for(byte x=0;x<8;x++)
  {
    //Si debe saltar una chispa...
    if(random8()<SPARKING)
    {
      //...aumentamos el calor de led de la fila inferior en una cantidad aleatoria entre [160,255]
      //Utilizamos la función qadd8 porque nos asegura que el valor nunca superará 255
      heat[x][7]=qadd8(heat[x][7],random(160,255));
    }
  }

  //Convertimos el calor en color
  //Recorremos todas las filas
  for(byte y=0;y<8;y++)
  {
    //Recorremos todas las columnas
    for(byte x=0;x<8;x++)
    {
      //Convertimos el calor en color con la función integrada HeatColor
      leds[x+8*y]=HeatColor(heat[x][y]);
    }
  }
  
  //Mostramos cambios
  FastLED.show();
  //Esperamos un momento
  delay(30);
}


