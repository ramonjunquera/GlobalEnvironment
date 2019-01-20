/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180821
  Objetivo: Comunicación bidireccional de comandos
  Material: ESP32 OLED TTGO 16Mb, 3 leds, 3 pulsadores

  Descripción:
  El servidor consta de 3 leds y 3 interruptores.
  Cada interruptor cambia el estado de su led asociado.
  También acepta comandos procedentes de Bluetooth. 0, 1 y 2, cambian el estado de cada
  uno de los leds.
  Cuando cambia el estado de un led se informa al cliente Bluetooth.

  Para hacer más cómo el envío de comandos desde el cliente Bluetooth podemos programar
  los 3 primero botones de macros con "0", "1" y "2".

  Se ha preparado el programa para que sea sencillo ampliar el número de leds

  Resultado:
  Podemos gestionar los leds del servidor tanto desde local como en remoto por Bluetooth
  Cuando cambia el estado de un led, recibimos en el cliente un mensaje del estado actual.
*/

#include <Arduino.h>
#include "BluetoothSerial.h" //Gestión de Bluetooth
#include "RoJoSwitch.h" //Gestión de interruptores

//Constantes globales
const byte pinCount=3; //Número de pines/interruptores
const byte pinLed[]={26,14,13}; //Pines de los leds
const byte pinSwitch[]={25,27,12}; //Pines de los interruptores

//Variables globales
BluetoothSerial BT; //Objeto de gestión del bluetooth
bool currentStatus[pinCount]; //Estados actuales
RoJoSwitch switches[pinCount]; //Objetos de gestión de los interruptores

void setup()
{
  //Activamos el Bluetooth
  BT.begin("ESP32");

  //Recorremos todos los leds...
  for(byte i=0;i<pinCount;i++)
  {
    //Configuramos el pin del led como salida
    pinMode(pinLed[i],OUTPUT);
    //Lo apagamos
    digitalWrite(pinLed[i],LOW);
    //Anotamos su estado como apagado
    currentStatus[i]=LOW;
    //Creamos el objeto de gestión del interruptor asociado
    switches[i]=RoJoSwitch(pinSwitch[i]);    
  }
}

void changeLed(byte indexLed)
{
  //Cambia el estado de un led e informa por Bluetooth

  //Cambiamos el estado actual
  currentStatus[indexLed]=!currentStatus[indexLed];
  //Aplicamos el nuevo estado al led
  digitalWrite(pinLed[indexLed],currentStatus[indexLed]);
  //Informamos por BT
  //BT.println(String(indexLed)+" is "+currentStatus[indexLed]?"ON":"OFF"); 
  BT.print(indexLed);
  BT.print(" is ");
  BT.println(currentStatus[indexLed]?"ON":"OFF"); 
}

void loop()
{
  //Recorremos los 3 interruptores
  for(byte i=0;i<pinCount;i++)
  {
    //Si se ha pulsado el botón...cambiamos el estado de su led
    if(switches[i].pressed()) changeLed(i);
  }
  
  //Si hay datos pendientes de recibir por el bluetooth...
  if (BT.available())
  {
    //...recogemos el byte
    byte rx=BT.read();
    //Si es válido (entre 0 y el máximo número de pines)...cambiamos el estado del led correspondiente
    if(rx>=48 && rx<48+pinCount) changeLed(rx-48);    
  }
}
