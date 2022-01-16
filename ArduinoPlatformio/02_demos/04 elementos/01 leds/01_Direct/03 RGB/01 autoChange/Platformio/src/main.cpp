/*
  Autor: Ramón Junquera
  Fecha: 20210215
  Tema: Led RGB
  Objetivo: Aprender a controlar un led RGB
  Material: breadboard,led RGB

  Descripción:
  Sólo utilizaremos un elemento. El led RGB.
  Haremos un programa para que encienda secuencialmente cada uno de sus colores (RGB) cada segundo.
  Utilizaremos el led reg Keyes K851261, compuesto por un led triple modelo Keyes 5050 y las
  correspondientes resistencias integradas.
  Las resistencias son componentes integrados (SMD) del modelo 331, con un valor de 330 ohmios.
  Al estar incluidas las resistencias, no tendremos que preocuparnos de ponerlas en la breadboard.
  Este modelo de led RGB es de alimentación compartida, porque su primera pata está marcada como VCC.
  Debe ser conectada a alimentación (5V).
  Si estuviese etiquetada como GND sería de tierra compartida.
  Las resistencias integradas son mayores que las que utilizamos habitualmente con leds normales
  (220 ohmios). Esto ofrece una protección extra tanto para la placa Arduino como al led RGB.
  Al ser un led RGB de alimentación compartida (ánodo común) nos encontramos con un pequeño problema...
  Sabemos que si un pin configurado como digital está en el estado HIGH es igual que el pin de 5V.
  Y en estado LOW como GND.
  Hasta ahora, para encender un led, conectábamos su cátodo a tierra y su ánodo, a través de una
  resistencia, al pin digital. Cuando cambiábamos su estado a HIGH, el led se encendía.
  Ahora no podemos hacer lo mismo, puesto que los tres leds tienen la alimentación compartida.
  Lo que haremos será conectar la pata común (VCC) a alimentación (5V), y cada una de las otras
  tres patas a 3 pines digitales distintos.
  Cuando el pin esté en LOW, el led se encenderá.
  Pero si el pin está en HIGH nos encontraremos que hemos conectado ambas patas del led a 5V
  y no habrá diferencia de potencial, por lo tanto el led no se encenderá.
  Esto es lo contrario a lo que veníamos haciendo. Ahora para encenderlo pondremos el pin en LOW
  y para apagarlo en HIGH.
  Si el led RGB fuese de tierra compartida (cátodo común) sería al revés.
  En el esquema de placa aparece un led rgb normal (sin resistencias incluidas) porque no está
  exactamente el mismo modelo.
  No incluimos el dibujo de las resistencias.
    
  Resultado:
  Se encienden los 3 colores básicos secuencialmente cada segundo
*/

#include <Arduino.h>

///Declaración de variables globales
const byte pinLeds[]={4,5,6}; //BGR

void setup() {
  //El pin 7 lo utilizaremos de ánodo. Siempre debe estar en HIGH
  pinMode(7,OUTPUT); digitalWrite(7,HIGH);
  for(int8_t i=2;i>=0;i--) { //Recorremos todos los leds...
    pinMode(pinLeds[i],OUTPUT); //Configuramos su pin como salida
    digitalWrite(pinLeds[i],HIGH); //Inicialmente todos apagados
  }
}

void loop() {
  //El led activo será cada vez uno...
  for(int8_t activeLed=2;activeLed>=0;activeLed--) { //Recorremos todos los leds...
    digitalWrite(pinLeds[activeLed],LOW); //Led encendido
    delay(1000);
    digitalWrite(pinLeds[activeLed],HIGH); //Led apagado
  }
}
