/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219 para displays de dígitos
  Fecha: 20211015
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219d
  Material: breadboard, cables, MAX7219, 2x 4 digits led display de cátodo común,
    resistencia de 10Kohmios, condensador 10µF, condensador 100 nF

  Descripción:
    El programa hace uso de las funcionalidades de la librería para varias demos.
    De la misma manera que podemos controlar una matriz de leds con el MAX7219, lo
    aplicamos a dos displays de 4 dígitos.
    Lo que en la matriz es una fila, aquí lo consideraremos un dígito.
    Cada una de las columnas de la matriz es un segmento de un dígito.
    Los pines de los segmentos de ambos displays son compartidos y conectados a columnas
    en el MAX7219.
    Los dígitos están montados de izquierda a derecha. El dígito más a la izquierda está
    conectado a la última fila.
    Los segmentos no tienen el orden natural de abcdefg. sino .abcdefg
    Esta ordenación de dígitos y segmentos se considera el estándar cuando trabajamos
    displays led de dígitos.
    El circuto puede ser montado tal y como se indica, pero también se vende montado:
    https://es.aliexpress.com/item/1005001555524397.html?gatewayAdapt=glo2esp&spm=a2g0s.9042311.0.0.274263c09sFJh8

  Notas:
  - Los condensadores son opcionales, aunque convenientes para filtrar interferencias.
*/
#include <Arduino.h>
#include <RoJoMAX7219d.h> //Librería de gestión de MAX7219 para displays de dígitos de leds

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219d display;

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDIN=D0;
  const byte pinCS=D1;
  const byte pinCLK=D2;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDIN=22;
  const byte pinCS=21;
  const byte pinCLK=4;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDIN=2;
  const byte pinCS=3;
  const byte pinCLK=4;
#elif defined(__arm__) //Si es una RPi
  const byte pinDIN=21;
  const byte pinCS=20;
  const byte pinCLK=16;
#endif

//Mostrar caracteres especiales: Ab.Cd
void test1() {
  //Desactivamos el refresco automático
  display.autoDraw=false;
  display.videoMem[0]=0b00110111; //H en formato .abcdefg
  display.print(1,0,false); //0
  display.videoMem[2]=0b00001110; //L en formato .abcdefg
  display.print(3,10,true); //A.
  display.print(4,15,false); //F
  display.print(5,14,false); //E
  display.print(6,0,false); //0
  display.draw();
  delay(1000);
}

//Mostrar números enteros
void test2() {
  display.autoDraw=true; //Activamos el refresco automático
  display.clear(); //Secuencia de -9 a 10
  for(int8_t i=-9;i<=10;i++) {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 3 dígitos y con ceros a la izquierda
    display.printInt(i,0,3,true);
    display.draw();
    delay(500);
  }
}

//Mostrar números enteros sin ceros a la izquierda
void test3() {
  //Secuencia de -2 a 10
  for(int8_t i=-2;i<=10;i++) {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 3 dígitos y sin ceros a la izquierda
    display.printInt(i,0,3,false);
    display.draw();
    delay(500);
  }
}

//Mostrar números decimales sin ceros a la izquierda
void test4() {
  //Secuencia de -9.9 a 10.0
  for(double i=-9.9;i<=10;i+=0.1) {
    //Escribimos el número decimal i desde la posición 0
    //con 2 dígitos de parte entera y un dígito para decimales
    //sin ceros a la izquierda
    display.printDec(i,0,2,1,false);
    display.draw();
    delay(100);
  }
}

//Mostrar números enteros combinados con caracteres
void test5() {
  //Secuencia de -9ºC a 10ºC
  display.print(2,18,false); //º
  display.print(3,12,false); //C
  for(int8_t i=-9;i<=10;i++) {
    //Escribimos el número entero i desde la posición 0
    //con una longitud de 2 dígitos y sin ceros a la izquierda
    display.printInt(i,0,2,false);
    display.draw();
    delay(500);
  }
}

void setup() {
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN,pinCS,pinCLK);
}

void loop() {
  test1();
  test2();
  test3();
  test4();
  test5();
}
