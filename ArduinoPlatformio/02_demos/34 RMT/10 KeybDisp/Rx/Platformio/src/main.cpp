/*
  Tema: Librería RoJoRMT
  Objetivo: Recibir un mensaje de texto y mostrarlo en un display
  Fecha: 20220512
  Autor: Ramón Junquera

  Descripción:
    Se muestra el mensaje de texto recibido.
    Si hay errores en la recepción en mostrará en negativo
  Notas:
  - La luz ambiental afecta al resultado. Se debe reducir en lo posible.
*/

#include <Arduino.h>
#include <RoJoRMT.h>
#include <RoJoSSD1306.h>

//Constantes globales
const byte pinLed=32;
const byte pinDisplaySDA=255;
const byte pinDisplaySLC=255;
const byte pinDisplayRES=23;
const byte maxLength=12; //Longitud máxima del texto
//Objetos globales
RoJoRMT conn;
RoJoSSD1306 display;
RoJoSprite v(0); //Sprite monocromo que utilizaremos como memoria de vídeo

void setup() {
  Serial.begin(115200);
  //Init comm
  byte errorCode=conn.begin(pinLed,false,255,maxLength); //Inicializamos configuración como Rx
  Serial.printf("Comunicación inicializada con error %u\n",errorCode);
  //Init display
  display.begin(pinDisplaySDA,pinDisplaySLC,pinDisplayRES,true); //Init display 128x32
  display.setContrast(128); //Aumentamos el contraste por defecto
  v.setSize(display.xMax(),display.yMax()); //Dimensionamos la memoria de video
  //Ready
  v.printOver("/10x15.fon","ready",1,40,8);
  display.drawSprite(&v);
}

void loop() {
  byte bufferRx[maxLength];
  uint16_t totalBytes;
  byte errorCode=conn.Rx(bufferRx,&totalBytes);
  if(totalBytes>0) {
    String txt="";
    for(uint16_t i=0;i<totalBytes;i++) txt+=(char)bufferRx[i];
    display.negative(errorCode>0); //Si hay errores...en negativo
    v.clear(); //Borramos la memoria de vídeo
    v.printOver("/10x15.fon",txt,1,0,8); //Escribimos el texto directamente sobre el sprite
    display.drawSprite(&v); //Mostramos la memoria de vídeo
  }
}