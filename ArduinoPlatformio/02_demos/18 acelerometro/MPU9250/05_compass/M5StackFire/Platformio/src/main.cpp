/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro 
  Objetivo: Mostrar brújula gráficamente
  Material: M5Stack Fire

  Descripción:
    Se muestra gráficamente y en tiempo real los datos leidos del magnetómetro en
    forma de brújula: flecha, posición de norte y grados con respecto al norte real (azimut).

  Notas:
    - Existe una constante global (autoCalibrate) para que la brújula se
      calibre automáticamente. Por defecto está activa. Si la desactivamos
      debemos indicar manualmente los valores de configuración.
    - Puesto que no se utiliza ni el giróscopo ni el acelerómetro, se desactivan.
    - Se incluye una línea comentada para que se muestre en el display las coordenadas del
      punto medio. Esto es útil para recalcular las coordenadas del punto medio cuando
      se activa el modo de calibración automático.
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display RoJoILI9341
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de variables globales
RoJoILI9341 display;
RoJoSprite v; //Sprite de memoria de vídeo
RoJoMPU9250 GAM; //Sensores

const bool autoCalibrate=true; //Autocalibración
int16_t azimut; //Ángulo de azimut
RoJoColor yellow={255,255,0}; //Color amarillo

void setup() {
  //Inicialización de display para M5Stack Fire
  display.begin(33,27,14,32);

  display.line(120,40,120,35,{255,255,255}); //Marca de Norte
  display.printOver("/10x15.fon","N",{255,255,255},115,15); //N de Norte

  //Dimensionamos sprite cuadrado para la memoria de vídeo
  v.setSize(240,240);
  //Inicialización de sensores
  GAM.begin();
  //No necesitaremos ni el Giróscopo ni el Acelerómetro
  GAM.sleepGA(true);

  if(!autoCalibrate) { //Si no debe autocalibrarse...
    GAM.autoM=false;
    GAM.midM[0]=-5601; //Promedio de medidas X en horizontal
    GAM.midM[1]=7885; //Promedio de medidas Y en horizontal
  }
}

void loop() {
  azimut=GAM.readAzimut(); //Leemos al azimut
  if(azimut>=0) { //Si el azimut es válido...
    //Estamos utilizando la pantalla rotada 90 grados.
    //Debemos sumar los 90 grados de rotación de pantalla al azimut calculado
    azimut+=90; if(azimut>360) azimut-=360;

    v.clear(); //Borramos la memoria de vídeo

    //Escribimos el valor del azimut en pantalla
    RoJoSprite txt; //Creamos un sprite monocromo para guardar el texto
    txt.print("/10x15.fon",String(azimut),{255,255,255}); //Lo llenamos con el valor texto del valor del azimut
    v.drawSprite(&txt,(display.xMax()-txt.xMax())/2,113); //Lo dibujamos centrado
    txt.end(); //Hemos terminado con el sprite de texto

    float azimutRad=(float)azimut*PI/180; //Pasamos el azimut a radianes

    //Dibujamos la flecha que indica la dirección en forma de triángulo
    v.triangle(120-119*sin(azimutRad),120-119*cos(azimutRad),120-60*sin(azimutRad+2.5),120-60*cos(azimutRad+2.5),120-60*sin(azimutRad-2.5),120-60*cos(azimutRad-2.5),yellow);
    v.circle(120,120,119,{255,255,255}); //Dibujamos la esfera

    //DEBUG. Muestra coordenadas de punto medio
    //v.printOver("/5x7.fon",String(GAM.midM[0])+","+String(GAM.midM[1]),0xFFFF,90,200);

    display.drawSprite(&v,0,40);
  }
}
