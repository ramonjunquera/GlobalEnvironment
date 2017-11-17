/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia
  Objetivo: Funcionamiento de una fotorresistencia
  Material: breadboard, fotorresistencia, resistencia de 10Kohmios, 3 resistencias de 220 ohmios, 3 leds,
    cables, Arduino UNO

  Descripción:
  Basándonos en los datos obtenidos en el ejercicio anterior, sustituiremos la lectura de los valores de la
  fotorresistencia por un ecualizador formado por tres leds.
  Encenderemos un número de leds proporcional a la cantidad de luz recibida. Desde ningún led (a oscuras)
  a los tres leds con incidencia de luz directa.

  Para comprobar que todo es correcto, no hemos desabilidado el envío de información por el puerto serie.

  Es importante colocar los leds alejados del fotorresistor para que su luz no influya.
  También podemos poner algún tipo de separador (cartón) que impida que la luz del lez izquierdo le afecte.

  Si no obtenemos el resultado deseado, podemos ajustar los valores a partir de los cuales se enciende cada led.
  
  Resultado:
  Cuanta más luz se detecte, más leds se encienden.
*/

#include <Arduino.h>

//Definición de pines
const byte pinLed1 = 11;
const byte pinLed2 = 12;
const byte pinLed3 = 13;

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
  //Todos los pines de los leds son de salida
  pinMode(pinLed1,OUTPUT);
  pinMode(pinLed2,OUTPUT);
  pinMode(pinLed3,OUTPUT);
}

void loop()
{
  //Leemos el valor analógico
  int valueLDR = analogRead(A0);
  //Decidimos a partir de qué valor se debe encender cada uno de los leds
  digitalWrite(pinLed1,(valueLDR>500?HIGH:LOW));
  digitalWrite(pinLed2,(valueLDR>800?HIGH:LOW));
  digitalWrite(pinLed3,(valueLDR>980?HIGH:LOW));
  Serial.println(valueLDR);
  delay(100);
}
