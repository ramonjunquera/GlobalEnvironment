/*
  Tema: Librería RoJoRMT
  Objetivo: Enviar un mensaje de texto desde un teclado
  Fecha: 20220512
  Autor: Ramón Junquera

Descripción:
- El mensaje se escribe con un teclado de membrana de 4x4.
- El mensaje tiene un límite máximo de caracteres definido por una constante.
- La tecla * permite borrar el último carácter.
- EL botón # permite enviar el mensaje.
- El texto actual se muestra en un display.
*/

#include <Arduino.h>
#include <RoJoRMT.h>
#include <RoJoPad4x4.h>
#include <RoJoSSD1306.h>

//Constantes globales
const byte pinLed=19;
const byte pinDisplaySDA=255;
const byte pinDisplaySLC=255;
const byte pinDisplayRES=23;
const byte maxLength=12; //Longitud máxima del texto
//Objetos globales
RoJoRMT conn;
RoJoPad4x4 pad(13,12,14,27,26,25,33,32);
RoJoSSD1306 display;
RoJoSprite v(0); //Sprite monocromo que utilizaremos como memoria de vídeo
//Variables globales
String txt=""; //Mensaje a enviar

void setup() {
  Serial.begin(115200);
  //Init comm
  byte errorCode=conn.begin(pinLed,true,255,maxLength); //Inicializamos configuración como Tx
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
  char c=pad.get(); //Obtenemos el carácter que se ha pulsado
  if(c) { //Si se ha pulsado algo...
    if(c=='#') { //Si se quiere enviar...
      if(txt.length()>0) conn.Tx((byte*)&txt,txt.length()); //Si hay algo que enviar...lo enviamos
    } else if(c=='*') { //Si se quiere borrar el último carácter...
      if(txt.length()>0) txt.remove(txt.length()-1); //Si hay algo que borrar..quitamos el último carácter
    } else { //Se ha pulsado un carácter visible...
      if(txt.length()<maxLength) txt+=c; //Si se puede...lo añadimos
    }
    v.clear(); //Borramos la memoria de vídeo
    v.printOver("/10x15.fon",txt,1,0,8); //Escribimos el texto directamente sobre el sprite
    display.drawSprite(&v); //Mostramos la memoria de vídeo
  }
}