/*
  Autor: Ramón Junquera
  Fecha: 20180609
  Tema: Gestión de la memoria EEPROM
  Objetivo: Guardar valores por defecto en una memoria no volátil (EEPROM)
  Material: breadboard, cables,cualquier placa Arduino, 4 interruptores,
    4 leds y 4 resistencias de 220 ohmios (para placas Arduino)

  Descripción:
    Crearemos un circuito simple que contenga 4 leds de distintos colores y 4 interruptores de pulsador.
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

  Nota:
    El ejemplo está preparado para cambiar fácilmente el número de led/interruptores.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "RoJoSwitch.h"

//Declaración de constantes globales
const byte totalLed=4; //Número total de leds
#ifdef ARDUINO_ARCH_AVR //Si es una placa Arduino...
  const byte pinLed[]={2,4,6,8}; //Pines de conexión de los leds
  const byte pinSwitch[]={3,5,7,9}; //Pines de conexión de los interruptores
#elif defined(ESP8266)
  const byte pinLed[]={D0,D6,D8,D3}; //Pines de conexión de los leds
  const byte pinSwitch[]={D5,D7,D4,D2}; //Pines de conexión de los interruptores
#elif defined(ESP32)
  const byte pinLed[]={32,25,27,12}; //Pines de conexión de los leds
  const byte pinSwitch[]={33,26,14,13}; //Pines de conexión de los interruptores
#endif

//Declaración de variables globales
bool statusLed[totalLed]; //Estado de cada uno de los leds
RoJoSwitch myButton[totalLed]; //Array de objetos de gestión de los interruptores

void setup()
{
  //En las placas ESP8266 el tamaño de mamoria EEPROM debe ser inicializado
  #if defined(ESP8266) || defined(ESP32)
    //Podemos definir el tamaño de la memoria EEPROM entre 1 y 4096 bytes
    EEPROM.begin(4096);
  #endif
  
  //Recorremos todos los leds
  for(byte led=0;led<totalLed;led++)
  {
    //Creamos el objeto de gestión de este interruptor
    myButton[led]=RoJoSwitch(pinSwitch[led]);
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
      //Si la placa es ESP necesitamos hacer el commit
      #if defined(ESP8266) || defined(ESP32)
        EEPROM.commit();
      #endif
    }
  }
}

