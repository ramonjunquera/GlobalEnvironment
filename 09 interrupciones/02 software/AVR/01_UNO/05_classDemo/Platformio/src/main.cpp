/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20191006
  Objetivo: Demostración de clase de gestión del timers de Arduino
  Material: Placa Arduino UNO/Nano

  Descripción:
    Utilizaremos los 2 timers disponibles en UNO
    Uno de ellos tendrá un periodo de 2 segundos y el otro de 1.
    Llamarán a una función que enviará por el puerto serie un mensaje.
    Mantendremos esta situación durante 10 segundos.
    Después detendremos ambos timers durante 3 segundos.
    Por último activaremos un timer para que se ejecute una sóla vez, y el 
    otro para que lo haga indefinidamente cada medio segundo.
  
  Resultado:
    Se muestran los mensajes de las funciones a las que llaman los timers.
*/

#include <Arduino.h>
#include <RoJoTimerAVR1.h> //Gestión de timer 1 de Arduino
#include <RoJoTimerAVR2.h> //Gestión de timer 2 de Arduino

//Función a la que se llama cuando se produce una interrupción del timer 1
void f1() {
  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f1");
}

//Función a la que se llama cuando se produce una interrupción del timer 2
void f2() {
  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f2");
}

void setup() {
  //Inicialización del puerto serie
  Serial.begin(115200);
  //Arrancamos los timers
  Serial.println("Starting timers");
  timerAVR1.attach(1,f1); //Llamará a la función f1 cada segundo
  timerAVR2.attach(2,f2); //Llamará a la función f2 cada 2 segundos
  //Esperamos 10 segundos
  delay(10000);
  //Detenemos los timers
  Serial.println("Stopping timers");
  timerAVR1.detach();
  timerAVR2.detach();
  //Esperamos 3 segundos
  delay(3000);

  //Arrancamos de timer 1 para una sóla ejecución en 3 segundos
  timerAVR1.once(3,f1);
  Serial.println("Starting timer 1");
  //Arrancamos de timer 2 cada medio segundo (500ms)
  timerAVR2.attach_ms(500,f2);
  Serial.println("Starting timer 2");
}

void loop() {
  //Nada especial que hacer aquí
}






