/*
  Autor: Ramón Junquera
  Fecha: 20191001
  Tema: Elementos analógicos
  Objetivo: Aprender cómo funcionan las escrituras analógicas
  Material: placa ESP32, 8 leds

  Nota:
    Para este ejemplo se puede utilizar tanto la librería RoJoAnalogWriteESP32L.h
    como la RoJoAnalogWriteESP32S.h

  Resultado:
    Efecto Kitt con 8 leds.
*/

#include <Arduino.h>
//Elegir una de las dos librerías
#include <RoJoAnalogWriteESP32L.h>
//#include <RoJoAnalogWriteESP32S.h>

//Array de pines de leds
const byte pinLeds[]={23,22,21,19,18,5,4,2};
//Array de niveles para cada pin
int16_t levels[8];

int8_t delta=1; //Dirección
byte channel=0; //Canal actual

//Aplica los niveles de todos los canales y los apaga un poco
void ApplyAndFade() {
  //Recorremos los 8 canales utilizados y aplicamos sus niveles
  for(byte c=0;c<8;c++) analogWrite(pinLeds[c],levels[c]);
  //Recorremos los 8 canales utilizados...
  for(byte c=0;c<8;c++) {
    //Reducimos el nivel del canal en un valor fijo
    levels[c]-=5;
    //Si hemos reducido demasiado, como mínimo lo dejaremos en 0
    if(levels[c]<0) levels[c]=0;
  }
}

void setup() {
  //Inicialmente todos los niveles están a cero
  for(byte c=0;c<8;c++) levels[c]=0;
}

void loop() {
  //Calculamos el valor del nuevo canal
  channel+=delta;
  //Si hemos llegado a un extremo...cambiamos la dirección
  if((channel==0) || (channel==7)) delta=-delta;
  //Aplicamos el nivel máximo al canal afectado
  levels[channel]=0xFF;
  //Repetimos un bucle que sólo baja la intensidad se todos los leds en cada ciclo
  for(byte z=0;z<8;z++) {
    //Aplicamos configuración actual
    ApplyAndFade();
    //Esperamos un momento
    delay(10);
  }
}
