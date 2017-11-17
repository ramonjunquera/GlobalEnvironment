//Desarrollo para el emisor (Arduino UNO)

#include <Arduino.h>
#include <IRremote.h>

//Definición de pines
const byte pinLed=2;

//Creación de objetos globales
IRsend ir;

void setup()
{
  //El pin del led será de salida
  pinMode(pinLed,OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(pinLed,HIGH);
  //Enviamos el código de power con el protocolo de Sony
  //El código power es el A90 (en hezadecimal)
  //También debemos indicar el número de bits que queremos enviar. En este caso son 3 medios bytes = 3*4 = 12 bits
  ir.sendSony(0xa90,12);
  //Apagamos el led
  digitalWrite(pinLed,LOW);
  //Esperamos un segundo a enviar el siguiente código
  delay(1000);
}
