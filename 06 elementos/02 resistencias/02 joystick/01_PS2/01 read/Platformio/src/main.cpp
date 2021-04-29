/*
  Autor: Ramón Junquera
  Fecha: 20191230
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Material: breadboard, joystick de PS2, cables

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

  Nota:
  Puesto que los circuitos de conversión analógico-digital dependen del dispositivo
  podremos comprobar que en placas Arduino, los valores máximos son de 1023, y en 
  placas ESP32 alcanzan los 4095.
*/

#include <Arduino.h>

//Definición de pines
//Los ejes X,Y están intercambiados
//El eje x está invertido
#ifdef ARDUINO_ARCH_AVR //Placa Arduino
  const byte pinButton=12;
  const byte pinX=A1;
  const byte pinY=A0;
#elif defined(ESP32) //Si es ESP32
  const byte pinButton=17;
  const byte pinX=2; 
  const byte pinY=0; 
#endif

void setup() {
  Serial.begin(115200);
  pinMode(pinButton,INPUT_PULLUP);
}

void loop() {
  int x=analogRead(pinX);
  int y=analogRead(pinY);
  bool b=digitalRead(pinButton);
  Serial.print("X=");
  Serial.print(x);
  Serial.print(",Y=");
  Serial.print(y);
  Serial.print(",B=");
  Serial.println(b);
  delay(500);
}

