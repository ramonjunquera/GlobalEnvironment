/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219
  Fecha: 20171115
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219 con
    display led de dígitos
  Material: breadboard, cables, MAX7219, 4 digits led display, placa RPi,
    resistencia de 10Kohmios, condensador 10µF, condensador 100 nF

  Descripción:
  Recordemos pinouts
  
  De 4-digits led display:

       12 11 10  9  8  7
       C0  a  f C1 C2  b
        ¦  ¦  ¦  ¦  ¦  ¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
        ¦  ¦  ¦  ¦  ¦  ¦
        e  d  h  c  g C3
        1  2  3  4  5  6

  Distribución de segmentos:
  
        a
      ¦¦¦¦¦¦
    ¦¦      ¦¦
  f ¦¦      ¦¦ b
    ¦¦  g   ¦¦
      ¦¦¦¦¦¦
    ¦¦      ¦¦
  e ¦¦      ¦¦ c
    ¦¦      ¦¦
      ¦¦¦¦¦¦     ¦¦ h
        d

  De MAX7219:
           
 DOUT C5 C1 C6 C4 V+ ISETC8 C3 C7 C2 CLK
    │  │  │  │  │  │  │  │  │  │  │  │
  ██████████████████████████████████████
  ██████████████████████████████████████
   ███████ MAX7219 ██████████████████████
  ██████████████████████████████████████
  ██████████████████████████████████████
    │  │  │  │  │  │  │  │  │  │  │  │
  DIN F1 F5 GND F7 F3 F4 F8GND F6 F2 LOAD      
        
        
  DIN = Primero de los tres pines conectados a la placa
  LOAD = Segundo de los tres pines conectados a la placa
  CLK = Tercero de los tres pines conectados a la placa
  DOUT = Salida a conectar con la pata DIN del siguiente chip de la cadena
  GND = Tierra
  V+ = Alimentación. Funciona a 3.3V. Sugeridos 5V
  ISET = Siempre estará conectado a V+ a través de una resistencia
  Fx = Filas
  Cx = Columnas
  
  Descripción y demostración de funcionalidades de la librería.
  
  Resultado:
  Realizamos varios tests cíclicos

  Nota:
    Los condensadores son opcionales, aunque convenientes para filtrar
    interferencias.
*/

#include <Arduino.h>
#include "RoJoMAX7219.cpp" //Librería de gestión de MAX7219

//La librería de gestión de sprites no se utiliza en este ejemplo, pero
//sí es necesaria para la librería RoJoMAX7219
#include "RoJoSprite.cpp"

using namespace std;

//Definimos los pines del display
const byte pinDIN=21;
const byte pinCS=20;
const byte pinCLK=16;

//Declaración de variables globales
//Creamos el objeto display que gestionará la cadena de chips MAX7219
//En la creación ya se incluye la activación y la inicialización
//tras ello estará lista para ser utilizada
//RoJoMatrix(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
RoJoMAX7219 display(1,pinDIN,pinCS,pinCLK);

void test1()
{
  //Mostrar caracteres especiales: Ab.Cd
  display.print(0,10,false); //A
  display.print(1,11,true); //b.
  display.print(2,12,false); //C
  display.print(3,13,false); //d
  display.show();
  delay(1000);
}

void test2()
{
  //Mostrar números enteros
  //Secuencia de -9 a 10
  display.clear();
  for(int8_t i=-9;i<=10;i++)
  {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 2 dígitos y con ceros a la izquierda
    display.printInt(i,0,2,true);
    display.show();
    delay(500);
  }
}

void test3()
{
  //Mostrar números enteros sin ceros a la izquierda
  //Secuencia de -2 a 10
  for(int8_t i=-2;i<=10;i++)
  {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 2 dígitos y sin ceros a la izquierda
    display.printInt(i,0,2,false);
    display.show();
    delay(500);
  }
}

void test4()
{
  //Mostrar números decimales sin ceros a la izquierda
  //Secuencia de -2.9 a 2.3
  
  for(double i=-2.9;i<=2.3;i+=0.1)
  {
    //Escribimos el número decimal i desde la posición 0
    //con 2 dígitos de parte entera y un dígito para decimales
    //sin ceros a la izquierda
    display.printDec(i,0,2,1,false);
    display.show();
    delay(500);
  }
  return;  
}

void test5()
{
  //Mostrar números enteros combinados con caracteres
  //Secuencia de -9ºC a 10ºC
  display.print(2,18,false); //º
  display.print(3,12,false); //C
  for(int8_t i=-9;i<=10;i++)
  {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 2 dígitos y sin ceros a la izquierda
    display.printInt(i,0,2,false);
    display.show();
    delay(500);
  }
}

int main(int argc, char **argv)
{
  while(1)
  {
	  test1();
	  test2();
	  test3();
	  test4();
	  test5();
  }
}

