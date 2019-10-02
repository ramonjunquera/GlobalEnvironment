/*
  Autor: Ramón Junquera
  Tema: Gestión de timers en ESP32
  Fecha: 20181108
  Objetivo: Ejemplo básico
  Material: placa ESP32
  Descripción:
    Ejemplo básico de configuración y uso de un timer en ESP32.
    Utilizamos el primero de los 4 timers disponibles.
    Aplicamos un prescaler de 80 para que la frecuencia de actualización del contador del timer
    tenga un periodo de 1 microsegundo.
    Utilizamos una alarma para que salte cuando el contador sea un millón.
    En este caso cada millón de microsegundos = cada segundo.
    La función del timer simplemente envía el valor de millis por el puerto serie.
    Este no es el mejor ejemplo de calidad de código, puesto que una interrupción debe durar
    el mínimo tiempo posible y el envío por el puerto serie no es algo rápido.
    Aun así el código es muy sencillo y cumple con el objetivo didáctico.
    Para mejorar la calidad del código de debería tener un flag que la función de timer active y
    en la función loop hacer pooling al flag para detectar cuándo se ha activado y tomar las 
    acciones necesarias entonces.
  Resultado:
    Se envía un mensaje por el puerto serie cada segundo.
*/

#include <Arduino.h>

//Definición de variables globales
hw_timer_t *myTimer; //Puntero a timer
 
void myFunction()
{
  //Función de llamada del timer
  //No puede tener parámetros ni devolver valores

  //Simplemente enviamos el tiempo transcurrido desde el último arranque en milisegundos por el puerto serie
  Serial.println(millis());
}
 
void setup()
{
  //Inicializamos el puerto serie
  Serial.begin(115200);
  //Creamos un timer nuevo:
  //  hw_timer_t * timerBegin(uint8_t timer, uint16_t divider, bool countUp);
  //Usaremos el timer 0, con un prescaler de 80 y un contador ascendente
  //Con un prescaler de 80 tenemos: frecuencia de actualización = frecuencia base/prescaler=80MHz/80=1MHz
  //El contador se actualiza un millon de veces por segundo = cada un microsegundo
  myTimer=timerBegin(0,80,true);
  //Asignamos función de interrupción
  //  void timerAttachInterrupt(hw_timer_t *timer, void (*fn)(void), bool edge);
  //Asignamos la función definida anteriormente con un tipo de interrupción edge
  timerAttachInterrupt(myTimer,&myFunction,true);
  //Asignamos alarma al timer
  //  void timerAlarmWrite(hw_timer_t *timer, uint64_t interruptAt, bool autoreload);
  //Indicamos que debe llamarse a la función del timer después de un millón de actualizaciones
  //y que cuando ocurra se reseteará el contador automáticamente (repetitiva)
  timerAlarmWrite(myTimer,1000000,true);
  //Activamos el timer
  //  bool timerAlarmEnabled(hw_timer_t *timer);
  timerAlarmEnable(myTimer);
}
 
void loop()
{
  //Nada especial que hacer aquí
}
