/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Sistemas de debug en ATtiny85. Librería DigiCDC.h
  Objetivo: Lectura e interacción
  Material adicional: ATtiny85, breadboard

  Descripción:
  En este ejercicio leeremos la información recibida por el puerto serie USB.
  Comprobaremos si alguno de los caracteres es numérico.
  Si lo es, haremos parpaderar el led integrado en placa tantas veces como
  su valor numérico.

  Nota:
  La clase SerialUSB no tiene definido el método parseInt como tiene Serial.

  Resultado:
  Controlamos el número de veces que parpadea el led integrado con comandos
  enviados por el puerto serie USB.
*/

#include <Arduino.h>
#include <DigiCDC.h> //Gestión de puerto serie

//Definición de constantes globales
const byte pinLed=1;

void setup() {
  //Inicializamos el puerto serie USB. No se indica velocidad!
  SerialUSB.begin();
  //Definimos el pin del led integrado en placa como salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
}

void loop() {
  //Si tenemos algún carácter pendiente de ser recibido...
  if (SerialUSB.available()) {
    //Anotamos el carácter recibido
    char c = SerialUSB.read();
    //Si se trata de un carácter numérico...
    if ((c >= '0') && (c <= '9')) {
      //Obtenemos su valor numérico
      byte maxCounter = c - '0';
      //Repetiremos tantas veces como se haya indicado
      for (byte counter=0;counter<maxCounter;counter++) {
        //Hacemos parpadear una vez el led integrado
        digitalWrite(pinLed,HIGH);
        SerialUSB.delay(200);
        digitalWrite(pinLed,LOW);
        SerialUSB.delay(200);
      }
    }
  }
}
