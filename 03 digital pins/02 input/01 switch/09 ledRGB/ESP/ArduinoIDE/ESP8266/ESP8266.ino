/*
  Autor: Ramón Junquera
  Tema: WeMosD1 R2 & Mini
  Versión: 20180605
  Objetivo: Lectura del estado de un pin digital
  Material: placa ESP8266, breadboard, 3 pulsadores, led RGB

  Descripción:
  El circuto consta de un led RGB de ánodo común y tres interruptores. Cada uno 
  de los cuales enciende un color.

  El objetivo es leer el estado de los pines de entrada a los que están conectados
  los interuptores.
  Se utilizan laa resistencias internas de pullup.
  Puesto que el estado del interruptor pulsado es LOW, como el del pin del led RGB
  de ánodo común, no tenemos que hacer ninguna conversión.

  Resultado:
  Cada pulsador enciende un color
*/

const byte pinSwitchR = D1;
const byte pinSwitchG = D2;
const byte pinSwitchB = D3;
const byte pinLedR = D6;
const byte pinLedG = D0;
const byte pinLedB = D5;

void setup()
{ 
  //Configuramos los de los interruptores como entrada con resistencias de pullup
  pinMode(pinSwitchR,INPUT_PULLUP); //R en pin D1
  pinMode(pinSwitchG,INPUT_PULLUP); //G en pin D2
  pinMode(pinSwitchB,INPUT_PULLUP); //B en pin D3
  
  //Configuramos los pines del led RGB como salidas
  pinMode(pinLedR, OUTPUT); //R en pin D6
  pinMode(pinLedG, OUTPUT); //G en pin D0
  pinMode(pinLedB, OUTPUT); //B en pin D5
}

void loop()
{
  //Los leds tendrán el estado de los interruptores
  digitalWrite(pinLedR,digitalRead(pinSwitchR)); //R
  digitalWrite(pinLedG,digitalRead(pinSwitchG)); //G
  digitalWrite(pinLedB,digitalRead(pinSwitchB)); //B
}
