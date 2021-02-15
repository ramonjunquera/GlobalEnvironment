/*
  Autor: Ramón Junquera
  Fecha: 20180604
  Tema: ESP32 Touch pins
  Objetivo: Detección de toques
  Material adicional: placa ESP32
  Descripción:
    Simularemos una de esas lámparas de sobremesa que con simples toques en su base
    varían su intensidad de luz.
    La luz la obtendremos de un led externo.

    Hay que recordar que las placas ESP32 a fecha actual no tienen gestión de PWM.
    Por eso utilizaremos la librería RoJoAnalogWriteESP32.
    Esta librería nos da una resolución PWM de 8 bits.

    Cada vez que detectemos un toque, aumentaremos el valor PWM en 64.
    Por lo tanto tendremos 256/64=4 niveles de intensidad de luz en el led.

    Toda la gestión la haremos con interrupciones.

    Lo único que hay que tener en cuenta es que la interrupción saltará continuamente
    cuando detecte un toque. Debemos diferenciar distintos toques.
    Para ello, anotamos siempre el momento en el que se producen los toques/interrupciones.
  Resultado:
    A cada toque detectado, cambia la intensidad del led.
*/

#include <Arduino.h>
#include "RoJoAnalogWriteESP32.h" //Gestión de PWM para ESP32

//Declaración de constantes globales
const byte pinTouch=T3;
const byte pinLed=19;
const uint32_t touchDelay=100; //Mínimo tiempo entre toques. Una décima de segundo

//Declaración de variables globales
//Tiempo en milisegúndos de aceptación del siguiente toque
uint32_t nextTouchTime=0;
//Nivel de PWM del led
byte levelPWM=0;

void touched()
{
  //Función a la que llama la interrupción touch
  //Se ha detectado un toque

  //Anotamos el tiempo actual
  uint32_t now=millis();
  //Si se considera un nuevo toque...
  if(now>nextTouchTime)
  {
    //...incrementamos el valor PWM del pin
    levelPWM+=64;
    //Y lo aplicamos
    analogWrite(pinLed,levelPWM);
  }
  //Anotamos la hora para el siguiente toque válido
  nextTouchTime=now+touchDelay;
}

void setup()
{
  //Aplicamos el valor PWM inicial al led
  analogWrite(pinLed,levelPWM);
  //Definimos la interrupción:
  //- pint Touch3 = GPIO15
  //- Función touched()
  //- Cuando se detecte un valor por debajo de 10
  touchAttachInterrupt(pinTouch,touched,10);
}

void loop()
{
  //Nada especial que hacer aquí
}
