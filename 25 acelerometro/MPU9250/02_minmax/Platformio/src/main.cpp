/*
  Autor: Ramón Junquera
  Fecha: 20190917
  Tema: Acelerómetro MPU9250
  Objetivo: Lectura de los valores mínimos y máximos
  Material: M5Stack Watch
  Descripción:
    Muestra los valores mínimos y máximos para cada sensor y eje en tiempo real en el display.
    Una vez que comience el programa se debe mover el dispositivo sobre todos sus ejes hasta
    que deje de actualizarse.
  Identificador de sensores:
    0 : Giróscopo
    1 : Acelerómetro
    2 : Magnetómetro
  Nomenclatura:
    GX: Giróscopo    : Eje X : mínimo, máximo
    GY: Giróscopo    : Eje Y : mínimo, máximo
    GZ: Giróscopo    : Eje Z : mínimo, máximo
    AX: Acelerómetro : Eje X : mínimo, máximo
    AY: Acelerómetro : Eje Y : mínimo, máximo
    AZ: Acelerómetro : Eje Z : mínimo, máximo
    MX: Magnetómetro : Eje X : mínimo, máximo
    MY: Magnetómetro : Eje Y : mínimo, máximo
    MZ: Magnetómetro : Eje Z : mínimo, máximo
    mid: coord media : x, y
  Ejemplo de rango: 
    GX:-2150,4206
    GY:-2260,5599
    GZ:-2325,6310
    AX:-2880,2467
    AY:-2084,2261
    Az:-2267,2994
    MX:21181,21649
    MY:5193,5658
    MZ:-2669,-2192
    mid:21415,5425
*/

#include <Arduino.h>
#include <RoJoSH1107.h> //Librería de gestión del display SH1107
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de variables globales
RoJoSH1107 display;
RoJoSprite v(1); //Sprite monocromo de memoria de vídeo
RoJoMPU9250 GAM;         //Giróscopo, Acelerómetro y Magnetómetro
int16_t minmax[3][3][2]; //Valores minimo y máximo para cada: [sensor][eje][min/max]

void setup() {
   //Inicialización de display para M5 Watch
   display.begin(33,27,14);
   //Dimensionamos sprite de memoria de vídeo con tamaño de display
   v.setSize(display.xMax(),display.yMax());
   GAM.begin(); //Inicialización de sensores
   //Fijamos valores por defecto
   for (byte sensor = 0; sensor < 3; sensor++)
      for (byte axis = 0; axis < 3; axis++) {
         minmax[sensor][axis][0] = 32766;  //mínimo
         minmax[sensor][axis][1] = -32766; //máximo
      }
}

void loop() {
   int16_t data[3];      //Array para guardar datos de ejes de un sensor
   bool changed = false; //hay algun cambio?

   GAM.readG(data); //Leemos datos de Giróscopo
   //Actualizamos límites de rango
   for (byte axis = 0; axis < 3; axis++) {
      if (data[axis] < minmax[0][axis][0]) {
         minmax[0][axis][0] = data[axis]; //Mínimo
         changed = true;
      }

      if (data[axis] > minmax[0][axis][1]) {
         minmax[0][axis][1] = data[axis]; //Máximo
         changed = true;
      }
   }

   GAM.readA(data); //Leemos datos de Acelerómetro
   //Actualizamos límites de rango
   for (byte axis = 0; axis < 3; axis++) {
      if (data[axis] < minmax[1][axis][0]) {
         minmax[1][axis][0] = data[axis]; //Mínimo
         changed = true;
      }
      if (data[axis] > minmax[1][axis][1]) {
         minmax[1][axis][1] = data[axis]; //Máximo
         changed = true;
      }
   }

   //Si conseguimos los datos del magnetómetro...
   if (GAM.readM(data)) {
      for (byte axis = 0; axis < 3; axis++) {
         if (data[axis] < minmax[2][axis][0]) {
            minmax[2][axis][0] = data[axis]; //Mínimo
            changed = true;
         }
         if (data[axis] > minmax[2][axis][1]) {
            minmax[2][axis][1] = data[axis]; //Máximo
            changed = true;
         }
      }
   }

   //Si ha habido cambios...los mostramos
   if (changed) {
      v.clear();
      v.printOver("/3x5.fon","GX:"+String(minmax[0][0][0])+","+String(minmax[0][0][1]),1);
      v.printOver("/3x5.fon","GY:"+String(minmax[0][1][0])+","+String(minmax[0][1][1]),1,0,8);
      v.printOver("/3x5.fon","GZ:"+String(minmax[0][2][0])+","+String(minmax[0][2][1]),1,0,16);
      v.printOver("/3x5.fon","AX:"+String(minmax[1][0][0])+","+String(minmax[1][0][1]),1,0,24);
      v.printOver("/3x5.fon","AY:"+String(minmax[1][1][0])+","+String(minmax[1][1][1]),1,0,32);
      v.printOver("/3x5.fon","AZ:"+String(minmax[1][2][0])+","+String(minmax[1][2][1]),1,0,40);
      v.printOver("/3x5.fon","MX:"+String(minmax[2][0][0])+","+String(minmax[2][0][1]),1,0,48);
      v.printOver("/3x5.fon","MY:"+String(minmax[2][1][0])+","+String(minmax[2][1][1]),1,0,56);
      v.printOver("/3x5.fon","MZ:"+String(minmax[2][2][0])+","+String(minmax[2][2][1]),1,0,64);
      v.printOver("/3x5.fon","mid:"+String(GAM.midM[0])+","+String(GAM.midM[1]),1,0,72);
      display.drawSprite(&v);
   }

   delay(10);
}