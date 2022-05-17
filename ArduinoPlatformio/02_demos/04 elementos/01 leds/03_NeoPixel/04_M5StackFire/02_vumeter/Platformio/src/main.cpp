/*
  Autor: Ramón Junquera
  Fecha: 20220517
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
  //Configuración NeoPixel para M5Stack Fire. 10 leds en pin 15
  leds.begin(10,15);
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
  
  for(byte i=0;i<10;i++) { //Recorremos todos los leds
    if (power>threshold) leds.videoMem[i].channel[1]=100; //Rojo oscuro
    else leds.videoMem[i].channel[1]=0; //Rojo apagado
    threshold*=3;
  }
  leds.show();
  delay(10);
}