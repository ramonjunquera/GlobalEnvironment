/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Fecha: 20180319
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  No es frecuente el uso del joystick de manera analógica.
  Lo normal es que lo único que interese sea saber la dirección hacia la que está indicando.
  No interesa cuánto.

  Con el mismo circuito que el ejercicio anterior, leeremos de manera analógica el joy
  y convertiremos sus valores a digital. No exactamente a digital, porque cada eje
  puede tener tres valores: bajo, centro y alto.

  Un eje nos da un rango de 1024 valores.
  Puesto que necesitamos 3 estados, lo dividiremos entre tres: 1024/3=341.33
  Si tomamos el valor 341, corremos el riesgo de que leamos el valor máximo (1023).
  En ese caso tendríamos 1024/341=3. Este es valor no contemplado, porque queríamos
  reducirlo a 0, 1 y 2.
  Por lo tanto, tomaremos como valor de corte 342.
  Por lo tanto, dividiremos el valor analógico por 342 y nos dará los valores:
    0 = bajo
    1 = centro
    2 = alto
  
  Representaremos en la matriz la posición por un cuadrado de 2x2

  En este ejercicio no tenemos en cuenta el botón.
  
  Resultado:
  Podemos ver gráficamente la posición digital del joy
*/

#include <Arduino.h>
#include "RoJoMAX7219SD.h" //Librería de gestión de MAX7219

//Creamos el objeto display que gestionará la cadena de chips MAX7219
RoJoMAX7219 display;
//Creamos el sprite del bloque
RoJoSprite block;

//Definición de pines
const byte pinDIN_display=4;
const byte pinCS_display=3;
const byte pinCLK_display=2;

void setup()
{
  //Inicialización del display
  //begin(byte chainedChips,byte pinDIN, byte pinCS, byte pinCLK)
  display.begin(1,pinDIN_display,pinCS_display,pinCLK_display);
  //Dimensionamos el sprite del bloque. Anchura=2. Páginas=1
  block.setSize(2,1);
  //Lo dibujamos
  //void drawPage(int16_t x,int16_t page,byte mask,byte color);
  block.drawPage(0,0,0b00000011,4); //4=escribir el valor tal cual
  block.drawPage(1,0,0b00000011,4);
}

void loop()
{
  //Leemos la entrada analógica para x
  int x=analogRead(A0);
  //Leemos la entrada analógica para y
  int y=analogRead(A1);

  //Convertimos las coordenadas a un valor entre 0 y 2
  x/=342;
  y/=342;
  //Borramos el fondo
  display.videoMem->clear();
  //Dibujamos el sprite sobre la memoria de video
  display.videoMem->drawSprite(x*3,y*3,&block,1); //1=dibujar
  //Mostramos la memoria de vídeo en la matriz
  display.show();
  //Esperamos un momento
  delay(50);
}
