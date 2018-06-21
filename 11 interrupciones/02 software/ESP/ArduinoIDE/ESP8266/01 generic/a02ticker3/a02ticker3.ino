/*
  Autor: Ramón Junquera
  Tema: Timer - Ticker
  Fecha: 20180621
  Objetivo: Desactivar Ticker
  Material: placa ESP8266, interruptor

  Descripción:
  En el ejercicio anterior explicábamos cómo asignar una función a un objeto Ticker.
  Una vez configurado, se repetía para siempre.

  En el ejemplo actual aprenderemos a desactivar un Ticker.

  El circuito consta de un pulsador que será controlado por una interrupción.
  No se tendrá en cuenta el efecto rebote.
  Cuando se pulse, se asignará a un Ticker una función.
  Pero la misma función llevará un control de las veces que ha sido llamada.
  Superado cierto límite, desactivará el Ticker que le está llamando y 
  dejará de repetirse, hasta la próxima pulsación del interruptor.

  Resultado:
  El led integrado parpadea sólo tres veces cada vez que se pulsa el interruptor
*/

//Incluimos la librería de la clase Ticker
#include <Ticker.h>

//Creamos un objeto Ticker
Ticker blinker;
//Contador de ejecuciones de la función Blink
byte blinkCounter;

void setup() 
{
  //Configuramos el pin del led integrado como salida
  pinMode(BUILTIN_LED, OUTPUT);
  //Apagamos el led integrado
  digitalWrite(BUILTIN_LED,HIGH);
  //Configuramos el pin del interruptor como entrada con resistencias internas de pullup
  pinMode(D1,INPUT_PULLUP);
  //Definimos una interrupción por hardware cuando se pulse el interruptor
  //conectado al pin D1
  attachInterrupt(D1,IntSwitch,FALLING);
}
void loop() 
{
  //No tenemos nada especial que hacer en la función loop()
}

void IntSwitch()
{
  //Se ha pulsado el interruptor

  //Reseteamos el número de ejecuciones
  blinkCounter=0;
  //Asignamos la función Blink al Ticker blinker para que se ejecute cada 0.2s
  blinker.attach(0.2,Blink);
}

void Blink()
{
  //Cambia el estado del led integrado
  digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
  //Aumentamos el número de ejecuciones de la función
  //Si supera el límite de ejecuciones previstas...desactivamos el objeto Ticker
  if(++blinkCounter > 5) blinker.detach();
}
