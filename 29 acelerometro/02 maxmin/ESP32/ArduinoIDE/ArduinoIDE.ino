/*
  Autor: Ramón Junquera
  Fecha: 20181103
  Tema: Acelerómetro GY-61
  Objetivo: Lectura de los valores máximos y mínimos
  Material: breadboard, acelerómetro GY-61, placa ESP32

  Descripción:
  Mostraremos los valores máximos y mínimos leidos para cada eje.
  Esto puede servir para conocer los rangos de valores de cada eje en 1g y poder calibrarlo.
  Es importante tener en cuenta que el dispositivoes capaz de leer entre -3g y 3g.
  Cualquier golpe o movimiento brusco quedará reflejado.
  Un ejemplo de valores de referencia máximos y mínimos en reposo podrían ser:
  [1360,2329] [1358,2379] [1362,2359] 
  Ejemplo de valores de dispositivo en horizontal y en reposo:
  [1758,1963] [1835,2032] [2141,2363] 

  Resultado:
  Vemos el los valores máxmios y mínimos decada eje
*/

#include <Arduino.h>

//Definición de constantes globales
const byte pins[]={36,37,38}; //Corresponden a los pinres A0, A1 y A2

//Definición de variables globales
uint16_t max[3],min[3];

void setup()
{
  //Inicializamos el puerto serie para que transmita a 115200 baudios
  Serial.begin(115200);
  //Inicializamos los valore de máximos y mínimos con una primera lectura
  for(byte i=0;i<3;i++) max[i]=min[i]=analogRead(pins[i]);
}

void loop()
{
  //Leeremos 1000 valores antes de mostrar los resultados
  for(uint16_t i=0;i<1000;i++)
    //Leemos cada uno de los 3 ejes
    for(byte a=0;a<3;a++)
    {
      //Leemos el valor del eje
      uint16_t v=analogRead(pins[a]);
      //Si es mayor que el mayor...ahora será este el mayor
      if(v>max[a]) max[a]=v;
      //Si es menor que el mmenor...ahora será este el menor
      if(v<min[a]) min[a]=v;
    }
  //Recorremos los 3 ejes y mostramos su valor
  for(byte a=0;a<3;a++) Serial.print("["+String(min[a])+","+String(max[a])+"] ");
  Serial.println();
}
