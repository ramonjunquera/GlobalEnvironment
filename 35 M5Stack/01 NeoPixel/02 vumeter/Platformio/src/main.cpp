/*
  microphone neopixel VU meter

  Neopixel LEDS flash dependent on the microphone amplitude

   hardwware:  M5StackFire

  please install the Adafruit library first!

  September 2018, ChrisMicro, MIT License

*/

#include <Arduino.h>
#include <RoJoM5Leds.h> //Gestión de leds para M5Stack

RoJoM5Leds leds; //Objeto de gestión de leds en M5Stack
const byte pinMicro=34; //Pin del micrófono
const uint16_t NUMBEROFSAMPLES=1000; //Número de muestras
uint16_t micValue[NUMBEROFSAMPLES]; //Array con las muestras del micro

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  uint32_t power = 0;
  uint32_t meanValue=0; //Valor medio

  //Recogemos las muestras del microfóno
  for(uint32_t n=0;n<NUMBEROFSAMPLES;n++)
  {
    uint16_t value=analogRead(pinMicro); //Leemos muestra
    micValue[n]=value; //La guardamos en el array de muestras
    meanValue+=value; //La añadimos al sumatorio de valores de muestras
    delayMicroseconds(20); //Esperamos un momento
  }
  meanValue/=NUMBEROFSAMPLES; //Calculamos el valor medio

  //Recorremos el array de muestras
  for(uint32_t n=0;n<NUMBEROFSAMPLES;n++)
  {
    //Calculamos el sumatorio de los cuadrados de las diferencias entre
    //el valor de la muestra y el valor medio
    power+=(micValue[n]-meanValue)*(micValue[n]-meanValue);
  }
  //Dividimos el sumatorio por el número de muestras
  power/=NUMBEROFSAMPLES;

  Serial.println(power);
  uint32_t threshold=1000;

  //Apagamos todos los leds
  leds.clear(0,0,1);
  //Recorremos todos los leds
  for(byte n=0;n<10;n++)
  {
    if (power>threshold) leds.setColor(n,100,0,0);
    threshold*=3;
  }
  leds.show();
  delay(10);

}