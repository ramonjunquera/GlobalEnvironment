#include <Arduino.h>

//Variables globales
volatile bool inInterruption=false; //Se está procesando actualmente una interrupción?
volatile unsigned long now=0; //Millis actuales
volatile unsigned long lastChange=0; //Millis de la última señal recibida
unsigned long minPulse=200; //Duración mínima de pulso en misisegundos

void setup()
{
  //Configuramos el pin 13 como salida
  pinMode(13, OUTPUT);
  //Y lo apagamos
  digitalWrite(13, LOW);
  //Activamos la interrupción 0 correspondiente al pin 2, para que ejecute la función IntSwitch cuando
  //detecte el paso de estado HIGH a LOW
  attachInterrupt(0,IntSwitch,FALLING);
}

void loop()
{
  //No tenemos nada especial que mostrar
}

void IntSwitch()
{
  //Si no estamos procesando una interrupción...
  if(!inInterruption)
  {
    //...ahora sí la estamos procesando
    inInterruption=true;
    //Tomamos nota del momento actual en microsegundos
    now=millis();
    //Si ha transcurrido el tiempo mínimo de pulso...
    if(lastChange+minPulse<now)
    {
      //...la señal recibida es válida
      //cambiamos el estado del pin 13
      digitalWrite(13,!digitalRead(13));
      //Anotamos la señal actual como la última
      lastChange=now;
    }
    //Hemos terminado de procesar la interrupción
    inInterruption=false;
  }
}
