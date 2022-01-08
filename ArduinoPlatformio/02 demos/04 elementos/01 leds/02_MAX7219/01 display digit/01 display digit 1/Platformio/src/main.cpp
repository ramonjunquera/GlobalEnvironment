/*
  Autor: Ramón Junquera
  Tema: Librería para chip MAX7219 para displays de dígitos
  Fecha: 20211015
  Objetivo: Demostración de capacidades de la librería RoJoMAX7219d
  Material: breadboard, cables, MAX7219, digit led display, placa
    Arduino UNO, resistencia de 10Kohmios, condensador 10µF,
    condensador 100 nF

  Descripción:
 
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

  Pinout: Led display de un dígito:
  
  g f - a b
  │ │ │ │ │
  █████████
  █████████
  █████████   los pines - corresponden a GND  
  █████████   ambos están unidos
  █████████
  │ │ │ │ │
  e d - c h
  

  Pinout: MAX7219:
           
    24  23 22  21  20 19  18  17  16 15  14  13
   DOUT C5 C1  C6  C4 V+ ISET C8  C3 C7  C2  CLK
    │   │   │   │   │   │  │   │   │  │   │  │
  ███████████████████████████████████████████████
  ███████████████████████████████████████████████
    ████████████████ MAX7219 █████████████████████
  ███████████████████████████████████████████████
  ███████████████████████████████████████████████
    │   │  │   │   │  │   │   │   │  │   │   │
   DIN  F1 F5 GND  F7 F3  F4  F8 GND F6  F2 LOAD      
    1   2   3  4   5   6   7  8   9  10  11  12      
        
        
  DIN = Primero de los tres pines conectados a la placa
  LOAD = Segundo de los tres pines conectados a la placa
  CLK = Tercero de los tres pines conectados a la placa
  DOUT = Salida a conectar con la pata DIN del siguiente chip de la cadena
  GND = Tierra
  V+ = Alimentación. Funciona a 3.3V. Sugeridos 5V
  ISET = Siempre estará conectado a V+ a través de una resistencia
  Fx = Filas / dígitos
  Cx = Columnas / segmentos
  
  Descripción y demostración de funcionalidades de la librería.
  La librería RoJoMAX7219d es una variación de RoJoMAX7219, de la que se ha
  eliminado el trabajo con sprites para reducir el consumo de memoria.
  Especialmente diseñada para trabajar con el chip MAX7219 y con displays
  de led de dígitos.
  Permite definir caracteres especiales, aunque ya tiene un alfabeto por defecto.
  
  Resultado:
  Realizamos varios tests cíclicos

  Nota:
    Los condensadores son opcionales, aunque convenientes para filtrar
    interferencias.
*/
#include <Arduino.h>
#include <RoJoMAX7219d.h> //Librería de gestión de MAX7219 para displays de dígitos de leds

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219d display;

//Definición de pines para Arduino UNO
const byte pinDIN=2;
const byte pinCS=3;
const byte pinCLK=4;

//Mostrar todos los caracteres de alfabeto
void test1() {
  for(byte i=0;i<19;i++) {
    //Mostramos el carácter en la posición 0 y sin punto decimal
    display.print(0,i,false);
    display.draw();
    delay(500);
  }
}

//Mostrar caracteres definidos por el usuario
void test2() {
  display.videoMem[0]=0b00110111; //H en formato .abcdefg
  display.draw();
  delay(500);
  display.videoMem[0]=0b00001110; //L en formato .abcdefg
  display.draw();
  delay(500);
  display.videoMem[0]=0b01100111; //P en formato .abcdefg
  display.draw();
  delay(500);
}

void setup() {
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN,byte pinCS,byte pinCLK)
  display.begin(1,pinDIN,pinCS,pinCLK);
}

void loop() {
  test1();
  test2();
}
