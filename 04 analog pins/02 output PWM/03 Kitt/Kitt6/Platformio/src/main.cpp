/*
  Autor: Ramón Junquera
  Tema: Elementos analógicos
  Fecha: 20191001
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: Arduno UNO, 6 leds, 6 resistencias de 220 ohmios
  Descripción:
    La placa Arduino UNO sólo tine 6 pines con posibilidad de PWM.
    Los utilizaremos todos.

  Resultado:
    Efecto Kitt con 6 leds.
*/

#include <Arduino.h>

const byte pinLeds[]={3,5,6,9,10,11}; //Array de pines de leds
int16_t levels[6]; //Array de niveles para cada pin
int8_t delta=1; //Dirección
byte channel=0; //Canal actual

//Aplica los niveles de todos los canales y los apaga un poco
void ApplyAndFade() {
  //Recorremos los 6 canales utilizados y aplicamos sus niveles
  for(byte c=0;c<6;c++) analogWrite(pinLeds[c],levels[c]);
  
  for(byte c=0;c<6;c++) { //Recorremos los 6 canales utilizados...
    levels[c]-=5; //Reducimos el nivel del canal en un valor fijo
    if(levels[c]<0) levels[c]=0; //Si hemos reducido demasiado, como mínimo lo dejaremos en 0
  }
}

void setup() {
  //Inicialmente todos los niveles están a cero
  for(byte c=0;c<6;c++) levels[c]=0;
}

void loop() {
  channel+=delta; //Calculamos el valor del nuevo canal
  if((channel==0) || (channel==5)) delta=-delta; //Si hemos llegado a un extremo...cambiamos la dirección
  levels[channel]=0xFF; //Aplicamos el nivel máximo al canal afectado
  //Repetimos un bucle que sólo baja la intensidad se todos los leds en cada ciclo
  for(byte z=0;z<14;z++) {
    ApplyAndFade(); //Aplicamos configuración actual
    delay(10); //Esperamos un momento
  }
}
