/*
  Tema: Sensor de humedad de tierra
  Fecha: 20211206
  Autor: Ramón Junquera

  Descripción:
  Es un sensor de humedad de tierra capacitivo.
  Aparte de los pintes de alimentación, sólo tiene un pin de datos analógico.
  Su rango de voltaje es de 3.3 a 5.5V.
  El rango de valores ADC es acotado. No corresponde con el rango completo.
  La función getHumidityPercert incluye dos constantes para poder calibrarlo y
  calcular correctamente el procentaje de humedad.
  Cuanto más humedad tenga menor será el valor (hasta su límite inferior).

  Precisión:
  El cambio de 0 a 70% es casi instantáneo con sólo tocarlo con los dedos o
  mojarlo un poco. Demasiado sensible.

  Resultado:
  Muestra el % de humedad cada segundo

  Nota:
  - Aunque se indica claramente el límite máximo para enterrarlo, los
    sensores capacitivos suben hasta la parte electrónica.
  - El pegamento de la cinta adhesiva es conductor de la electricidad.
  - Si ponemos cinta adhesiva para proteger la parte electrónica del sensor
    sólo conseguiremos que nos de siempre un 100% de humedad. Mala idea.
  - Si queremos proteger la electrónica deberíamos pensar en una protección sin adhesivos.
    O una caja estanca hecha a medida o plástico que lo recubra.
  - El sensor se entierra. Se convierte en un pin de tierra perfecto. No necesita el GND
    de la placa. Si lo conectamos a VCC, consumirá energía aunque nos preocupemos en
    cortar la conexión GND. Esto es importante si hacemos uso de Deep Sleep y queremos
    reducir el consumo. Solución: aislar TODOS los pines utilizados por el sensor: GND, VCC & ADC.
*/

#include <Arduino.h>

//Definición de constantes globales
const byte pinHumidityADC=34;

//Devuelve el % de humedad
uint16_t getHumidityPercert(byte pin) {
  const uint16_t fullWet=896; //Valor ADC para humedad máxima
  const uint16_t fullDry=2742; //Valor ADC para todo seco
  //Tomaremos 16 medidas y calcularemos el promedio
  uint16_t analog=0;
  for(byte i=16;i>0;i--) {
    analog+=analogRead(pin);
    delay(1);
  }
  analog/=16;
  //Calculamos el % de humedad
  analog=100-100*(analog-fullWet)/(fullDry-fullWet);
  if(analog>200) analog=0;
  else if(analog>100) analog=100;
  return analog;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(getHumidityPercert(pinHumidityADC));
  delay(1000);
}