/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones en ESP32
  Fecha: 20181108
  Objetivo: Demostración de clase de gestión del timers de ESP32
  Material: placa ESP32
  Descripción:
    Se utiliza el led integrado en placa para demostrar el funcionamiento de la clase
    que gestiona el timer de ESP32.
  Resultado:
    Se crea un timer que hace parpadear el led integrado cada segundo.
    Tras un momento se detiene.
    Se crea un timer que se lance una única vez.
    Se crea un nuevo timer cíclico con un nuevo periodo.
*/

#include <Arduino.h>
#include "RoJoTimerESP32.h" //Gestión de timers de ESP32

//Definición de variables globales
RoJoTimerESP32 myTimer;

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
  //Configuramos el timer 0 con la función de llamada y periodo = 1s = 1000000us
  bool ok=myTimer.set(0,&myInterruptFunction,1000000);
  //Si algo ha salido mal...
  if(!ok)
  {
    //...informamos
    Serial.println("No se ha podido configurar el timer");
    //Hemos terminado
    return;
  }
  else Serial.println("Timer 0 configurado ok a 1s");
  //Iniciamos el timer cíclico
  myTimer.start(false);
  //Dejamos que parpadee durante 5 segundos
  delay(5000);
  //Paramos el timer
  myTimer.stop();
  Serial.println("Timer detenido");
  //Esperamos dos segundos
  delay(2000);
  //Ejecutamos el timer sólo una vez
  myTimer.start(true);
  Serial.println("Timer lanzado una sola vez");
  //Esperamos tres segundos
  delay(3000);
  //Reconfiguramos el timer 0 para que se ejecute cada medio segundo
  ok=myTimer.set(0,&myInterruptFunction,500000);
  //Si algo ha salido mal...
  if(!ok)
  {
    //...informamos
    Serial.println("No se ha podido configurar el timer 0");
    //Hemos terminado
    return;
  } 
  else Serial.println("Timer 0 configurado ok a 0.5s");
  //Lo arrancamos de manera indefinida
  myTimer.start(false);
  Serial.println("Timer lanzado");
}

void loop()
{
  //Nada especial que hacer aquí
}






