/*
  Autor: Ramón Junquera
  Tema: Display LCD 1602A
  Fecha: 20180627
  Objetivo: Demostración de la clase de gestión
  Material: breadboard, cables, display 1602A, resistencia de 220 ohmios, resistencia de 1 Kohmios, placa RPi

  Descripción:
  El display LCD modelo 1602A es capaz de mostrar 2 líneas de 16 caracteres cada una.
  El 1602A normalmente viene sin patas. Es muy aconsejable soldarle un "peine" de patas rectas (pines macho de 2,54mm).
  Esto hace más cómo su manejo, porque puede ser pinchada a la breadboard sin problema.

  Resultado:
  Vemos las capacidades de la librería en funcionamiento.
*/

#include <Arduino.h>
#include "RoJoLCD1602A.cpp" //Librería de gestión del display LCD1602A

//Declaración de variables globales
RoJoLCD1602A lcd; //Objeto de gestión del display

int main(int argc, char **argv)
{
  //Definimos pines e inicializamos
  lcd.begin(7,8,25,24,23,18); //begin(RS,E,D0,D1,D2,D3)
  
  //Posicionamos el cursor en la columna 7 y primera fila
  lcd.pos(6,0);
  //Escribimos el texto a mostrar a partir de la posición del cursor
  lcd.print("Hola");
  //Posicionamos el cursor en la columna 6 y segunda fila
  lcd.pos(5,1);
  //Escribimos el texto a mostrar a partir de la posición del cursor
  lcd.print("mundo!");
  delay(500);
  lcd.enable(false); //Apagamos la pantalla (sin perder su contenido)
  delay(500);
  lcd.enable(true); //Encendemos la pantalla
  delay(500);
  lcd.cursor(true); //Activamos el cursor
  delay(1000);
  lcd.blink(true); //Activamos el parpadeo del carácter que tiene el cursor
  delay(2000);
  lcd.cursor(false); //Desactivamos el cursor
  lcd.blink(false); //Desactivamos el parpadeo

  //Demostración de caracteres definidos por el usuario
  //Definiremos 8 caracteres que guardaremos en las 8 primeras posiciones
  byte heart[8]=
  {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
  };
  lcd.createChar(0,heart);
  byte diamond[8]=
  {
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
  };
  lcd.createChar(1,diamond);
  byte spade[8]=
  {
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b00100,
    0b01110,
    0b00000
  };
  lcd.createChar(2,spade);
  byte club[8]=
  {
    0b00000,
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b00100,
    0b01110,
    0b00000
  };
  lcd.createChar(3,club);
  byte rec[8]=
  {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111
  };
  lcd.createChar(4,rec);
  byte smiley[8]=
  {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b10001,
    0b01110,
    0b00000
  };
  lcd.createChar(5,smiley);
  byte euro[8]=
  {
    0b00110,
    0b01001,
    0b11100,
    0b01000,
    0b11100,
    0b01001,
    0b00110,
    0b00000
  };
  lcd.createChar(6,euro);
  byte square[8]=
  {
    0b00110,
    0b00001,
    0b00010,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcd.createChar(7,square);

  //Borramos la pantalla. Necesario para que las modificaciones las modificaciones de
  //los caracteres personalizados sean visibles.
  lcd.clear();
  //Mostramos el carácter 0. Sólo podemos hacerlo así, puesto que el 0 indica el final de 
  //un String en C
  lcd.printChar(0);
  //Mostramos el resto de caracteres definidos
  lcd.print("\1\2\3\4\5\6\7");
  //Esperamos un momento
  delay(5000);
  //Borramos pantalla
  lcd.clear();
  
  while(1)
  {
	  //Mostraremos el tiempo transcurrido en milisegundos
	  lcd.home();
	  //Nota. No se puede convertir un uint64_t a string directamente
	  //con String(millis())
	  //Sin embargo, en C++ tenemos la función to_string() que nos
	  //ayuda a convertir cualquier valor a string
	  lcd.print(to_string(millis()));
	  delay(310); //No vale la pena esperar menos porque el refresco del LCD es muy bajo
  }
}

