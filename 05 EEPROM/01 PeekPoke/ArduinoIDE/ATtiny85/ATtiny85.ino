/*
  Autor: Ramón Junquera
  Fecha: 20191030
  Tema: Gestión de la memoria EEPROM del ATtiny85
  Objetivo: Leer y escribir valores en la memoria EEPROM
  Material adicional: breadboard, cables, interruptor, ATtiny85

  Descripción:
  ATtiny85 contiene 512 bytes de memoria EEPROM.
  El funcionamiento es exáctamente igual que en el resto de placas de Arduino.
  La librería utilizada es la misms: EEPROM.h
  El problema es que esta librería no se encuentra por defecto en el entorno de desarrollo
  de Digispark. Por lo tanto, si no hacemos nada, se nos mostrará el mensaje de librería 
  no encontrada.

  La solución es sencilla. Se debe copiar la carpeta que contiene la librería desde el entorno
  normal de Arduino al de Digispark.

  Habitualmente el origen es:
  C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM
  Y el destino:
  C:\Users\<user>\AppData\Local\Arduino15\packages\digistump\hardware\avr\1.6.7\libraries

  El circuito consta de un interruptor que cambia el estado del led integrado en placa y que 
  al mismo tiempo, guarda el estado en el primer byte de la memoria EEPROM.
  Esto permite que tras un reset, se recupere el último estado del led.

  Para gestionar el interruptor se utiliza la interrupción hardware desde los comandos de
  Arduino.

  No se tiene en cuenta el efecto rebote. No es necesario para la demostración de este ejemplo.
  Así que es posible que algunas veces falle el cambio de estado del led.
    
  Resultado:
  Tras resetear la placa, se recupera automáticamente el último estado del led
*/

#include <Arduino.h>
#include "EEPROM.h"

//Declaración de constantes globales
const byte pinLed = 1; //pin del led
const byte pinSwitch = 2; //pin al que está conectado el interruptor
const byte intSwitch = 0; //número de interrupción asociada al pin del interruptor

void IntSwitch() {
  //Función a la que se llama cuando se pulsa el interruptor

  //Anotamos el nuevo estado del led
  byte newStatus=!digitalRead(pinLed);
  //Escribimos el nuevo estado en el led
  digitalWrite(pinLed,newStatus);
  //Escribimos el nuevo estado en la primera posición de la EEPROM
  EEPROM.write(0,newStatus);
}


void setup() {
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Fijamos el estado del led dpendiendo de si la primera posición de la EEPROM contiene el valor 0
  digitalWrite(pinLed,EEPROM.read(0)!=0);
  //Activamos las resistencias de pull-up para el pin de interruptor
  //Por defecto el pin del interruptor estará en HIGH y cuando se pulse en LOW
  pinMode(pinSwitch,INPUT_PULLUP);
  //Definimos la interrupción
  //Se llamará a la función IntSwitch
  //Ocurrirá cuando se pulse el interruptor (pase de HIGH a LOW)
  attachInterrupt(intSwitch,IntSwitch,FALLING);
}

void loop() {
  //No tenemos nada especial que hacer
  //El pulsador se gestiona por interrupciones
}
