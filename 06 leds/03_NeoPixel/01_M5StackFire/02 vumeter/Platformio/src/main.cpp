/*
  Autor: Ramón Junquera
  Fecha: 20200216
  Tema: M5Stack Fire. Leds NeoPixel
  Objetivo: VU meter
  Material: M5Stack Fire

  Descripción:
    Los leds de NeoPixel se comprotarán como un VU meter, mostrando el volumen recogido
    por el micrófono en tiempo real
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h> //Gestión de leds NeoPixel

RoJoNeoPixel leds; //Objeto de gestión de leds NeoPixel
const byte pinMicro=34; //Pin del micrófono
const uint16_t maxSamples=1000; //Número de muestras
uint16_t micValue[maxSamples]; //Array con las muestras del micro

void setup() {
  Serial.begin(115200);
  //Configuración NoePixel para M5Stack Fire. 10x1 en pin 15
  //Sólo tenemos una fila y 10 columnas
  //Las 5 primeras columnas son del lado derecho comenzando desde arriba
  //Las 5 últimas del lado izquierdo comenzando desde abajo
  leds.begin(10,1,15);
}

void loop() {
  uint32_t meanValue=0; //Valor medio
  uint32_t power=0; //Media de cuadrados de diferencias con valor medio

  //Recogemos las muestras del micrófono
  for(uint32_t n=0;n<maxSamples;n++) {
    uint16_t value=analogRead(pinMicro); //Leemos muestra
    micValue[n]=value; //La guardamos en el array de muestras
    meanValue+=value; //La añadimos al sumatorio de valores de muestras
    delayMicroseconds(20); //Esperamos un momento
  }
  meanValue/=maxSamples; //Calculamos el valor medio

  for(uint32_t n=0;n<maxSamples;n++) { //Recorremos el array de muestras
    //Calculamos el sumatorio de los cuadrados de las diferencias entre
    //el valor de la muestra y el valor medio
    power+=(micValue[n]-meanValue)*(micValue[n]-meanValue);
  }
  power/=maxSamples; //Dividimos el sumatorio por el número de muestras para obtener la media

  Serial.println(power);
  uint32_t threshold=1000;
  
  leds.v->clear(); //Apagamos todos los leds
  for(byte n=0;n<10;n++) { //Recorremos todos los leds
    if (power>threshold) leds.v->drawPixel(n,0,{100,0,0}); //Rojo oscuro
    threshold*=3;
  }
  leds.draw();
  delay(10);
}