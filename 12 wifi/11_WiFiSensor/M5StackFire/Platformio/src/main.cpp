/*
  Autor: Ramón Junquera
  Fecha: 20190929
  Tema: WiFi
  Objetivo: Detector de movimiento
  Material: M5Stack Fire

  Descripción:
    Prueba de concepto de detector de movimiento por variación de intensidad de puntos
    de acceso wifi accesibles.
    Aprovechamos las tiras de led que incluye el M5Stack Fire para representar por colores el
    estado actual.
    Inicialmente la luz es blanca durante unos segundos que permiten posicionar el
    dispositivo y alejarnos para que se autoconfigure.
    Al comenzar la autoconfiguración veremos que se intercambian los colores rojo y amarillo.
    El rojo significa que hay cambios importantes en la configuración y el amarillo que
    no hay o no son importante.
    Comienza en rojo y progresivamente irá pasando a amarillo.
    Al finalizar la autoconfiguración, dará un pitido agudo y la luz pasará a verde.
    En verde es que no se detecta movimiento (o es muy leve).
    Al detectar movimiento la luz pasa a azul y da un pitido grave.
*/

#include <Arduino.h>
#include <RoJoM5Leds.h> //Librería de gestión de tiras de led en M5Stack Fire
#include <RoJoWiFiSensor.h> //Librería de detección de movimiento por wifi

//Declaración de objetos globales
RoJoWiFiSensor sensor;
RoJoM5Leds leds;

//Luces en rojo
void lightRed() {
  leds.clear(255,0,0);
  leds.show();
}

//Luces en amarillo
void lightYellow() {
  leds.clear(255,255,0);
  leds.show();
}

void setup() {
  Serial.begin(115200);

  //Leds en blanco y esperamos a que se deje el entorno libre para tomar medidas
  leds.clear(255,255,255);
  leds.show();
  delay(5000);

  if(!sensor.begin(lightRed,lightYellow)) { //Si no consigo inicializar...
    leds.clear(); //Apagamos luces
    leds.show(); //Mostramos
    while(true); //Bucle infinito
  }
  leds.clear(0,255,0); //Ponemos la luz en verde para indicar que hemos terminado
  leds.show();
  //Pitido agudo
  //Buzzer ON
  ledcAttachPin(25,0);
  ledcWriteTone(0,1000);
  delay(100);
  //Buzzer OFF
  ledcDetachPin(25);
  ledcWriteTone(0,0);
}

void loop() {
  if(sensor.check()) { //Si se detecta movimiento...
    leds.clear(0,0,255); //Luces azules
    //Buzzer ON
    ledcAttachPin(25,0);
    ledcWriteTone(0,200);
    delay(100);
    //Buzzer OFF
    ledcDetachPin(25);
    ledcWriteTone(0,0);
  } 
  else { //Si no se detecta movimiento...
    leds.clear(0,255,0); //Luces verdes
  }
  leds.show(); //Mostrar configuración de luces
}
