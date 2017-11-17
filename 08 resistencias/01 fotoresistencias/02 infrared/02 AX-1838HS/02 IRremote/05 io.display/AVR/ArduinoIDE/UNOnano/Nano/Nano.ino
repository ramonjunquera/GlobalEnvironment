//Desarrollo para el receptor (Arduino Nano)

#include <Arduino.h>
#include <IRremote.h>
#include <LiquidCrystal.h>

//Definición de pines
const byte pinIRrec=3;
const byte pinLed=2;

//Creación de objetos globales
IRrecv ir(pinIRrec);
LiquidCrystal lcd(7,8,9,10,11,12);

//Definición de variables globales
byte x=0;
byte y=0;

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Activamos el receptor de infrarrojos
  ir.enableIRIn();
  //El pin del led será de salida
  pinMode(pinLed,OUTPUT);
  //Definimos el tamaño de la pantalla
  lcd.begin(16,2);
  //Limpiamos la pantalla
  lcd.clear();
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
    char c = char(results.value);
    //Lo mostramos en el display
    lcd.write(c);
    //Lo enviamos por el puerto serie (para debug)
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
    lcd.setCursor(x,y);
    //Apagamos el led
    digitalWrite(pinLed,LOW);
    //Indicamos que estamos preparados para recibir el siguiente código
    ir.resume();
  }
}
