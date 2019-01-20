/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20181010
  Objetivo: Demostración de clase de gestión del timers de Arduino
  Material: placa Arduino UNO/Nano

  Descripción:
  Crearemos dos objetos de timers. Uno por cada uno disponible.
  Uno de ellos tendrá un periodo de 2 segundos y el otro de 1.
  Llamarán a una función que enviará por el puerto serie un mensaje.
  Mantendremos esta situación durante 10 segundos.
  Después detendremos ambos timers durante 3 segundos.
  Por último activaremos de nuevo el timer de un segundo.
  
  Resultado:
  Se muestran los mensajes de las funciones a las que llaman los timers.
*/

#include <Arduino.h>
#include "RoJoTimerAVR.h" //Gestión de timers de Arduino

//Creamos objetos de timer
RoJoTimerAVR timer1,timer2;


void f1()
{
  //Función a la que se llama cuando se produce una interrupción del timer 1

  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f1");
}

void f2()
{
  //Función a la que se llama cuando se produce una interrupción del timer 2

  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f2");
}

void setup()
{
  //Inicialización del puerto serie
  Serial.begin(115200);
  //Configuramos el timer 1 que llamará a la función f1 cada 2 segundos
  bool ok=timer1.set(1,f1,2000000);
  //Si la configuración no ha salido bien...
  if(!ok)
  {
    Serial.println("Timer 1 init error");
    return;
  }
  //Configuramos el timer 2 que llamará a la función f2 cada segundo
  ok=timer2.set(2,f2,1000000);
  //Si la configuración no ha salido bien...
  if(!ok)
  {
    Serial.println("Timer 2 init error");
    return;
  }
  //Arrancamos los timers
  Serial.println("Starting timers");
  timer1.start();
  timer2.start();
  //Esperamos 10 segundos
  delay(10000);
  //Detenemos los timers
  Serial.println("Stopping timers");
  timer1.stop();
  timer2.stop();
  //Esperamos 3 segundos
  delay(3000);
  //Arrancamos de nuevo el timer 2
  Serial.println("Starting timer 2");
  timer2.start();
}

void loop()
{
  //Nada especial que hacer aquí
}






