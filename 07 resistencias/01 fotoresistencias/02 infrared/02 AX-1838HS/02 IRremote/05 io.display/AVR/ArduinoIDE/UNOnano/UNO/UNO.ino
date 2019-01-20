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
  //Activamos puerto serie para mensajes de debug
  Serial.begin(115200);
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
    //Enviamos el cógigo del carácter pulsado con el protocolo RC5
    //Los protocolos se Sony o Nec no son apropiados para transmisión de datos
    //Aunque sólo enviamos un byte (8 bits) el protocolo de RC5 necesita enviar 9 bits como mínimo
    //si lo reducimos a 8, los caracteres más altos (como el *) se enviarán incompletos y no se
    //podrán decodificar correctamente.
    ir.sendRC5(key,9);
    //Lo enviamos al puerto serie para debug
    Serial.println(key);
    //Apagamos el led
    digitalWrite(pinLed,LOW);
  }
}
