/*
  Autor: Ramón Junquera
  Fecha: 20201125
  Tema: Micrófono
  Objetivo: Demo de grabación/reproducción con librería RoJoI2Sa
  Material: M5Stack Fire

  Descripción:
  Dados unos parámetros iniciales:
  - Pines de micrífono y buzzer
  - Número de muestras
  - Frecuencia de muestreo
  Creamos un buffer para almacenar todas las muestras.
  Asignamos funionalidad a los dos primeros botones:
  - Botón A : grabación
    Solicitamos la recuperación de muestras del micrófono.
    Convertimos las muestras de 16 a 8 bits para poder reproducirlas
  - Botón B : reproducción
    Solicitamos el envío de las muestras al buzzer
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Gestión de sprites color
#include <RoJoSwitch.h> //Gestión de interruptores
#include <RoJoI2Sa.h> //Gestión de dispositivos de audio analógicos

//Contantes globales
const byte pinMic=34;
const byte pinBuzzer=25;
const uint32_t samples=20000; //Número de muestras (2s a 10KHz)
const uint32_t freq=10000; //Frecuencia de muestreo

//Creamos objeto de gestión
RoJoILI9341 display;
RoJoSwitch buttonA(39),buttonB(38); //Botones en M5Stack Fire

//Variables globales
uint16_t sampleBuffer[samples];

void setup() {
  //Inicializamos el display para M5Stack Fire
  //RES=33,DC=27,CS=14,BackLight=32
  display.begin(33,27,14,32);
  display.rotation(1); //Botonera abajo
  display.disk(65,225,10,{255,0,0}); //Label REC
  display.triangleFill(150,215,170,225,150,235,{0,255,0}); //Label PLAY
  display.printOver("/10x22.fon","Demo grabacion/reproduccion",{255,255,255},25);
  RoJoI2Sa_begin(0,pinMic,freq); //Inicializamos I2Sa con timer 0 para leer micro
  RoJoI2Sa_begin(1,pinBuzzer,freq); //Inicializamos I2Sa con timer 1 para escribir buzzer
  //Nota:
  //No hay problema en utilizar un único timer para ambas tareas, pero habría que 
  //preocuparse en cambiar el pin con RoJoI2Sa_begin antes de una comunicación.
}

void loop() {
  if(buttonA.pressed()) { //Si se ha pulsado el botón de grabación...
    display.disk(160,120,60,{255,0,0}); //Mostramos icono REC
    RoJoI2Sa_read(0,sampleBuffer,samples); //Leemos datos del micrófono
    display.block(100,60,220,180,{0,0,0}); //Borramos icono REC
    RoJoI2Sa_convert16to8(sampleBuffer,(byte*)sampleBuffer,samples); //Convirtiendo a 8bits
  } else if (buttonB.pressed()) { //Si se ha pulsado el botón de reproducción...
    display.triangleFill(100,60,220,120,100,180,{0,255,0}); //Mostramos icono PLAY
    RoJoI2Sa_write(1,(byte*)sampleBuffer,samples); //Escribimos datos en buzzer
    display.block(100,60,220,180,{0,0,0}); //Borramos icono PLAY
  }
  delay(10);
}
