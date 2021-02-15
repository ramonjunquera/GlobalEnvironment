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
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306

//Definición de pines joystick
const byte pinX=2; 
const byte pinY=0;
//Definición de pines display
const byte pinSDA=4;
const byte pinSCL=15;
const byte pinRES=16;

//Creamos objeto de gestión
RoJoSSD1306 display;
//Creamos un sprite monocromo que utilizaremos como memoria de vídeo
RoJoSprite v(1);
//Últimas coordenadas
uint16_t lastX,lastY;

void setup() {
  //Inicializamos el display con los pines en los que está conectado la placa
  //void begin(byte pinSDA,byte pinSCL,byte pinRST,uint32_t freqCOMM=0);
  display.begin(pinSDA,pinSCL,pinRES);
  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop() {
  //Leemos coordenadas actuales
  uint16_t x=127-analogRead(pinX)/32; //Conversión 4096/128=32.  Eje X invertido
  uint16_t y=analogRead(pinY)/64; //Conversión 4096/64=64
  //Si son las mismas que las de la última vez...terminamos
  if(x==lastX && y==lastY) return;
  //Borramos sprite de memoria de vídeo
  v.clear();
  //Dibujamos el nuevo pixel
  v.drawPixel(x,y,1);
  //Mostramos el sprite de memoria de vídeo
  display.drawSprite(&v);
  //Las coordenadas anteriores serán las actuales
  lastX=x; lastY=y;
  delay(100);
}
