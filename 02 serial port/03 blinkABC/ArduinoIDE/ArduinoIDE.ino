/*
  Autor: Ramón Junquera
  Tema: Puerto serie
  Objetivo: Cómo se recibe la información por el puerto serie para comunicarse con el ordenador
  Material adicional: cualquier placa

  Descripción:
  Analizaremos el texto recibido por el puerto serie y haremos parpadear el led integrado en
  placa tantas veces como caracteres encontremos entre la A y la C (ABC).
  
  Resultado:
  El led parpadea por cada letra ABC que encuentra
*/ 

#include <Arduino.h>

const byte pinLed=LED_BUILTIN; //Led 

void setup()
{
  //Inicializamos el puerto serie a la velocidad de 9600 baudios
  Serial.begin(115200);
  //Definimos el pin 13 como salida
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  //Si hay caracteres esperando en el buffer del serial...
  if (Serial.available())
  {
    //Tomamos nota del siguiente caracter
    char inChar = Serial.read();
    //Si está entre la A y la C...
    if (inChar>='A' && inChar<='C')
    {
      //Hacemos parpadear el led integrado
      digitalWrite(pinLed,HIGH);
      delay(200);
      digitalWrite(pinLed,LOW);
      delay(200);

    }
  }
}


