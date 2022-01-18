/*
  Tema: Display LCD 1602A con conexión I2C
  Fecha: 20220118
  Autor: Ramón Junquera

  Descripción:
    El display LCD modelo 1602A es capaz de mostrar 2 líneas de 16 caracteres cada una.
    El 1602A normalmente viene sin patas. Es muy aconsejable soldarle un "peine" de patas
    rectas (pines macho de 2,54mm).
    Esto hace más cómodo su manejo, porque puede ser pinchada a la breadboard sin problema.
    Para reducir el número de pines utilizados, se utilzia un adaptador I2C.
    El adaptador es realmente un expansor de pines.
    El ejemplo es idéntico al de conexión directa excepto por la librería utilizada y
    la llamada al método begin, que en este caso no nocesita parámetros.

  Resultado:
    Vemos las capacidades de la librería en funcionamiento.
*/

#include <Arduino.h>
#include <RoJoLCD1602AI2C.h>

//Declaración de variables globales
RoJoLCD1602AI2C lcd; //Objeto de gestión del display

void setup() {
  lcd.begin(); //Inicializamos
  
  lcd.pos(6,0); //Posicionamos el cursor en la columna 7 y primera fila
  lcd.print("Hola"); //Escribimos el texto a mostrar a partir de la posición del cursor
  lcd.pos(5,1); //Posicionamos el cursor en la columna 6 y segunda fila
  lcd.print("mundo!"); //Escribimos el texto a mostrar a partir de la posición del cursor
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
  byte heart[8]= {
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
  byte diamond[8]= {
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
  byte spade[8]= {
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
  byte club[8]= {
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
  byte rec[8]= {
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
  byte smiley[8]= {
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
  byte euro[8]= {
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
  byte square[8]= {
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

  //Borramos la pantalla. Necesario para que las modificaciones de los caracteres
  //personalizados sean visibles.
  lcd.clear();
  //Mostramos el carácter 0. Sólo podemos hacerlo así, puesto que el 0 indica el final de 
  //un String en C
  lcd.printChar(0);
  lcd.print("\1\2\3\4\5\6\7"); //Mostramos el resto de caracteres definidos
  delay(2000);
  lcd.backlight(LOW); //Luz de fondo apagada
  delay(2000);
  lcd.backlight(HIGH); //Luz de fondo encendida
  delay(1000);
  lcd.clear(); //Borramos pantalla
}

void loop() {
  //Mostraremos el tiempo transcurrido en milisegundos
  lcd.home();
  lcd.print(String(millis()));
  delay(310); //No vale la pena esperar menos porque el refresco del LCD es muy bajo
}
