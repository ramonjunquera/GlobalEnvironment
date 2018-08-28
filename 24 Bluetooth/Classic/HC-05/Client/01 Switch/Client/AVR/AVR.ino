#include <Arduino.h>
#include <SoftwareSerial.h> //Para crear un puerto serie para HC-05
#include "RoJoSwitch.h" //Gestión de interruptores

//Constantes globales
const byte pinSwitch=2; //Pin asignado a interruptor integrado en MakerUNO
const byte pinTX = 11;
const byte pinRX = 12;

//Variables globales
RoJoSwitch mySwitch(pinSwitch); //Objeto de gestión del interruptor
SoftwareSerial BT(pinTX,pinRX); //Puerto software para bluetooth

void setup()
{
  //Activamos el puerto serie software para el HC-05 con velocidad estándar
  BT.begin(38400);
  //Configuramos el led como salida
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  //Si se pulsa el botón...se envía el código de cambio al servidor
  if(mySwitch.pressed()) BT.println("X");
  //Si hay caracteres pendientes de ser recibidos...
  if(BT.available())
  {
    //...anotamos el byte recibido
    char rxChar=BT.read();
    //Si es un código permitido...aplicamos el estado correspondiente al led
    if(rxChar=='0' || rxChar=='1') digitalWrite(LED_BUILTIN,rxChar=='1');
  }
}
