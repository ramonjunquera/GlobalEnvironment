/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja
  Objetivo: Funcionamiento de una fotorresistencia infrarroja
  Material: 2 breadboards, 2 fotorresistencias IR, 2 leds IR, 3 resistencias de 10Kohmios,
     2 leds normales, cables, Arduino UNO y Arduino Nano, display led 4 dígitos, MAX7219

  Descripción:
  Al circuito anterior le añadiremos un chip MAX7219 y un display led de 4 dígitos.
  En él se mostrará el valor del contador. Así no tenemos que depender de la conectividad.
  Puesto que se cuanta tanto en positivo como en negativo, el valor podría llegar a ser negativo.
  Por eso utilizaremos el primer dígito del display para mostrar el signo, y los otros tres el valor.

  Ya no necesitamos inicializar las comunicaciones por el puerto serie.
  Y la función Serial.println se ha sustituido por una función propia.
  
  Resultado:
  Podemos ver el valor del contador en el display
*/

#include <Arduino.h>
#include "LedControlMS.h"

volatile unsigned long lastMicros2=0; //Último tiempo en microsegundos en el que se detectó algo en pin 2
volatile unsigned long lastMicros3=0; //Último tiempo en microsegundos en el que se detectó algo en pin 3
volatile unsigned long timeOut=1000000; //Máximo tiempo permitido para contabilizar un paso (un segundo)
volatile boolean onInt=false; //Semáforo para saber si ya estamos procesando una interrupción
volatile int counter=0; //Contador. Paso de 2 a 3 = positivo. De 3 a 2 = negativo.

//Creamos el objeto que gestionará la matriz de leds
//Los parámetros son:
//  pin del DIN
//  pin del CLK
//  pin del LOAD(/CS)
//  número de chips enlazados
LedControl lc=LedControl(9,8,7,1);

void setup()
{
  //Los pines 4 y 5 son de salida para encender los leds de control
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  //Definimos la interrupción 0 correspondiente al pin 2 para que llame a Pin2Low
  attachInterrupt(0,Pin2Low,FALLING);
  //Definimos la interrupción 1 correspondiente al pin 3 para que llame a Pin3Low
  attachInterrupt(1,Pin3Low,FALLING);  
  //Ambas interrupciones "saltarán" cuando se "apage" el led de control (FALLING)

  //Activamos el MAX7219
  lc.shutdown(0,false);
  //Indicamos que tenemos 4 filas (dígitos) conectadas
  lc.setScanLimit(0,4);
  //Fijaremos el brillo a una intensidad media (8) para el primer chip
  lc.setIntensity(0,8);
  //Limpiaremos la pantalla. Apagamos todos los leds
  lc.clearDisplay(0);
  //Comenzamos mostrando el 000
  lc.setDigit(0,1,0,false);
  lc.setDigit(0,2,0,false);
  lc.setDigit(0,3,0,false);
}

void loop()
{
  //El estado del pin 5 será el mismo que el del pin digital 3
  digitalWrite(5,digitalRead(3));
  //El estado del pin 4 será el mismo que el del pin digital 2
  digitalWrite(4,digitalRead(2));
}

void Pin2Low()
{
  //Función a la que se llama cuando se el receptor del pin 2 deja de recibir luz

  //Si ya estamos procesando una interrupción...hemos terminado
  if(onInt) return;
  
  //Ahora sí estamos procesando una interrupción
  onInt=true;

  //Anotaremos que la última activación del pin 2 es ahora
  lastMicros2=micros();
  //Si no ha pasado más tiempo del permitido desde la última señal del pin 3...
  if(lastMicros3+timeOut>micros())
  {
    //...hemos detectado un paso de 2 a 3, considerado positivo
    counter++;
    //Borraremos los datos de la última vez que fueron activados los pines
    lastMicros2=0;
    lastMicros3=0;
    //Mostramos el nuevo valor del contador
    ShowCounter();
  }

  //Ya hemos terminado de procesar la interrrupción
  onInt=false;
}

void Pin3Low()
{
  //Función a la que se llama cuando se el receptor del pin 3 deja de recibir luz

  //Si ya estamos procesando una interrupción...hemos terminado
  if(onInt) return;
  
  //Ahora sí estamos procesando una interrupción
  onInt=true;

  //Anotaremos que la última activación del pin 3 es ahora
  lastMicros3=micros();
  //Si no ha pasado más tiempo del permitido desde la última señal del pin 2...
  if(lastMicros2+timeOut>micros())
  {
    //...hemos detectado un paso de 3 a 2, considerado negativo
    counter--;
    //Borraremos los datos de la última vez que fueron activados los pines
    lastMicros2=0;
    lastMicros3=0;
    //Mostramos el nuevo valor del contador
    ShowCounter();
  }

  //Ya hemos terminado de procesar la interrrupción
  onInt=false;
}

void ShowCounter()
{
  //Muestra en el display el valor del contador

  //Si el contador es negativo...
  if(counter<0)
  {
    lc.setChar(0,0,'-',false);
  }
  else
  {
    lc.setChar(0,0,' ',false);
  }
  //Tomamos el valor absoluto del contador
  int value=abs(counter);
  //Obtenemos el dígito de las unidades
  byte digit=value%10;
  //Lo mostramos
  lc.setDigit(0,3,digit,false);
  //Obtenemos el dígito de las decenas
  digit=(value/10)%10;
  //Lo mostramos
  lc.setDigit(0,2,digit,false);
  //Obtenemos el dígito de las centenas
  digit=(value/100)%10;
  //Lo mostramos
  lc.setDigit(0,1,digit,false);
}

