/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Material adicional: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Ahora que hemos aprendido a leer los valores del joystick, utilizaremos una matriz de leds
  monocromos de 8x8 gestionada por un chip MAX7219, para representar la posición.

  En este ejemplo no utilizaremos la función map para convertir el valor leido a uno en un rango entre
  0 y 7. La razón es que la función debería ser x=map(x,0,1023,0,7)
  Con esto la x sólo tomaría el valor 7 con 1023. Esto es demasiado cerrado.
  Utilizamos algo más simple.
  Si el rango de valores de entrada es de 1024 y queremos reducirlo a 8 posibilidades, dividimos:
  1024/8=128. Así que si dividimos el valor de entrada por 128, estaremos dando 128 valores distintos a cada
  valor de salida. Esto es bastante más flexible.

  En el programa utilizaremos el botón para decidir si se enciende el led que marca la posición en la matrix.
  Si pulsamos el botón, desaparece.
  
  Resultado:
  Vemos los valores leidos del joystick en pantalla en tiempo real
*/

#include <Arduino.h>
#include <RoJoMatrix.h>

const byte pinButton=12;

//Creamos el objeto m que gestionará la matriz (de matrices) de leds
//En la creación ya se incluye la activación y la inicialización
//tras ello estará lista para ser utilizada
//Los parámetros son:
//  pin del DIN (DATA)
//  pin del CLK
//  pin del LOAD(/CS)
//  número de matrices enlazadas
RoJoMatrix m(4,2,3,1);

void setup()
{
  pinMode(pinButton,INPUT_PULLUP);
}

void loop()
{
  //Leemos la entrada analógica para x
  int x=analogRead(A0);
  //Leemos la entrada analógica para y
  int y=analogRead(A1);
  //Leemos el estado del pulsador. 1=suelto, 1=presionado
  bool b=digitalRead(pinButton);

  //Convertimos las coordenadas a un valor entre 0 y 7
  x/=128;
  y/=128;
  //Borramos la memoria de video
  m.Clear();
  //Si el botón no está pulsado...activamos el led en las coordenadas calculadas del joystick
  if(b) m.SetLed(x,y,1);
  //Mostramos la memoria de vídeo en la matriz
  m.Refresh();
  //Esperamos un momento
  delay(50);
}
