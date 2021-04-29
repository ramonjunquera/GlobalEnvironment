/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Lectura de estado de un pin
  Objetivo: Comprobar cuántos pines pueden ser de entrada
  Material adicional: ATtiny85, breadboard, 4 pulsadores

  Descripción:
  Hasta ahora hemos descubierto que podemos configurar resistencias de pullup
  internas y que el pin 0 puede ser leido de manera digital.
  En este ejercicio intentaremos comprobar el resto de pines.
  Puesto que el pin 1 es el que está asociado al led integrado, y es la señal
  que nos sirve para saber si está pulsado algún interruptor, no podremos
  comprobarlo aquí.
  Pero pondremos pulsadores en los pines 0, 2, 3 y 4.
  Configuraremos para todos ellos resistencisa internas de pullup.
  Comprobaremos recuencialmente el estado de cada interruptor y si encontramos
  alguno pulsado, encenderemos el led.

  Con uno sólo que esté pulsado ya encendermeos el led. Ante el primero que 
  encontremos, dejamos de mirar el resto.

  Nota:
  Este proyecto NO funciona cuando suministramos la alimentación a través del USB
  porque interfiere en el estado de los pines.
  Se debe alimentar con una fuente externa a través de los pines GND & 5V

  Resultado:
  El led integrado se enciende al pulsar cualquiera de los cuatro interruptores
*/

//Definimos los pines en los que tenemos los interruptores
const byte pinSwitches[]={0,2,3,4};
const byte pinLed=1;

void setup() { 
  //Configuramos el pin 1 como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
  //Configuramos los pines de los pulsadores como entrada con resistencias 
  //internas de pullup
  for(byte i=0;i<4;i++) pinMode(pinSwitches[i],INPUT_PULLUP);
}

void loop() {
  //Inicialmente el led estará apagado
  bool ledStatus=false;
  //Recorremos todos los interruptores
  for(byte i=0;i<4;i++) {
    //Si el interruptor está pulsado...
    if(!digitalRead(pinSwitches[i])) {
      //...el led deberá estar encendido
      ledStatus=true;
      //No hace falta seguir mirando más interruptores. Salimos del bucle
      exit;
    }
  }
  //Aplicamos el estado al led
  digitalWrite(pinLed,ledStatus);
}
