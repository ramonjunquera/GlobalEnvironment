/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: Arduni UNO, 6 leds, 6 resistencias de 220 ohmios
  Descripción:
  La placa Arduino UNO sólo tine 6 pines con posibilidad de PWM.
  Los utilizaremos todos.

  Resultado:
  Efecto Kitt con 6 leds.
*/

#include <Arduino.h>

//Array de pines de leds
const byte pinLeds[]={3,5,6,9,10,11};
//Array de niveles para cada pin
int16_t levels[6];

int8_t delta=1; //Dirección
byte channel=0; //Canal actual

void ApplyAndFade()
{
  //Aplica los niveles de todos los canales y los apaga un poco
  
  //Recorremos los 6 canales utilizados y aplicamos sus niveles
  for(byte c=0;c<6;c++) analogWrite(pinLeds[c],levels[c]);
  //Recorremos los 6 canales utilizados...
  for(byte c=0;c<6;c++)
  {
    //Reducimos el nivel del canal en un valor fijo
    levels[c]-=5;
    //Si hemos reducido demasiado, como mínimo lo dejaremos en 0
    if(levels[c]<0) levels[c]=0;
  }
}

void setup()
{
  //Inicialmente todos los niveles están a cero
  for(byte c=0;c<6;c++) levels[c]=0;
}

void loop()
{
  //Calculamos el valor del nuevo canal
  channel+=delta;
  //Si hemos llegado a un extremo...cambiamos la dirección
  if((channel==0) || (channel==5)) delta=-delta;
  //Aplicamos el nivel máximo al canal afectado
  levels[channel]=0xFF;
  //Repetimos un bucle que sólo baja la intensidad se todos los leds en cada ciclo
  for(byte z=0;z<14;z++)
  {
    //Aplicamos configuración actual
    ApplyAndFade();
    //Esperamos un momento
    delay(10);
  }
}
