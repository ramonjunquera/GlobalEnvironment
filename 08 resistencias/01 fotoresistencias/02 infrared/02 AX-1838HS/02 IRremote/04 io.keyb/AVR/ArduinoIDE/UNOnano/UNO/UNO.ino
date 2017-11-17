//Desarrollo para el emisor (Arduino UNO)

#include <Arduino.h>
#include <IRremote.h>
#include <RoJoKeypad4x4.h>

//Definición de constantes
const byte pinLed=2; //Pin del led de actividad

//Creación de objetos globales
IRsend ir;
//Creamos objeto keypad basado en clase RoJoKeypad4x4 para gestión del teclado
RoJoKeypad4x4 keypad = RoJoKeypad4x4(11,10,9,8,7,6,5,4);

void setup()
{
  //El pin del led será de salida
  pinMode(pinLed,OUTPUT);
}

void loop()
{
  //Anotamos la tecla pulsada
  char key = keypad.get();
  //Si hay alguna tecla pulsada...
  if (key)
  {
    //Encendemos el led
    digitalWrite(pinLed,HIGH);
    //Enviamos el cógigo del carácter pulsado con el protocolo de Sony
    //Aunque sólo enviamos un byte (8 bits) el protocolo de Sony envía siempre un mínimo de 12 bits
    //Si lo reducimos, la información se corromperá
    ir.sendSony(key,12);
    //Apagamos el led
    digitalWrite(pinLed,LOW);
  }
}
