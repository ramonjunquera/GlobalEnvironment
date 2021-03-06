/*
  Autor: Ramón Junquera
  Fecha: 20191030
  Tema: Modo de bajo consumo
  Objetivo: Uso del bajo consumo con interrupciones software (timers)
  Material: Arduino UNO, breadboard, interruptor de pulsador, cables

  Descripción:
  En ejercicios anteriores hemos aprendido a entrar en modo de bajo consumo y salir de él por un cambio
  de estado en un pin.
  Hay una segunda manera de salir... con un timer.
  El timer no es ninguno de los utilizados en ejercicios anteriores. Esta vez se llama WatchDog.
  Es un timer que se encarga de que el programa no se quede atascado, y si lo hace resetea la placa.
  Se ha creado una librería para facilitar la gestión: RoJoLowPowerWDT.

  El programa pasa a modo de bajo consumo durante un tiempo determinado.
  Cuando despierta hace parpadear el led integrado y vuelve a entrar.

  Para comprobar que las interrupciones hardware también hacen salir del modo de bajo consumo, se incluye
  un interruptor conectado a un pin con interrupción por hardware.
  Cuando lo pulsamos hacemos que se despierte y que parpadee el led.
    
  Resultado:
  La placa permanece en estado de bajo consumo durante un tiempo máximo o hasta que se pulse el interruptor.
*/

#include <Arduino.h>
#include "RoJoLowPowerWDT.h" //Gestión del timer para el modo de bajo consumo

//Declaración de constantes globales
const byte pinLed = 13; //Pin del led integrado en placa
const byte pinSwitch = 2; //Pin del interruptor
const long sleepTime = 3000; //Tiempo de hibernación en milisegundos

void setup() {
  Serial.begin(115200);
  //El pin del interruptor estará en HIGH y cuando se pulse en LOW
  pinMode(pinSwitch,INPUT_PULLUP);
  //El pin del led será de salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
  //Definimos la interrupción 0 correspondiente al pin 2 cuando pasa de HIGH a LOW (pulsación)
  //Se llamará a la función IntSwitch()
  attachInterrupt(0,IntSwitch,FALLING);
}
 
void loop() {
  //Entramos en modo de bajo consumo durante el tiempo definido
  lowPowerWDT.sleep(sleepTime);
    
  //A partir de este punto ya no se ejecuta el programa hasta que despertemos de nuevo
  //Ya sea porque ha finalizado el tiempo del timer o porque se ha desactivador el timer
    
  //Encendemos el led por un momento
  digitalWrite(pinLed,HIGH);
  delay(100);
  digitalWrite(pinLed,LOW);
}

void IntSwitch() {
  //Función a la que se llama cuando cambia el estado del interruptor

  //Despertamos!. Cancelamos el timer. Se continuará la función loop() en el mismo punto en el que se dejó
  lowPowerWDT.wakeup();
}
