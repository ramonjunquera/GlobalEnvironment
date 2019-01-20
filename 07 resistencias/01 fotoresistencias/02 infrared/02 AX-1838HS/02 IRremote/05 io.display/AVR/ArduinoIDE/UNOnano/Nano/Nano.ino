//Desarrollo para el receptor (Arduino Nano)

#include <Arduino.h>
#include <IRremote.h>
#include "RoJoLCD1602A.h"

//Definición de pines
const byte pinIRrec=3;
const byte pinLed=2;

//Creación de objetos globales
IRrecv ir(pinIRrec);
RoJoLCD1602A lcd;

//Definición de variables globales
byte x=0;
byte y=0;

void setup()
{
  //Activamos el puerto serie para mensajes de debug
  Serial.begin(115200);
  //Activamos el receptor de infrarrojos
  ir.enableIRIn();
  //El pin del led será de salida
  pinMode(pinLed,OUTPUT);
  //Inicializamos el display
  lcd.begin(7,8,9,10,11,12); //begin(RS,E,D0,D1,D2,D3)
}

void loop()
{
  //Creamos objeto para recibir los resultados del receptor infrarrojo
  decode_results results;
  //Si se ha recibido algo por el receptor infrarrojo...
  if(ir.decode(&results))
  {
    //Encendemos el led
    digitalWrite(pinLed,HIGH);
    //Tomamos nota del carácter recibido
    char c = (char)results.value;
    //Lo mostramos en el display
    lcd.printChar(c);
    //Lo enviamos por el puerto serie (para debug)
    Serial.print(c,HEX);
    Serial.print(" - ");
    Serial.println(c);
    //El siguiente carácter se escribirá en la siguiente posición
    x++;
    //Si ya hemos escrito 16 caracteres..
    if(x==16)
    {
      //Volvemos al principio de la línea
      x=0;
      //Y pasamos a la siguiente línea
      y++;
    }
    //Si ya hemos escrito las 2 líneas...
    if(y==2)
    {
      //Volvemos al principio
      y=0;
    }
    //Si el carácter recibido es * ...
    if(c=='*')
    {
      //...borramos el display
      lcd.clear();
      //Y posicionamos el cursor al inicio
      x=0;
      y=0;
    }
    //Fijamos la posición del cursor
    lcd.pos(x,y);
    //Apagamos el led
    digitalWrite(pinLed,LOW);
    //Indicamos que estamos preparados para recibir el siguiente código
    ir.resume();
  }
}
