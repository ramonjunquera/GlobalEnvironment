/*
  Autor: Ramón Junquera
  Fecha: 20200127
  Tema: M5Stick C. Joystick HAT
  Objetivo: Representación gráfica de posición del Joy
  Material: M5Strick C, M5Stick Joystick HAT

  Descripción:
  Mostraremos un pixel en pantalla con la posición del joystick.
  Al pulsar el botón del joy se encenderá el led integrado.
  Los rangos se ajustan dinámicamente al tamaño del display
*/

#include <Arduino.h>
#include <Wire.h>
#include <RoJoST7735S.h> //Destión de display
#include <RoJoM5JoyHAT.h> //Gestión del HAT de joystick para M5Stick C

//Definición de objetos globales
RoJoM5JoyHAT joy;
RoJoST7735S display;
//Definición de variables globales
int8_t xMin=126; //Mínimo valor de x
int8_t yMin=126; //Mínimo valor de y
int8_t xMax=-126; //Máximo valor de x
int8_t yMax=-126; //Máximo valor de y
byte lastX=0; //Previa coordenada X
byte lastY=0; //Previa coordenada Y

void setup() {
  Serial.begin(115200); //Activamos el puerto serie
  pinMode(LED_BUILTIN,OUTPUT); //Definimos el pin del led integrado como salida
  digitalWrite(LED_BUILTIN,HIGH); //Apagamos el led
  display.begin(); //Inicialización por defecto de display para M5StickC
  if(joy.begin()) Serial.println("Joystick inicializado correctamente.");
  else Serial.println("Error al inicializar el joystick.");
}

void loop() {
  int8_t joyX,joyY;
  bool button;
  if(joy.get(&joyX,&joyY,&button)) { //Si se ha podido leer...
    if(joyX<xMin) xMin=joyX;
    if(joyX>xMax) xMax=joyX;
    if(joyY<yMin) yMin=joyY;
    if(joyY>yMax) yMax=joyY;
    digitalWrite(LED_BUILTIN,button);
    byte x,y;
    if(xMin==xMax) x=40;
    else x=(int)(joyX-xMin)*80/((int)(xMax-xMin));
    if(yMin==yMax) y=80;
    else y=160-(int)(joyY-yMin)*160/((int)(yMax-yMin)); //Eje y está invertido
    if(x!=lastX || y!=lastY) { //Si la coordenada ha cambiado...
      display.drawPixel(lastX,lastY,0); //Borramos la coordenada anterior
      lastX=x; lastY=y;
      display.drawPixel(x,y,0xFFFF); //Dibujamos la nueva
    }
  }
  delay(20);
}
