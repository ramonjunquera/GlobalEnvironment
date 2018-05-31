/*
  Autor: Ramón Junquera
  Tema: Módulo PCA9685 - 16xPWM
  Objetivo: Ejemplo de gestión de un varios módulos
  Fecha: 20180531
  Material: cualquier placa, 2x PCA9685, 4x leds
  Descripción:
  Es este ejemplo encadenaremos dos módulos PCA9685 y comprobaremos su
  funcionamiento.
  Suponemos que las deirecciones I2C de los módulos son 0x40 y 0x41.
  En cada módulo sólo utilizaremos dos canales: el primero y el octavo.
  
  Resultado:
  Comprobamos que podemos gestionar más de un módulo por el bus I2C

  I2C pinout:

  device  SCL SDA
  ------- --- ---
  Mega    21  20
  Nano    A5  A4
  UNO     A5  A4
  ESP8266 D1  D2
  ESP32   22  21
  RPi 0   28  27 -> ID_SC & ID_SD    
  RPi 1    5   3 -> GPIO3 & GPIO2
*/
 
#include <Arduino.h>
#include "RoJoPCA9685.h"

//Declaración de objetos de gestión de módulos PCA9685
RoJoPCA9685 pwm40;
RoJoPCA9685 pwm41;
//Declaración de variables para la dirección y el nivel
int delta,level;

void setup()
{
  //Inicializamos los objetos en el puerto 1 de I2C controlado por
  //el objeto Wire. Cada módulo tiene su propio identificador I2C.
  pwm40.begin(0x40);
  pwm41.begin(0x41);
}

void loop()
{
  //Recorremos los dos canales activos para el módulo 0x40
  for(byte c=0;c<2;c++)
  {
    //El sentido será ascendente
    delta=1;
    //Comenzamos desde el nivel 1
    level=1;
    //Mientras no volvamos a nivel cero...
    while(level>0)
    {
      //Aplicamos nivel al canal
      pwm40.setPWM(c*8,level);
      //Si el nivel es el último...cambiamos de dirección
      if(level==4095) delta=-delta;
      //Calculamos el siguiente nivel
      level+=delta;
    }
    //Permitimos que se ejecuten las tareas de background
    yield();
  }
  //Recorremos los dos canales activos para el módulo 0x41
  for(byte c=0;c<2;c++)
  {
    //El sentido será ascendente
    delta=1;
    //Comenzamos desde el nivel 1
    level=1;
    //Mientras no volvamos a nivel cero...
    while(level>0)
    {
      //Aplicamos nivel al canal
      pwm41.setPWM(c*8,level);
      //Si el nivel es el último...cambiamos de dirección
      if(level==4095) delta=-delta;
      //Calculamos el siguiente nivel
      level+=delta;
    }
    //Permitimos que se ejecuten las tareas de background
    yield();
  }
}

