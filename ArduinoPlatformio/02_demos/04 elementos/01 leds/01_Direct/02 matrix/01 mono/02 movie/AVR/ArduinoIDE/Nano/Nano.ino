/*
  Autor: Ramón Junquera
  Tema: Matriz de leds de 8x8. Multiplexación manual
  Objetivo: Mostrar imagen en movimiento   
  Fecha: 20171107
  Material: 2 breadboards, cables, matriz de leds 8x8, 8 resistencias de 220 ohmios, Arduino Nano

  Descripción:
  El objetivo del ejercicio es mostrar una imagen en movimiento en una matriz de leds, para lo cual
  necesitamos multiplexarlos. Le haremos manualmente para entender cómo funciona.

  El ejercicio está basado en el anterior, que muestra una imagen estática.

  Se crea un array que almacene las imágenes de cada fotograma.
  Se crea un array que almacena la secuencia de fotogramas a mostrar.
  La memoria de vídeo refresca continuamente, pero se sobreescribe a cada cambio de fotograma.
  
  Resultado:
  Vemos una imagen en movimiento en la matriz de leds
*/

//Definición de pines de filas y columnas
const byte pinColumn[8] = {5,15,14,8,12,7,3,2};
const byte pinRow[8] = {9,4,10,6,17,11,16,13};
//Creamos los fotogramas
//Tendrá 3 fotogramas, cada uno de ellos de 8 bytes
const byte frames[3][8]=
{
  { //Fotograma 0
    0b01111000, //.XXXX...
    0b01000100, //.X...X..
    0b01111110, //.XXXXXX.
    0b00110000, //..XX....
    0b01100000, //.XX.....
    0b10010000, //X..X....
    0b10010000, //X..X....
    0b01100000  //.XX.....
  },
  { //Fotograma 1
    0b01111000, //.XXXX...
    0b01000100, //.X...X..
    0b01111110, //.XXXXXX.
    0b00011000, //...XX...
    0b00011000, //...XX...
    0b00100100, //..X..X..
    0b00100100, //..X..X..
    0b00011000  //...XX...
  },
  { //Fotograma 2
    0b01111000, //.XXXX...
    0b01000100, //.X...X..
    0b01111110, //.XXXXXX.
    0b00001100, //....XX..
    0b00000110, //.....XX.
    0b00001001, //....X..X
    0b00001001, //....X..X
    0b00000110  //.....XX.
  }
};
//Creamos la secuencia de fotogramas
///La secuencia es de 4 fotogramas
const byte sequence[4]={0,1,2,1};
//Creamos la memoria de vídeo
//La información se guarda en filas
byte videoMem[8];

void setup()
{
  //Configuramos los pines de columnas y filas como salida
  for(byte i=0;i<8;i++)
  {
    pinMode(pinRow[i], OUTPUT);
    pinMode(pinColumn[i], OUTPUT);
  }
}

void show()
{
  //Refresca la memoria de vídeo
  
  //Recorremos todas las filas
  for(byte r=0;r<8;r++)
  {
    //Recorremos todas las columnas
    for(byte c=0;c<8;c++)
    {
      //Activamos la columna si tenemos su correspondiente bit activo
      digitalWrite(pinColumn[c],videoMem[r] & (1<<(7-c)));
    }
    //Activamos la fila
    digitalWrite(pinRow[r],LOW);
    //La mantenemos un momento encendida
    delay(2);
    //Desactivamos la fila
    digitalWrite(pinRow[r],HIGH);
  }
}

void loop()
{
  //Recorreremos todos los fotogramas de la secuencia
  for(byte f=0;f<4;f++)
  {
    //Copiamos el fotograma que corresponde a la memoria de vídeo
    for(byte r=0;r<8;r++) videoMem[r]=frames[sequence[f]][r];
    //Refrescamos el display con la memoria de vídeo unas cuantas veces
    for(byte t=0;t<20;t++) show();
  }
}



