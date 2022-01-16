/*
  Autor: Ramón Junquera
  Fecha: 20200126
  Tema: M5Stick C. Joystick HAT
  Objetivo: Ejemplo de obtención de datos
  Material: M5Strick C, M5Stick Joystick HAT

  Descripción:
  Leeremos los datos del joystick y los mostraremos por el puerto serie.
  Mostraremos para cada eje, el valor mínimo, el valor actual, y el máximo.
*/

#include <Arduino.h>
#include <Wire.h>
#include <RoJoM5JoyHAT.h> //Gestión del HAT de joystick para M5Stick C

//Definición de objetos globales
RoJoM5JoyHAT joy;
//Definición de variables globales
int8_t xMin=126; //Mínimo valor de x
int8_t yMin=126; //Mínimo valor de y
int8_t xMax=-126; //Máximo valor de x
int8_t yMax=-126; //Máximo valor de y

void setup() {
  Serial.begin(115200); //Activamos el puerto serie
  pinMode(LED_BUILTIN,OUTPUT); //Definimos el pin del led integrado como salida
  digitalWrite(LED_BUILTIN,LOW); //Encendemos el led para indicar que está en marcha
  if(joy.begin()) Serial.println("Joystick inicializado correctamente.");
  else Serial.println("Error al inicializar el joystick.");
}

void loop() {
  int8_t x,y;
  bool button;
  if(joy.get(&x,&y,&button)) { //Si se ha podido leer...
    if(x<xMin) xMin=x;
    if(x>xMax) xMax=x;
    if(y<yMin) yMin=y;
    if(y>yMax) yMax=y;
    Serial.println("x=["+String(xMin)+","+String(x)+","+String(xMax)+"],y=["+String(yMin)+","+String(y)+","+String(yMax)+"],button="+String(button));
  } else Serial.println("no data");
  delay(200);
}
