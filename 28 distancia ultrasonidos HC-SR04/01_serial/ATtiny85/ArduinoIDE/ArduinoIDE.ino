/*
  Autor: Ramón Junquera
  Fecha: 20191029
  Tema: Instrucciones soportadas por ATtiny85
  Objetivo: Uso de función PulseIn
  Material adicional: breadboard, cables, placa ATtiny85, sensor medidor de distancia por ultrasonidos HC-SR04

  Descripción:
  El típico ejemplo para el uso del comando PulseIn es el uso de un sensor de distancia por
  ultrasonidos.
  Utilizaremos el sensor HC-SR04.

  Tomaremos la distancia periódicamente y la enviaremos a través del puerto serie creado con
  la librería DigiCDC.

  El ejemplo está basado en el creado originalmente para Arduino.
  Sólo contiene ligeras adaptaciones.

  Resultado:
  Se envia contínuamente al puerto serie USB la distancia detectada por el sensor en centímetros
*/

#include <Arduino.h>
#include <DigiCDC.h> //Incluimos la librería de DigiCDC para el debug

//Declaración de constantes globales
const byte pinTrig=1; //Pin del trigger
const byte pinEcho=0; //Pin del echo

void setup() {
  //Inicializamos el puerto serie USB. No se indica velocidad!
  SerialUSB.begin();
  //Definimos el pin del trigger como salida (generación del pulso de ultrasonidos)
  pinMode(pinTrig,OUTPUT);
  //Definimos el pin del echo como entrada (tiempo de rebote del ultrasonido)
  pinMode(pinEcho,INPUT);
}

void loop() {
  //Comenzamos desactivando el sensor
  digitalWrite(pinTrig,LOW);
  //Esperamos un momento
  SerialUSB.delay(1);
  //Activamos el pulso de ultrasonidos
  digitalWrite(pinTrig,HIGH);
  //Esperamos un instante para que llegue el eco
  SerialUSB.delay(1);  
  //Apagamos el trigger. Realmente no es necesario, porque ya lo hacemos al principio.
  digitalWrite(pinTrig,LOW);
  //Calculamos la distancia multiplicando la constante por el tiempo transcurrido entre dos pulsos
  //El resultado vendrá en centímetros
  SerialUSB.println(int(0.01715*pulseIn(pinEcho,HIGH)));
  //Esperamos medio segundo para volver a tomar una nueva medida
  SerialUSB.delay(500);
}
