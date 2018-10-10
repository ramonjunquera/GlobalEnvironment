/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20181010
  Objetivo: Demostración de clase de gestión del timers de Arduino
  Material: placa Arduino Mega

  Descripción:
  Crearemos dos objetos de timers utilizando los timers 4 y 5 que sólo están disponibles en Mega.
  Uno y Nano sólo tienen los timers 1 y 2.
  Uno de ellos tendrá un periodo de 2 segundos y el otro de 1.
  Llamarán a una función que enviará por el puerto serie un mensaje.
  Mantendremos esta situación durante 10 segundos.
  Después detendremos ambos timers durante 3 segundos.
  Por último activaremos de nuevo el timer de un segundo.
  
  Resultado:
  Se muestran los mensajes de las funciones a las que llaman los timers.
*/

#include <Arduino.h>
#include "RoJoTimer.h" //Gestión de timers de Arduino

//Creamos objetos de timer
RoJoTimer timer4,timer5;


void f4()
{
  //Función a la que se llama cuando se produce una interrupción del timer 4

  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f4");
}

void f5()
{
  //Función a la que se llama cuando se produce una interrupción del timer 5

  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f5");
}

void setup()
{
  //Inicialización del puerto serie
  Serial.begin(115200);
  //Configuramos el timer 4 que llamará a la función f4 cada 2 segundos
  bool ok=timer4.set(4,f4,2000000);
  //Si la configuración no ha salido bien...
  if(!ok)
  {
    Serial.println("Timer 4 init error");
    return;
  }
  //Configuramos el timer 5 que llamará a la función f5 cada segundo
  ok=timer5.set(5,f5,1000000);
  //Si la configuración no ha salido bien...
  if(!ok)
  {
    Serial.println("Timer 5 init error");
    return;
  }
  //Arrancamos los timers
  Serial.println("Starting timers");
  timer4.start();
  timer5.start();
  //Esperamos 10 segundos
  delay(10000);
  //Detenemos los timers
  Serial.println("Stopping timers");
  timer4.stop();
  timer5.stop();
  //Esperamos 3 segundos
  delay(3000);
  //Arrancamos de nuevo el timer 5
  Serial.println("Starting timer 5");
  timer5.start();
}

void loop()
{
  //Nada especial que hacer aquí
}






