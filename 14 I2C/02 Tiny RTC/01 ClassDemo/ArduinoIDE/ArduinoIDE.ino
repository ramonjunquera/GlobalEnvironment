/*
  Autor: Ramón Junquera
  Tema: Módulo de reloj Tiny RTC I2C Real Time
  Objetivo: Demo de librería
  Material: breadboard, cualquier placa con I2C con entorno Arduino, cables, módulo "Tiny RTC I2C"

  Descripción:
  Mostraremos las capacidades de la librería de gestión del RTC.
  Inicialmente comprobamos si el reloj está presente.
  Lo ponemos en hora en función de la hora de compilación.
  Mostramos por el puerto serie la hora completa cada segundo.
  
  Resultado:
  Vemos la hora del reloj en el monitor del puerto serie
*/

#include <Arduino.h>
#include "RoJoRTCclock.h" //Gestión del reloj del módulo Tiny RTC

//Creamos objeto de gestión del reloj
RoJoRTCclock clock;

void setup()
{
  Serial.begin(115200);
  //Si el reloj no está presente...
  if(!clock.check()) Serial.println(F("Reloj no detectado"));
  else Serial.println(F("Reloj detectado"));
  
  //Fijamos la hora del reloj en base al tiempo de compilación.
  //Es el tiempo que tarda desde el momento en que se pulsa por primera vez el botón de compilar y enviar
  //hasta que el programa comienza a funcionar. Depende del ordenador donde se compila y de la placa.
  //Se tendrán que hacer varias pruebas hasta encontrar el valor más adecuado para cada caso
  //Lo que hacemos es solicitar la fecha de compilación a través de la función getCompilerTime
  //Nos devuelve una variable datetime, que es la misma que enviamos para que se ajuste el reloj con la 
  //función set.
  clock.set(clock.getCompilerTime(),19);
}

void loop()
{
  //Creamos una variable para guardar la fecha y hora actuales
  datetime currentTime=clock.get();
  //Si hemos podido leer la hora del módulo conectado a I2C...
  if (currentTime.second < 128)
  {
    //Ya tenemos el tiempo actual guardado en la variable currentTime
    Serial.print(currentTime.day);
    Serial.print(F("/"));
    Serial.print(currentTime.month);
    Serial.print(F("/"));
    Serial.print(currentTime.year);
    Serial.print(F(" "));
    Serial.print(currentTime.hour);
    Serial.print(F(":"));
    Serial.print(currentTime.minute);
    Serial.print(F(":"));
    Serial.print(currentTime.second);
    Serial.print(F(" weekDay="));
    Serial.println(currentTime.weekDay);
  }
  else //No hemos podido leer la hora!
  {
    //Mostramos 4 guiones en la pantalla para indicar que algo no funciona
    Serial.println(F("----"));
  }
  //Esperamos un segundo
  delay(1000);
}
