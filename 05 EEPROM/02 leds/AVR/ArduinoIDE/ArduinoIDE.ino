/*
  Autor: Ramón Junquera
  Tema: Gestión de la memoria EEPROM
  Objetivo: Guardar valores por defecto en una memoria no volátil de la placa Arduino
  Material adicional: breadboard, cables,cualquier placa Arduino, 3 interruptores,
    3 leds y 3 resistencias de 220 ohmios

  Descripción:
  Crearemos un circuito simple que contenga 3 leds de distintos colores y 3 interruptores de pulsador.
  Cada pulsador encenderá/apagará uno de los leds.
  Para simplificarlo, utilizaremos la librería de gestión de pulsadores (RoJoSwitch)
  La novedadad es que guardaremos el estado de los leds en la memoria EEPROM para que la próxima ves que iniciemos
  la placa, recuperemos la situación actual.
  
  Desperdiciaremos memoria y utilizaremos un byte para guardar sólo un booleano. 0 significará false, y cualquier otra
  cosa true.
  
  Teniendo en cuenta la poca cantidad de memoria EEPROM que tenemos, deberíamos aprovecharla mejor.
  Si no fuese un ejemplo, se deberían guardar los estados de los leds en bits agrupados en bytes.
    
  Resultado:
  Si reiniciamos la placa, el estado se conserva.
*/

#if !defined(ARDUINO_ARCH_AVR)
  #error Solo las placas Arduino tienen memoria EEPROM integrada
#endif 

#include <Arduino.h>
#include <EEPROM.h>
#include "RoJoSwitch.h"

//Declaración de variables globales
//Número total de leds
const byte totalLed=3;
//Pines de conexión de los leds
const byte pinLed[3]={2,4,6};
//Estado de cada uno de los leds
bool statusLed[3];
//Objetos de gestión de los interruptores
RoJoSwitch myButton[3]={RoJoSwitch(3),RoJoSwitch(5),RoJoSwitch(7)};

void setup()
{
  //Recorremos todos los leds
  for(byte led=0;led<totalLed;led++)
  {
    //Los pines de los leds son de salida
    pinMode(pinLed[led],OUTPUT);
    //También recuperamos los estados de cada uno de los leds
    statusLed[led]=EEPROM.read(led)>0;
    //Aplicamos el estado que corresponde
    digitalWrite(pinLed[led],statusLed[led]);
  }
}

void loop()
{
  //Recorremos todos los leds
  for(byte led=0;led<totalLed;led++)
  {
    //Si se ha pulsado su interruptor...
    if(myButton[led].pressed())
    {
      //...cambiamos el estado del led...
      statusLed[led]=!statusLed[led];
      //...lo aplicamos...
      digitalWrite(pinLed[led],statusLed[led]);
      //...lo guardamos en la memoria EEPROM
      EEPROM.write(led,statusLed[led]);
    }
  }
}

