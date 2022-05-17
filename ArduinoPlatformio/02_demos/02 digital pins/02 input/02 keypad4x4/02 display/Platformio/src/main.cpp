/*
  Tema: Teclado de membrana de 4x4
  Objetivo: Mostrar pulsaciones en display
  Fecha: 20220511
  Autor: Ramón Junquera

  Descripción:
    Para no depender de una conexión serie, utilizamos un display que muestra las teclas que se van pulsando.
    Se configura la tecla * para borrar el último carácter.
    Se limita el número de caracteres a mostrar.

  Notas:
  - Arduino UNO & Nano no tienen suficiente memoria como para gestionar las librerías gráficas
*/

#include <Arduino.h>
#include <RoJoPad4x4.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306

RoJoSSD1306 display;
RoJoSprite v(0); //Sprite monocromo que utilizaremos como memoria de vídeo

#ifdef ESP32 
  RoJoPad4x4 pad(13,12,14,27,26,25,33,32); //LOLIN32
  //Pinout display
  const byte pinSDA=255;
  const byte pinSCL=255;
  const byte pinRES=16;
#else
  RoJoKeypad4x4 pad(13,12,11,10,9,8,7,6); //Mega
  //Pinout display
  const byte pinSDA=255;
  const byte pinSCL=255;
  const byte pinRES=3;
#endif

const byte maxLength=12; //Longitud máxima del texto
String txt=""; //Texto a mostrar

void setup() {
  display.begin(pinSDA,pinSCL,pinRES,true); //Init display 128x32
  display.setContrast(128); //Aumentamos el contraste por defecto
  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo tamaño que el display
  v.setSize(display.xMax(),display.yMax());

  v.printOver("/10x15.fon","ready",1,40,8);
  display.drawSprite(&v);
}

void loop() {
  char c=pad.get(); //Obtenemos el carácter que se ha pulsado
  if(c) { //Si se ha pulsado algo...
    if(c=='*') { //Si se ha pulsado *...
      if(txt.length()>0) { //Si hay algo que borrar...
        txt.remove(txt.length()-1); //Borramos el último carácter
      }
    } else { //Si no es *...
      if(txt.length()<maxLength) { //Si no se ha llegado al máximo...
        txt+=c; //Añadimos el carácter
      }
    }
    v.clear(); //Borramos la memoria de vídeo
    v.printOver("/10x15.fon",txt,1,0,8); //Escribimos el texto directamente sobre el sprite
    display.drawSprite(&v); //Mostramos la memoria de vídeo
  }
}
