/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20191006
  Objetivo: Demostración de clase de gestión del timers de Arduino
  Material: Placa Arduino Mega

  Descripción:
    Utilizaremos los 3 timers que sólo tiene la Arduino Mega: 3, 4 y 5.
    Les asignaremos periodos de 1, 1.5 y 2 segundos.
    Llamarán a una función que enviará por el puerto serie un mensaje.
    Mantendremos esta situación durante 10 segundos.
    Después detendremos los timers durante 3 segundos.
    Por último activaremos un timer para que se ejecute una sóla vez, y 
    otro para que lo haga indefinidamente cada medio segundo.
  
  Resultado:
    Se muestran los mensajes de las funciones a las que llaman los timers.
*/

#include <Arduino.h>
#include <RoJoTimerAVR3.h> //Gestión de timer 3 de Arduino
#include <RoJoTimerAVR4.h> //Gestión de timer 4 de Arduino
#include <RoJoTimerAVR5.h> //Gestión de timer 5 de Arduino

//Función a la que se llama cuando se produce una interrupción del timer 3
void f3() {
  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f3");
}

//Función a la que se llama cuando se produce una interrupción del timer 4
void f4() {
  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f4");
}

//Función a la que se llama cuando se produce una interrupción del timer 5
void f5() {
  //Mostramos info de interrupción
  Serial.println(String(millis()) + " : f5");
}

void setup() {
  //Inicialización del puerto serie
  Serial.begin(115200);
  //Arrancamos los timers
  Serial.println("Starting timers");
  timerAVR3.attach(1,f3);
  timerAVR4.attach(1.5,f4);
  timerAVR5.attach(2,f5);
  //Esperamos 10 segundos
  delay(10000);
  //Detenemos los timers
  Serial.println("Stopping timers");
  timerAVR3.detach();
  timerAVR4.detach();
  timerAVR5.detach();
  //Esperamos 3 segundos
  delay(3000);

  //Arrancamos de timer 4 para una sóla ejecución en 3 segundos
  timerAVR4.once(3,f4);
  Serial.println("Starting timer 4");
  //Arrancamos el timer 5 cada medio segundo (500ms)
  Serial.println("Starting timer 5");
  timerAVR5.attach_ms(500,f5);
}

void loop()
{
  //Nada especial que hacer aquí
}






