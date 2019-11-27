/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Lectura de estado de un pin
  Objetivo: Comprobar si tenemos resistencias de pullup internas
  Material adicional: ATtiny85, breadboard, pulsador

  Descripción:
  Haremos una evolución del ejercicio anterior.
  Sustituiremos la resistencia pullup real por una de las integradas en el
  ATtiny85.

  El circuito varía ligeramente porque desaparece la resistencia real.

  También varía el programa porque ahora se configura el pin como INPUT_PULLUP

  Nota:
  De la misma manera que en placas comunues como la UNO, no existen las resistencias
  pulldown internas.
  
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
  pinMode(pinButton,INPUT_PULLUP);
}

void loop() {
  //Escribimos en el pin del led el estado inverso al pin del pulsador
  digitalWrite(pinLed,!digitalRead(pinButton));
}
