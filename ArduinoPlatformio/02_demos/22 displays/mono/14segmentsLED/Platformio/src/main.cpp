/*
  Autor: Ramón Junquera
  Tema: Librería RoJoHT16K33 para gestión de display de leds de 15 segmentos
  Versión: 20211220

  Descripción:
  Ejemplo de uso de librería RoJoHT16K33 para gestión de display de leds de 4 dígitos de 15 segmentos
*/
#include <Arduino.h>
#include <RoJoHT16K33.h>

RoJoHT16K33 display;

//Mostrar caracteres especiales
void test1() {
  display.autoDraw=false; //Desactivamos el refresco automático
  display.clear();
  display.videoMem[0]=0b0001010100001001; //Copa de Martini
  display.videoMem[1]=0b0110110100001001; //Reloj de arena con punto
  display.draw(); //Mostramos los caracteres especiales
  delay(2000);
  display.blink(1); //El display parpadeará 2 veces por segundo
  delay(3000);
  display.blink(0); //Quitamos el parpadeo
}

//Mostrar números enteros
void test2() {
  display.autoDraw=true; //Activamos el refresco automático
  for(int8_t i=-12;i<=12;i++) { //Secuencia de -9 a 10
    display.clear();
    display.print(String(i));
    delay(500);
  }
}

//Brillo
void test3() {
  display.print("Hola");
  int8_t delta=-1;
  int8_t brightness=14;
  while(brightness<15) {
    display.setBrightness(brightness);
    delay(300);
    brightness+=delta;
    if(brightness==0) delta=-delta;
  }
}

//Mostrar números enteros combinados con caracteres
void test4() {
  display.clear();
  display.videoMem[2]=0b0000000011100011; //º
  display.print(3,'C');
  //Secuencia de -9ºC a 10ºC
  for(int8_t i=-9;i<=10;i++) {
    String s=String(i);
    if(s.length()<2) s=" "+s;
    display.print(s);
    delay(500);
  }
  display.enable(false); //Desactivamos el display
  delay(1000);
  display.enable(true); //Activamos el display
  delay(1000);
}

void setup() {
  display.begin(); //Inicialización del display
}

void loop() {
  test1();
  test2();
  test3();
  test4();
}
