/*
  Autor: Ramón Junquera
  Tema: Sensor de distancia HC-SR04
  Fecha: 20190929
  Objetivo: Utilizar conceptos aprendidos para hacer proyectos más complejos
  Material adicional: breadboard, cables, sensor de distancia HC-SR04, display LCD1602A, 1 resistencia de 220 ohmios, 1 resistencia de 1 Kohmios

  Descripción:
    Haremos un medidor de distancia por ultrasonidos autónomo.
    La medida se mostrará en tiempo real en un displau LCD.
  
  Resultado:
    Visualizar la distancia medida por el sensor en el LCD.
*/

#include <Arduino.h>
#include <RoJoLCD1602A.h>

//Declaración de variables globales
RoJoLCD1602A lcd; //Objeto de gestión del display
const byte pinTrig=6; //Pin del trigger
const byte pinEcho=5; //Pin del echo
long distancia; //Variable para guardar la distancia calculada

void setup() {
  lcd.begin(7,8,9,10,11,12); //Inicialización de display
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
  distancia = 0.01715*pulseIn(pinEcho,HIGH);
  lcd.home(); //Posicionamos el cursor en origen
  lcd.print(String(distancia)+" cm      "); //Escribimos la distancia
  delay(500); //Esperamos medio segundo antes de tomar una nueva medida
}

