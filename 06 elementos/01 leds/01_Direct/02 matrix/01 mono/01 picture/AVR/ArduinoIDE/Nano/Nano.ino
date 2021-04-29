/*
  Autor: Ramón Junquera
  Tema: Matriz de leds de 8x8. Multiplexación manual
  Objetivo: Mostrar imagen estática
  Fecha: 20171107
  Material: 2 breadboards, cables, matriz de leds 8x8, 8 resistencias de 220 ohmios, Arduino Nano

  Descripción:
  El objetivo del ejercicio es mostrar una imagen estática en una matriz de leds, para lo cual
  necesitamos multiplexarlos. Le haremos manualmente para entender cómo funciona.
  
  Una matriz de leds de 8x8 tiene 16 pines. 8 para filas y 8 para columnas.
  Un Arduino UNO o Nano tiene 14 pines digitales numerados del 0 al 13.
  No son suficientes.
  Los pines analógicos (5 en UNO y 6 en Nano), pueden ser tratados como pines digitales.
  La numeración continúa con los pines digitales:
    A0 = 14
    A1 = 15
    A2 = 16
    A3 = 17
    A4 = 18
    A5 = 19
    A6 = 20
  Con esta extensión sí tenemos suficientes pines digitales para controlar la matriz completa.
  No utilizaremos los pines 0 y 1 para dejar libre el puerto serie.
  Por lo tanto utilizaremos los pines desde el 2 al 17.
  
  Recordemos el pinout de la matriz:
  
    C8  C7 F2  C1  F4  C6 C4  F1
    │  │  │  │  │  │  │  │
  ███████████████████
  ███████████████████
  ███████████████████
  ███████████████████
  ███████████████████
    │  │  │  │  │  │  │  │
    F5  F7 C2  C3  F8 C5  F6  F3  

  Iremos enciendiendo cada una de las filas con la configuración de columnas que le corresponda.
  Como mucho tendremos una fila encendida en cada momento.
  Por esa razón pondremos las resistencias en los pines de columnas.

  Puesto que las columnas las contamos de izquierda a derecha, pero los bits de un byte
  
  Resultado:
  Vemos una imagen estática en la matriz de leds
*/

//Definición de pines de filas y columnas
const byte pinColumn[8] = {5,15,14,8,12,7,3,2};
const byte pinRow[8] = {9,4,10,6,17,11,16,13};
//Creamos la memoria de vídeo
//La información se guarda en filas
const byte videoMem[8] = 
{
  0b01111110, //.XXXXXX.
  0b10000001, //X......X
  0b10011101, //X..XXX.X
  0b10100101, //X.X..X.X
  0b10100101, //X.X..X.X
  0b10011110, //X..XXXX.
  0b10000000, //X.......
  0b01111000  //.XXXXX..
};

void setup()
{
  //Configuramos los pines de columnas y filas como salida
  for(byte i=0;i<8;i++)
  {
    pinMode(pinRow[i], OUTPUT);
    pinMode(pinColumn[i], OUTPUT);
  }
}

void loop()
{
  //Refrescamos continuamente la memoria de vídeo
  show();
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

