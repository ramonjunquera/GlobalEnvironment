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
#include <RoJoTimerESP8266.h> //Gestión de timers de ESP8266

RoJoTimerESP8266 myTimer; //Objeto de gestión del timer

//Función a la que llamará el timer
void myInterruptFunction() {
  //Simplemente cambia el estado del led integrado
  digitalWrite(LED_BUILTIN,!(digitalRead(LED_BUILTIN)));
}

void setup() {
  //Inicialización del puerto serie
  Serial.begin(115200);
  Serial.println();
  //Configuramos el pin del led integrado como salida
  pinMode(LED_BUILTIN,OUTPUT);
  //Lanzamos el timer con la función de llamada y periodo = 1s = 1000000us
  myTimer.attach(1,myInterruptFunction);
  Serial.println("Timer iniciado a 1s");
  Serial.println("Está activo? "+String(myTimer.active()));
  //Dejamos que parpadee durante 5 segundos
  delay(5000);
  //Paramos el timer
  myTimer.detach();
  Serial.println("Timer detenido");
  Serial.println("Está activo? "+String(myTimer.active()));
  //Esperamos dos segundos
  delay(2000);
  //Lanzamos el timer sólo una vez
  myTimer.once(1,myInterruptFunction);
  Serial.println("Timer lanzado una sola vez dentro de 1 segundo");
  Serial.println("Está activo? "+String(myTimer.active()));
  //Esperamos dos segundos
  delay(2000);
  Serial.println("Está activo? "+String(myTimer.active()));
  delay(2000);
  //Lo lanzamos idefinidamente cada medio segundo (500ms)
  myTimer.attach_ms(500,myInterruptFunction);
  Serial.println("Timer iniciado a 0.5s indefinidamente");
  Serial.println("Está activo? "+String(myTimer.active()));
}

void loop() {
  delay(1000); //Nada especial que hacer aquí
}
