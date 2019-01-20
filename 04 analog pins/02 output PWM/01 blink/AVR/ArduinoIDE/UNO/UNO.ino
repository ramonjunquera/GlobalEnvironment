/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: breadboard, led, resistencia de 220 ohmios, placa Arduino UNO o MakerUNO

  Descripción:
  Haremos variar la intensidad de un led conectado a un pin con capacidades PWM
  progresivamente desde apagado hasta el máximo y después a la inversa.

  Aunque las placas Arduino tienen un led integrado que habitualmente está
  asociado con el pin 13, este pin no tiene capacidades PWM y no podemos usarlo.
  Por eso utilizamos un led externo.

  Nota:
  La placa MakerUno no necesita de componentes externos para este ejercicio.
  El pin 3 ya tienen su correspondiente pin integrado en placa.

  Resultado:
  El led se enciende y apaga progresivamente
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = 3;

void setup()
{
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed, OUTPUT);
}

byte level=0; //Nivel PWM
int8_t delta=1; //Dirección

void loop()
{
  //Escribimos el nivel PWM en el pin del led
  analogWrite(pinLed,level);
  //Calculamos el valor del nivel del próximo ciclo
  level+=delta;
  //Si hemos llegado a alguno de los extremos...cambiamos de dirección
  if(level==0 || level==0xFF) delta=-delta;
  //Esperamos un momento
  delay(5);
}
