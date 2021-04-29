/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Lectura de estado de un pin
  Objetivo: Prueba básica de lectura digital
  Material adicional: ATtiny85, breadboard, pulsador, resistencia de 10 Kohmios

  Descripción:
  Crearemos el circuito más básico para comprobar que se puede leer un pin digital.
  Consistirá en un pulsador que enciende el led integrado en placa.
  Utilizaremos una resistencia de pullup para que si no hay pulsación, mantenga el
  estado en HIGH.

  El programa configura los dos pines a utilizar y después transfiere al pin del 
  led integrado el contrario del estado leido.
  Recordemos que esto es así porque cuando no se pulsa el interruptor, se lee
  el estado HIGH debido a la resistencia pullup.
  
  Resultado:
  El led integrado se enciende al pulsar el interruptor
*/

//Definición de constantes globales
const byte pinLed=1;
const byte pinButton=0;

void setup() { 
  //Configuramos el pin del led como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
  //Configuramos el pin del pulsador como entrada para poder leerlo
  pinMode(pinButton,INPUT);
}

void loop() {
  //Escribimos en el pin del led el estado inverso al pin del pulsador
  digitalWrite(pinLed,!digitalRead(pinButton));
}
