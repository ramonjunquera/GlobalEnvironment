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
  //Inicialmente el estado debe cambiar si se ha pulsado el botón
  bool mustChangeStatus=mySwitch.pressed();
  //Si hay caracteres pendientes de ser recibidos...
  if(BT.available())
  {
    //...si el carácter recibido es X...debemos cambiar el estado
    if(BT.read()=='X') mustChangeStatus=true;
  }
  //Si debemos cambiar el estado...
  if(mustChangeStatus)
  {
    //...lo hacemos
    currentStatus=1-currentStatus;
    //Aplicamos el nuevo estado el led integrado
    digitalWrite(LED_BUILTIN,currentStatus);
    //Informamos del nuevo estado a los posibles clientes
    BT.println(currentStatus);
  }
}
