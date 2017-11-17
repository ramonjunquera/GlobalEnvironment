/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja AX-1838HS
  Objetivo: Funcionamiento de una fotorresistencia infrarroja AX-1838HS con librería IRremote.h
  Material: breadboard, receptor de infrarrojos modelo AX-1838HS, 4 leds, 4 resistencias de 220 ohmios,
    ,cables, mando a distancia de televisión, Arduino Nano.

  Descripción:
  Basándonos en el ejemplo anterior en el que podíamos visualizar los códigos de las teclas pulsados en el mando
  a distancia, utilizaremos esos códigos para interactuar con un circuito.

  Suponemos que el mando a distancia tiene los botones del teletexto. Cuatro botones de colores.
  Habitualmente en el orden rojo, verde, amarillo y azul.
  Usaremos cuatro leds de esos mismos colores. Cuando pulsemos uno de los cuatro botones, el led del mismo
  color cambiará su estado.
  Para ello debemos utilizar el programa anterior para obtener los códigos de los cuatro botones.

  Resultado:
  Al pulsar los botones de colores del teletexto, cambia el estado del led del mismo color.
*/

#include <Arduino.h>
#include <IRremote.h>

//Definición de pines
const byte pinIRrec=2;
const byte pinLedRed=3;
const byte pinLedGreen=4;
const byte pinLedYellow=5;
const byte pinLedBlue=6;

//Creación de objetos globales
IRrecv ir(pinIRrec);

void setup()
{
  //Activamos el receptor de infrarrojos
  ir.enableIRIn();
  //Activamos todos los pines de los leds como salida
  pinMode(pinLedRed,OUTPUT);
  pinMode(pinLedGreen,OUTPUT);
  pinMode(pinLedYellow,OUTPUT);
  pinMode(pinLedBlue,OUTPUT);
}

void loop()
{
  //Creamos objeto para recibir los resultados del receptor infrarrojo
  decode_results results;
  //Si se ha recibido algo por el receptor infrarrojo...
  if(ir.decode(&results))
  {
    switch(results.value)
    {
      case 0xE6BD0: //Botón rojo
        //Cambiamos estado del led rojo
        digitalWrite(pinLedRed,!digitalRead(pinLedRed));
      break;
      case 0x16BD0: //Botón verde
        //Cambiamos estado del led verde
        digitalWrite(pinLedGreen,!digitalRead(pinLedGreen));
      break;
      case 0x96BD0: //Botón amarillo
        //Cambiamos estado del led rojo
        digitalWrite(pinLedYellow,!digitalRead(pinLedYellow));
      break;
      case 0x66BD0: //Botón azul
        //Cambiamos estado del led rojo
        digitalWrite(pinLedBlue,!digitalRead(pinLedBlue));
      break;                  
      default: //Cualquier otro código
        //No lo tenemos en cuenta
      break; 
    }
    //Estamos preparados para recibir el siguiente código
    ir.resume();
  }
  
  
}
