/*
  Autor: Ramón Junquera
  Tema: Puerto serie
  Objetivo: Cómo se recibe la información por el puerto serie para comunicarse con el ordenador
  Material adicional: cualquier placa

  Descripción:
  Analiza el texto recibido por el puerto serie e intenta obtener un valor numérico entero.
  Hace partadear el led integrado tantas veces como se indique en ese valor.
  
  Resultado:
  Vemos parapadear el led tantas veces como indiquemos

  Nota:
  Aunque se admiten números negativos o cero, ninguno de ellos hace parpadear el led.
  parseInt saltará todos los caracteres sin significado numérico hasta encontrar lo que
  busca. Una vez que lo encuentre lo tendrá en cuenta hasta que encuentre otro carácter
  sin significado numérico o reciba un timeout porque no hay más caracteres.
*/ 

#include <Arduino.h>

const byte pinLed=LED_BUILTIN; //Led integrado

void setup()
{
  //Inicializamos el puerto serie a 115200 baudios
  Serial.begin(115200);
  //Definimos el pin 13 como salida
  pinMode(pinLed, OUTPUT);
}

void loop()
{
  //Si hay caracteres en el buffer del serial...
  if(Serial.available())
  {
    //Intentamos obtener el valor numérico entero del texto recibido
    int32_t maxCounter=Serial.parseInt();
    //Repetiremos tantas veces como se haya indicado
    for(int32_t counter=0;counter<maxCounter;counter++)
    {
      //Hacemos parpadear una vez el led integrado
      digitalWrite(pinLed,HIGH);
      delay(200);
      digitalWrite(pinLed,LOW);
      delay(200);
    }
  }
}



