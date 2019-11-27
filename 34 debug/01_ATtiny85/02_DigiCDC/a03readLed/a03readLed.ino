/*
  Autor: Ramón Junquera
  Fecha: 20191027
  Tema: Sistemas de debug en ATtiny85. Librería DigiCDC.h
  Objetivo: Lectura e interacción
  Material adicional: ATtiny85, breadboard

  Descripción:
  El ejercicio consiste en leer la información recibida.
  Si se detecta el carácter 1, se encenderá el led integrado en placa.
  Si se detecta el carácter 0, se apagará.
  El resto de caracteres no se tendrán en cuenta.
  
  Resultado:
  Podemos controlar el led integrado en placa con comandos enviados por el puerto serie
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
    //Decidimos qué hacer con el carácter recibido...
    switch(SerialUSB.read()) {
      case '1': //Si el carácter es el 1...
        digitalWrite(pinLed,HIGH); //...encendemos el led
        break;
      case '0': //Si el carácter es el 0...
        digitalWrite(pinLed,LOW); //...apagamos el led
        break;
    }
  }
}
