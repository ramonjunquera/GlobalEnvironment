/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180827
  Objetivo: Informar del estado del servidor
  Material: placa MakerUNO, módulo HC-05

  Descripción:
  Mediante un interruptor cambiaremos el estado del led integrado.
  Utilizamos la placa MakerUNO porque tiene un interruptor de pulsador
  integrado conectado al pin 2.
  Esta placa, al igual que la UNO, tiene la variable LED_BUILTIN asociada
  al pin 13.
  Cuando cambia su estado informará a todos los clientes que están conectados:
   0=apagado, 1=encendido

  Resultado:
  El servidor informa de su estado a los clientes
*/

#include <Arduino.h>
#include <SoftwareSerial.h> //Para crear un puerto serie para HC-05
#include "RoJoSwitch.h" //Gestión de interruptores

//Constantes globales
const byte pinSwitch=2; //Pin asignado a interruptor integrado en MakerUNO
const byte pinTX = 11;
const byte pinRX = 12;

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
  digitalWrite(LED_BUILTIN,currentStatus);
}

void loop()
{
  //Si se ha pulsado el botón...
  if(mySwitch.pressed())
  {
    //...cambiamos el estado actual
    currentStatus=1-currentStatus;
    //Aplicamos el nuevo estado el led integrado
    digitalWrite(LED_BUILTIN,currentStatus);
    //Informamos del nuevo estado a los posibles clientes
    BT.println(currentStatus);
  }
}
