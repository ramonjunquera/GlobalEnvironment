//Desarrollo para el receptor (Arduino Nano)

#include <Arduino.h>
#include <IRremote.h>

//Definición de pines
const byte pinIRrec=3;
const byte pinLed=2;

//Creación de objetos globales
IRrecv ir(pinIRrec);

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Activamos el receptor de infrarrojos
  ir.enableIRIn();
  //El pin del led será de salida
  pinMode(pinLed,OUTPUT);
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
    //..enviamos el código recibido por el puerto serie en hexadecimal y binario
    Serial.print(results.value,HEX);
    Serial.print(" - ");
    Serial.println(results.value,BIN);
    //Apagamos el led
    digitalWrite(pinLed,LOW);
    //Esperamos un momento para no enviar códigos contínuamente
    delay(500);
    //Indicamos que estamos preparados para recibir el siguiente código
    ir.resume();
  }
}
