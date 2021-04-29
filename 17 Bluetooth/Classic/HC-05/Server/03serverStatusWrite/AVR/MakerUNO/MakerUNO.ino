/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180827
  Objetivo: Conexión bidireccional con servidor
  Material: placa MakerUNO, módulo HC-05

  Descripción:
  Basado en el ejemplo anterior.
  El interruptor integrado cambia el estado del led integrado e informa a
  posibles clientes.
  Si se recibe el código 0 se apagará el led.
  Si se recibe el código 1 se encenderá el led.

  Si se configuran en la aplicación "Serial Bluetooth Terminal" los dos primeros 
  botones con los códigos 0 y 1, será más sencillo enviarlos al servidor.

  Resultado:
  Se puede modificar el led del servidor con comandos enviados desde un cliente
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
  //Si hay caracteres pendientes de ser recibidos...
  if(BT.available())
  {
    //...anotamos el byte recibido
    char rxChar=BT.read();
    //Dependiendo de lo que hayamos recibido...
    switch(rxChar)
    {
      case '0': //Código para apagar el led
        //Nuevo estado = apagado
        currentStatus=0;
        //Aplicamos estado al led
        digitalWrite(LED_BUILTIN,currentStatus);
        break;
      case '1': //Código para encender el led
        //Nuevo estado = encendido
        currentStatus=1;
        //Aplicamos estado al led
        digitalWrite(LED_BUILTIN,currentStatus);
        break;
      //El resto de posibles caracteres no se tienen en cuenta
    }
  }
}
