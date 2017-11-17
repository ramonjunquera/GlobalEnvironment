/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
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
  Por lo tanto, dividiremos el valore analógico por 342 y nos dará los valores:
    0 = bajo
    1 = centro
    2 = alto
  
  Representaremos en la matriz la posición por un cuadrado de 2x2

  En este ejercicio no tenemos en cuenta el botón.
  
  Resultado:
  Podemos ver gráficamente la posición digital del joy
*/

#include <Arduino.h>
#include <RoJoMatrix.h>

//Creamos el objeto m que gestionará la matriz (de matrices) de leds
//En la creación ya se incluye la activación y la inicialización
//tras ello estará lista para ser utilizada
//Los parámetros son:
//  pin del DIN (DATA)
//  pin del CLK
//  pin del LOAD(/CS)
//  número de matrices enlazadas
RoJoMatrix m(4,2,3,1);
//Creamos las variables del sprite
const byte spriteWidth=2; //Anchura (número de columnas)
byte spriteGraphic[spriteWidth]=
{
  B11,
  B11
}; //Datos gráficos

void setup()
{
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
  m.Clear();
  //Dibujamos el sprite sobre la memoria de video
  m.SetSprite(spriteGraphic,spriteWidth,x*3,y*3,false);
  //Mostramos la memoria de vídeo en la matriz
  m.Refresh();
  //Esperamos un momento
  delay(50);
}
