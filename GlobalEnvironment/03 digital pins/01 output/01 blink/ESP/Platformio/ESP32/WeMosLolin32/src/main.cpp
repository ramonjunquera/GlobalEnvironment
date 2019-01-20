/*
Autor: Ramón Junquera
Tema: Gestión de pines digitales de salida
Material: WeMos Lolin32
Descripción:
Sabemos por las especificaciones de la placa que el pin 5 está asociado a led
integrado en placa.
De todas maneras, el compilador le ha asignado a la constante global
LED_BUILTIN este mismo valor.

Es muy conveniente incluir siempre en la cabecera de nuestros programas el
include de Arduino.h
No es necesario para compilar, puesto que lo incluye él mismo, pero si lo
hacemos tendremos ayuda contextual en el desarrollo.

Los tiempos de espera de encendido y apagado son distintos para que podamos
distinguir que como todas las placas ESP, el estado LOW es encendido y el
HIGH apagado.

Resultado:
El led integrado parpadea cada segundo
*/

#include "Arduino.h" //Para facilitar la ayuda contextual

void setup()
{
  //Definimos el pin del led integrado como salida
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  //Encendemos el led
  digitalWrite(LED_BUILTIN, LOW);
  //Esperamos un segundo
  delay(1000);
  //Apagamos el led
  digitalWrite(LED_BUILTIN, HIGH);
   //Esperamos 2 décimas de segundo
  delay(200);
}
