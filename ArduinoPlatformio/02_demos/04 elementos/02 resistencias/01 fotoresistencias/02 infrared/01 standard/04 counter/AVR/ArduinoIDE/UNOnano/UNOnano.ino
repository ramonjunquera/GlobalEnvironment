/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja
  Objetivo: Funcionamiento de una fotorresistencia infrarroja
  Material: 2 breadboards, 2 fotorresistencias IR, 2 leds IR, 2 resistencias de 10Kohmios,
     2 leds normales, cables, Arduino UNO y Arduino Nano

  Descripción:
  Manteniendo el mismo circuito anterior, haremos un "contador" de entradas y salidas.
  Suponiendo que ambos sistemas de detección de objetos opacos están instalados en una puerta por la
  que pasan personas, haremos un sistema para contar cuántas personas hay dentro (a un lado de la puerta).
  Debemos tener en cuenta que las personas pueden atravesar la puerta en cualquiera de las dos direcciones.

  Para no estar continuamente atentos al estado de los pines nos aprovechamos de:
  - Que sólo tenemos dos receptores
  - Que Arduino UNO/Nano es capaz de gestionar 2 interrupciones por cambio de estado de un pin
  - Que los pines a los que estás conectados los receptores son los que gestionan las interrupciones
  Por lo tanto crearemos una interrupción para cada uno de los pines de señal de los receptores.
  Cuando se detecte que algo ha atravesado la puerta (los dos sensores) modificaremos el contador
  y lo enviaremos por el puerto serie.
  
  Resultado:
  Podemos comprobar en el monitor serie como se muestra el recuento de veces que se ha pasado en un sentido u otro
*/

#include <Arduino.h>

volatile unsigned long lastMicros2=0; //Último tiempo en microsegundos en el que se detectó algo en pin 2
volatile unsigned long lastMicros3=0; //Último tiempo en microsegundos en el que se detectó algo en pin 3
volatile unsigned long timeOut=1000000; //Máximo tiempo permitido para contabilizar un paso (un segundo)
volatile boolean onInt=false; //Semáforo para saber si ya estamos procesando una interrupción
volatile int counter=0; //Contador. Paso de 2 a 3 = positivo. De 3 a 2 = negativo.

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Los pines 4 y 5 son de salida para encender los leds de control
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  //Definimos la interrupción 0 correspondiente al pin 2 para que llame a Pin2Low
  attachInterrupt(0,Pin2Low,FALLING);
  //Definimos la interrupción 1 correspondiente al pin 3 para que llame a Pin3Low
  attachInterrupt(1,Pin3Low,FALLING);  
  //Ambas interrupciones "saltarán" cuando se "apage" el led de control (FALLING)
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
    //Enviamos el contador por el puerto serie
    Serial.println(counter);
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
    //Enviamos el contador por el puerto serie
    Serial.println(counter);
  }

  //Ya hemos terminado de procesar la interrrupción
  onInt=false;
}
