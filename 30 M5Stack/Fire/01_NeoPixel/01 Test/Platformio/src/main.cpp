/*
  Autor: Ramón Junquera
  Fecha: 20190703
  Tema: M5Stack. Leds
  Objetivo: Demo de gestión de leds
  Material: M5Stack

  Descripción:
    Sucesión de varios ejemplos
*/

#include <Arduino.h>
#include <RoJoM5Leds.h> //Gestión de leds para M5Stack

RoJoM5Leds leds; //Objeto de gestión de leds en M5Stack

//Sucesión ascendente
void Test1()
{
  Serial.println("Test1 start");
  //Apagamos todos los leds
  leds.clear();
  //Damos 3 vueltas
  for(byte turnCount=0;turnCount<3;turnCount++)
    //Recorremos todos los leds
    for(byte ledIndex=0;ledIndex<10;ledIndex++)
    {
      //Encendemos el led procesado. Sólo canal verde
      leds.ledColor[ledIndex][1]=255;
      //Mostramos la configuración actual
      leds.show();
      //Apagamos el led procesado para que no aparezca en el siguiente ciclo
      leds.ledColor[ledIndex][1]=0;
      //Esperamos un momento
      delay(100);
    }
  Serial.println("Test1 end");
}

//Efecto KITT por ambos lados
void Test2()
{
  char currentLed=0; //Posición del led procesado
  char delta=1; //Dirección

  //Apagamos todos los leds
  leds.clear();
  //Hacemos 100 cambios de led
  for(byte i=0;i<100;i++)
  {
    //Encendemos el led procesado en ambos lados
    leds.ledColor[(byte)currentLed][0]=255; //Derecha
    leds.ledColor[9-currentLed][0]=255; //Izquierda
    //Mostramos la configuración actual
    leds.show();
    //Calculamos la siguiente posición
    currentLed+=delta;
    //Si hemos llegado a un extremo...cambiamos de dirección
    if(currentLed==0 || currentLed==4) delta=-delta;
    //Atenuamos todos los leds rojos
    for(byte ledIndex=0;ledIndex<10;ledIndex++) leds.ledColor[ledIndex][0]/=2;
    //Esperamos un momento
    delay(100);
  }
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Test1(); //Sucesión ascendente
  Test2(); //KITT
}