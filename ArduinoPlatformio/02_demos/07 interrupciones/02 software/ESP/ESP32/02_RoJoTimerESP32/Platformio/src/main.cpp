/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones en ESP32
  Fecha: 20221026
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
#include <RoJoTimerESP32.h> //Gestión de timers de ESP32

//Definición de variables globales
RoJoTimerESP32 myTimer(0); //Creamos un objeto que gestionará el timer 0
#ifdef ARDUINO_LOLIN32
  byte pinLed=5;
#else
  byte pinLed=LED_BUILTIN;
#endif

//Función a la que llamará el timer
void myInterruptFunction() {
  //Simplemente cambia el estado del led integrado
  digitalWrite(pinLed,!digitalRead(pinLed));
}

void setup() {
  Serial.begin(115200); delay(4000); //Inicialización del puerto serie
  pinMode(pinLed,OUTPUT); //Configuramos el pin del led integrado como salida
  //Lanzamos el timer con un periodo de un segundo y con la función de llamada
  myTimer.attach(1,myInterruptFunction);
  Serial.println("Timer. Periodo: 1 s. Indefinido");
  delay(5000); //Dejamos que parpadee durante 5 segundos
  Serial.println("Timer activo? "+String(myTimer.active()));
  myTimer.detach(); //Paramos el timer
  Serial.println("Timer detenido");
  Serial.println("Timer activo? "+String(myTimer.active()));
  delay(2000); //Esperamos dos segundos
  //Ejecutamos el timer sólo una vez dentro de un segundo
  myTimer.once(1,myInterruptFunction);
  Serial.println("Timer. Periodo: 1 s. Sólo una vez");
  Serial.println("Timer activo? "+String(myTimer.active()));
  delay(2000); //Esperamos 2 segundos
  Serial.println("Timer activo? "+String(myTimer.active()));
  delay(2000); //Esperamos 2 segundos
  //Ejecutamos el timer indefinidamente cada medio segundo (500ms)
  myTimer.attach_ms(500,myInterruptFunction);
  Serial.println("Timer. Periodo: 0.5 s. Indefinido");
}

void loop() {
  //Nada especial que hacer aquí
}
