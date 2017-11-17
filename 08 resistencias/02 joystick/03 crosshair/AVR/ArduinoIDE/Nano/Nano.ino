/*
  Autor: Ramón Junquera
  Tema: Lectura y escritura de señales digitales
  Objetivo: Funcionamiento del joystick de PS2
  Material: breadboard, Arduino Nano, joystick de PS2, cables

  Descripción:
  Ahora que somos capaces de posicionar un led en relación al estado del joy, mostraremos un 
  punto de mira (un sprite) gracias a las capacidades de la librería RoJoMatrix.

  Cuando pulsemos el botón del joy, se quedará encendido el led marcado.

  El circuito es el mismo que el del ejercicio anterior.
  
  Resultado:
  Podemos encender leds, señalándolos con un punto de mira
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
//Creamos las variables del sprite
const byte spriteWidth=5; //Anchura (número de columnas)
byte spriteGraphic[spriteWidth]=
{
  B00000100,
  B00000100,
  B00011011,
  B00000100,
  B00000100
}; //Datos gráficos
//Creamos las variables del fondo dibujado
const byte backWidth=8; //Anchura (número de columnas)
byte backGraphic[backWidth]; //Datos gráficos

void setup()
{
  //Definimos el pin al que conectamos el pulsador del joy como entrada con resistencia pullup interna activa
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
  //Si se ha pulsado el botón...activamos en el fondo el led seleccionado
  if(!b) backGraphic[x]|=1<<y;
  //Dibujamos el fondo dibujado siempre comenzando desde el origen y sin que sea transparente
  m.SetSprite(backGraphic,backWidth,0,0,false);
  //Dibujamos el punto de mira sobre la memoria de video, haciendo su fondo transparente
  m.SetSprite(spriteGraphic,spriteWidth,x-2,y-2,true);
  //Mostramos la memoria de vídeo en la matriz
  m.Refresh();
  //Esperamos un momento
  delay(50);
}
