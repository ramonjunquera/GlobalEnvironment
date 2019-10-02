/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Fecha: 20191001
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: placa ESP8266, 11 leds
  Descripción:
    Utilizaremos los 11 pines de la placa para controlarlos con PWM.
    Puesto que también usamos los pines del puerto serie, éste quedará inutilizado.
    En algunas placas es necesario desconectar los pines RX/TX cuando se transfiera
    el programa para que no de error de comunicación.
    Se definen los pines de comunicación porque no todas las placas tienen definidas
    las constantes RX y TX.

  Resultado:
    Podemos controlar el brillo de cada uno de los leds de manera independiente
*/

#include <Arduino.h>

const byte pinTX=1;
const byte pinRX=3;

//Definición de variables globales
const byte pin[11]={pinTX,pinRX,D1,D2,D3,D4,D8,D7,D6,D5,D0};
int level[11]; //Brillo de cada led
byte currentLed=0;
int delta=1;
const unsigned long delayValue=100; 
const byte levelReduced=200; //Cantidad de brillo que se reduce por ciclo

void setup() {
  //Configuramos todos los pines de salida
  for(byte p=0;p<11;p++) pinMode(pin[p],OUTPUT);
}

void loop() {
  //Reducimos el brillo de todos los leds
  for(byte p=0;p<11;p++) {
    //Reducimos el brillo en la cantidad definida
    level[p]-=levelReduced;
    //Si es menor que el mínimo...le asignamos el mínimo
    if(level[p]<0) level[p]=0;
  }
  //El led actual queda al máximo brillo
  level[currentLed]=1023;
  //Calculamos cuál será el siguiente led
  currentLed+=delta;
  //Si hemos llegado a un extremo...cabiamos de dirección
  if(currentLed==0 || currentLed==10) delta=-delta;
  //Aplicamos los nuevos estados
  for(byte p=0;p<11;p++) analogWrite(pin[p],level[p]);
  //Esperamos
  delay(delayValue);
}
