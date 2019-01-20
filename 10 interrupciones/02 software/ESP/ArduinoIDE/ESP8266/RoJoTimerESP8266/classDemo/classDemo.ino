/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones en ESP8266
  Fecha: 20181022
  Objetivo: Demostración de clase de gestión del timers de ESP8266
  Material: placa ESP8266
  Descripción:
    Se utiliza el led integrado en placa para demostrar el funcionamiento de la clase
    que gestiona el timer de ESP8266.
  Resultado:
    Se crea un timer que hace parpadear el led integrado cada segundo.
    Tras un momento se detiene.
    Se crea un timer que se lance una única vez.
    Se crea un nuevo timer cíclico con un nuevo periodo.
*/

#include <Arduino.h>
#include "RoJoTimerESP8266.h" //Gestión de timers de ESP8266

void myInterruptFunction()
{
  //Función a la que llamará el timer

  //Simplemente cambia el estado del led integrado
  digitalWrite(LED_BUILTIN,!(digitalRead(LED_BUILTIN)));
}

void setup()
{
  //Inicialización del puerto serie
  Serial.begin(115200);
  //Configuramos el pin del led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Configuramos el timer con la función de llamada y periodo = 1s = 1000000us
  bool ok=RoJoTimerESP8266::set(&myInterruptFunction,1000000);
  //Si algo ha salido mal...
  if(!ok)
  {
    //...informamos
    Serial.println("No se ha podido configurar el timer");
    //Hemos terminado
    return;
  } 
  //Iniciamos el timer cíclico
  RoJoTimerESP8266::start(false);
  //Dejamos que parpadee durante 5 segundos
  delay(5000);
  //Paramos el timer
  RoJoTimerESP8266::stop();
  //Esperamos dos segundos
  delay(2000);
  //Ejecutamos el timer sólo una vez
  RoJoTimerESP8266::start(true);
  //Esperamos tres segundos
  delay(3000);
  //Reconfiguramos el timer para que se ejecute cada medio segundo
  ok=RoJoTimerESP8266::set(&myInterruptFunction,500000);
  //Si algo ha salido mal...
  if(!ok)
  {
    //...informamos
    Serial.println("No se ha podido configurar el timer");
    //Hemos terminado
    return;
  } 
  //Lo arrancamos de manera indefinida
  RoJoTimerESP8266::start(false);
}

void loop()
{
  //Nada especial que hacer aquí
}






