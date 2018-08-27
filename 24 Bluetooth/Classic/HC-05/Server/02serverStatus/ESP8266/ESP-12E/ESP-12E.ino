/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180827
  Objetivo: Informar del estado del servidor
  Material: placa ESP-12E, módulo HC-05

  Descripción:
  Mediante un interruptor cambiaremos el estado del led integrado.
  Utilizamos la placa ESP-12E porque tiene un interruptor de pulsador
  integrado conectado al pin D3 además del led.
  Cuando cambio su estado informará a todos los clientes que están conectados:
   0=apagado, 1=encendido
  El led integrado de esta placa tiene invertido su estado respecto al pin.

  Resultado:
  El servidor informa de su estado a los clientes
*/

#include <Arduino.h>
#include <SoftwareSerial.h> //Para crear un puerto serie para HC-05
#include "RoJoSwitch.h" //Gestión de interruptores

//Constantes globales
const byte pinSwitch=D3; //Pin asignado a interruptor integrado en ESP-12E
const byte pinTX = D7;
const byte pinRX = D6;

//Variables globales
byte currentStatus=0; //Comenzamos con el led apagado. 0=apagado, 1=encendido
RoJoSwitch mySwitch(pinSwitch); //Objeto de gestión del interruptor
SoftwareSerial BT(pinTX,pinRX); //Puerto software para bluetooth

void setup()
{
  //Activamos el puerto serie software para el HC-05 con velocidad estándar
  BT.begin(38400);
  //Configuramos el led como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Asignamos el estado inicial
  //Recordemos que en esta placa el estado del led es inverso al del pin
  digitalWrite(LED_BUILTIN,1-currentStatus);
}

void loop()
{
  //Si se ha pulsado el botón...
  if(mySwitch.pressed())
  {
    //...cambiamos el estado actual
    currentStatus=1-currentStatus;
    //Aplicamos el nuevo estado el led integrado
    digitalWrite(LED_BUILTIN,1-currentStatus);
    //Informamos del nuevo estado a los posibles clientes
    BT.println(currentStatus);
  }
}
