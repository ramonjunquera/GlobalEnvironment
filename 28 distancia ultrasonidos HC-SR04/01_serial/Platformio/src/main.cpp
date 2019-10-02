/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia HC-SR04
  Fecha: 20190929
  Objetivo: Aprender el funcionamiento del sensor de distancia por ultrasonidos.

  Resultado:
    Recibimos contínuamente la distancia del objeto más próximo al sensor
*/

#include <Arduino.h>

//Declaración de variables globales
#ifdef ARDUINO_ARCH_AVR
  const byte pinTrig=9; //Pin del trigger
  const byte pinEcho=8; //Pin del echo
#elif defined(ESP8266)
  const byte pinTrig=D3; //Pin del trigger
  const byte pinEcho=D4; //Pin del echo
#endif

void setup() {
  Serial.begin(115200); //Inicializamos el puerto serie
  pinMode(pinTrig,OUTPUT); //Definimos el pin del trigger como salida (generación del pulso de ultrasonidos)
  pinMode(pinEcho,INPUT); //Definimos el pin del echo como entrada (tiempo de rebote del ultrasonido)
}

void loop() {
  digitalWrite(pinTrig,LOW); //Comenzamos desactivando el sensor
  delayMicroseconds(5); //Esperamos un momento
  digitalWrite(pinTrig,HIGH); //Activamos el pulso de ultrasonidos
  delayMicroseconds(10); //Esperamos un instante para que llegue el eco
  digitalWrite(pinTrig,LOW); //Apagamos el trigger. Realmente no es necesario, porque ya lo hacemos al principio.
  //Calculamos la distancia multiplicando la constante por el tiempo transcurrido entre dos pulsos
  //El resultado vendrá en centímetros
  Serial.println(int(0.01715*pulseIn(pinEcho,HIGH)));
  delay(500); //Esperamos medio segundo antes de tomar una nueva medida
}
