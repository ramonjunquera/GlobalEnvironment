#include <Arduino.h>

//Variables globales
byte pinSwitch=12; //Pin del interruptor
unsigned long minPulse=200; //Duración del pulso en milisegundos

void setup()
{
  //Configuramos el pin 13 como salida (led IR)
  pinMode(13, OUTPUT);
  //Configuramos el pin del interruptor como entrada con resistencia de PULLUP
  pinMode(pinSwitch,INPUT_PULLUP);
}

void loop()
{
  //Si el interruptor se ha pulsado...
  if(digitalRead(pinSwitch)==LOW)
  {
    //Encedemos el led IR
    digitalWrite(13,HIGH);
    //Esperamos el tiempo del pulso
    delay(minPulse);
    //Apagamos el led IR
    digitalWrite(13,LOW);
    //Esperamos un momento para evitar el rebote de pulsación
    delay(100);
    //Esperamos a que se suelte el interruptor
    while(digitalRead(pinSwitch)==LOW)
    {
      //...esperando...
    }
    //Se ha soltado el interruptor
    //Esperamos un momento para evitar el rebote de soltar el pulsador
    delay(100);
  }
}
