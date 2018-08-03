/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Aprenderemos a utilizar el joystick que tenía la antigua PlayStation 2.

  El ejercicio será muy sencillo.
  Simplemente enviaremos los valores leidos del joystick por el puerto serie cada medio segundo, para comprobar
  que efectívamente responden a la interacción con el mando.

  Aparte de darle alimentación, conectaremos cada una de las señales de los potenciómetros a un pin de
  entrada analógico.

  Si colocamos los pines del joystick a la izquierda, tenemos que la esquina superior izquierda tiene las 
  coordenadaa 0,0.
  El horizontal es el eje X. El vertical el Y.
  La esquina inferior derecha tiene las coordenadas máximas: 1023,1023

  Puesto que el joystick tiene su posición en el centro, tendremos que por defecto las coordenadas son
  aproximadamente la mitad del rango. Idealmente serían 511,511, pero en la práctica tenemos que no es tan
  exacto. Los valores dependen de cada mando. En mi caso tiene las coordenadas 503,488
  Podemos considerar que el joystick está en el centro si se encuentra en el rango de valores de 511+-30 

  Consideramos el botón del mando como un interruptor de pulsador normal.
  Lo conectaremos al pin 12.
  En la función setup() definiremos el pin del botón como de entrada, pero con la particularidad de que
  activaremos las resistencias internas de pullup.
  Esto nos permite ahorrarnos incluir la resistencia en el circuito.
  Cuando el botón NO esté pulsado, leeremos el valor 1. Cuando se pulse, 0.
  
  Una de las típicas preguntas es: cómo puedo sujetar el joystick a la protoboard si los pines para engancharla
  están en un lateral?
  La "típica" respuesta es: con un par de gomas.

  En el circuito adjunto en Fritzing se ha utilizado un componente standard para representar el joystick.
  Así no tendremos que cargar uno customizado.
  El problema es que los pines no coinciden con los del mando real, pero esta es la equivalencia:
    GND = GND
    +5V = VCC
    VRX = HOR
    VRY = VER
    SW  = SEL
  
  Resultado:
  Vemos los valores leidos del joystick en pantalla en tiempo real
*/

#include <Arduino.h>

const byte pinButton=12;

void setup()
{
  Serial.begin(115200);
  pinMode(pinButton,INPUT_PULLUP);
}

void loop()
{
  int x=analogRead(A0);
  int y=analogRead(A1);
  bool b=digitalRead(pinButton);
  Serial.print("X=");
  Serial.print(x);
  Serial.print(",Y=");
  Serial.print(y);
  Serial.print(",B=");
  Serial.println(b);
  delay(500);
}
