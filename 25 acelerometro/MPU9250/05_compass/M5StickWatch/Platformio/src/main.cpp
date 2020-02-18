/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro 
  Objetivo: Mostrar brújula gráficamente
  Material: M5Stack Watch

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
      se activa el modo de configuración automático.
*/

#include <Arduino.h>
#include <RoJoSH1107.h> //Librería de gestión del display SH1107
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de variables globales
RoJoSH1107 display;
RoJoSprite v(1); //Sprite monocromo de memoria de vídeo
RoJoMPU9250 GAM; //Sensores

const bool autoCalibrate=true; //Autocalibración
int16_t azimut; //Ángulo de azimut

void setup() {
  //Inicialización de display para M5 Watch
  display.begin(33,27,14);
  //Dimensionamos sprite de memoria de vídeo con tamaño de display
  v.setSize(display.xMax(),display.yMax());
  //Inicialización de sensores
  GAM.begin();
  //No necesitaremos ni el Giróscopo ni el Acelerómetro
  GAM.sleepGA(true);

  if(!autoCalibrate) { //Si no se debe autocalibrar...
    GAM.autoM=false;
    GAM.midM[0]=21435; //Promedio de medidas X en horizontal
    GAM.midM[1]=5439; //Promedio de medidas Y en horizontal
  }
}

void loop() {
  azimut=GAM.readAzimut(); //Leemos al azimut
  if(azimut>=0) { //Si el azimut es válido...
    v.clear(); //Borramos la memoria de vídeo

    //Escribimos el valor del azimut en pantalla
    RoJoSprite txt(1); //Creamos un sprite monocromo para guardar el texto
    txt.print("/5x7.fon",String(azimut),{0,0,1}); //Lo llenamos con el valor texto del valor del azimut
    v.drawSprite(&txt,(display.xMax()-txt.xMax())/2,104); //lodibujamos centrado
    txt.end(); //Hemos terminado con el sprite de texto

    float azimutRad=(float)azimut*PI/180; //Pasamos el azimut a radianes

    //Dibujamos la flecha que indica la dirección
    //Versión simple: una sóla línea
    //v.line(32,64,32-31*sin(azimutRad),64-31*cos(azimutRad),{0,0,1});
    //Versión más bonita: un triángulo
    v.triangle(32-31*sin(azimutRad),64-31*cos(azimutRad),32-15*sin(azimutRad+2.5),64-15*cos(azimutRad+2.5),32-15*sin(azimutRad-2.5),64-15*cos(azimutRad-2.5),{0,0,1});

    v.circle(32,64,31,{0,0,1}); //Dibujamos la esfera

    v.line(32,32,32,29,{0,0,1}); //Marca de Norte
    v.printOver("/5x7.fon","N",{0,0,1},30,20); //N de Norte

    //DEBUG. Muestra coordenadas de punto medio
    //v.printOver("/5x7.fon",String(GAM.midM[0])+","+String(GAM.midM[1]),{0,0,1},0,120);

    display.drawSprite(&v);
  }
  delay(200);
}
