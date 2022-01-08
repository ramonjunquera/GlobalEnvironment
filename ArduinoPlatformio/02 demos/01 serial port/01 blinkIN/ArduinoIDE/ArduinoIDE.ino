/*
  Autor: Ramón Junquera
  Tema: Puerto serie
  Objetivo: Cómo se recibe la información por el puerto serie para comunicarse con el ordenador
  Material adicional: cualquier placa
  Descripción:
  Por cada carácter recibido por el puerto serie, haremos parpadear una vez el led integrado en placa.
  Es importante configurar la consola del puerto serie a la misma velocidad que definimos en el programa.
  
  Resultado:
  Vemos parapadear el led cada vez que enviamos texto por el puerto serie.

  Nota:
  Dependiendo de cómo tengamos configurada monitor serie, cambiará el número de caracteres que se reciben.
  En el modo "Sin ajuste de línea" se recibirán tantos caracteres como escribamos.
  En el modo "Ambos NL & CR" se recibirán dos caracteres extra correspondientes al caracter de nueva
  línea (10) y el de retorno de carro (13).
*/ 

#include <Arduino.h>

const byte pinLed=LED_BUILTIN;

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
  if(Serial.available()>0)
  {
    //Leemos el siguiente carácter
    char c=Serial.read();
    //Hacemos parpadear una vez el led integrado
    digitalWrite(pinLed,HIGH);
    delay(200);
    digitalWrite(pinLed,LOW);
    delay(200);
  }
}



