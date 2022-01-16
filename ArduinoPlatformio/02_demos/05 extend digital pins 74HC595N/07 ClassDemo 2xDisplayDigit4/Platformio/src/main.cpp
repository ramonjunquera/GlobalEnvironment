/*
  Autor: Ramón Junquera
  Fecha: 20210927
  Tema: Chip de extensión de pines digitales 74HC595N
  Objetivo: Demostración de librería con dos displays de segmentos de led de 4 dígitos

  Descripción:
  Demostración  de uso de la librería RoJo74HC595N8d para la gestión de 2 displays led
  númericos de 4 dígitos, controlados por dos chips 74HC595.

  Pinout de display led numérico de 4 dígitos:

       12 11 10  9  8  7
       C0  a  f C1 C2  b
        ¦  ¦  ¦  ¦  ¦  ¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
        ¦  ¦  ¦  ¦  ¦  ¦
        e  d  h  c  g C3
        1  2  3  4  5  6

  Distribución de segmentos de un dígito:
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

  Pinout de chip 74HC595N:

     16   15   14   13   12   11   10   9
    VCC   Q0   DS   OE  STCP SHCP  MR  Q7S
     ¦    ¦    ¦    ¦    ¦    ¦    ¦    ¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
   ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦ 74HC595N ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
  ¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦¦
     ¦    ¦    ¦    ¦    ¦    ¦    ¦    ¦
     Q1   Q2   Q3   Q4   Q5   Q6   Q7   GND
     1    2    3    4    5    6    7    8

  Inicialmente se muestra untexto con caracteres de la librería y definidos
  por el usuario.
  Después se muestran secuencialmente los entre -9 y 99.
*/

#include <Arduino.h>
#include <RoJo74HC595N8d.h>

//Definición de pines
const byte pinData = 2; //DS = DIO
const byte pinLatch = 3; //STCP = RCK
const byte pinClock = 4; //SHCP = SCK

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines de conexión del 74HC595N
RoJo74HC595N8d display = RoJo74HC595N8d(pinData,pinLatch,pinClock);

void setup() {
  display.set(0,0b01110110); //H
  display.set(1,0,false); //0
  display.set(2,0b00111000); //L
  display.set(3,10,true); //A.
  display.set(4,15,false); //F
  display.set(5,14,false); //E
  display.set(6,0,false); //0
  display.set(7,5,false); //S
  display.show(2000);
}

void loop() {
  //Recorremos todos los números entre el -9 y el 99
  for(int64_t i=-9;i<100;i++) {
    display.write(i);
    display.show(200);
  }
}
