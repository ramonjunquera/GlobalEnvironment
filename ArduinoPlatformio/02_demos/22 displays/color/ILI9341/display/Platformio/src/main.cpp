/*
  Pendiente:
  - Probar con ESP8266
    - Sólo falta probar FFat
      Error. Encontrada la librería https://github.com/Adam5Wu/ESPVFATFS (FFat para ESP8266)
      Necesita librería Misc.h & .cpp que a su vez necesita config.h (las 3 añadidas)
      pero aun necesita más!!!!. Es viable?
  - Probar con Arduino

  Autor: Ramón Junquera
  Fecha: 20220126
  Tema: Librería para display ILI9341 SPI 240x320
  Objetivo: Demo de librería RoJoILI9341
  Material: breadboard, cables, display ILI9486, lector SD
  Ejemplos: ESP32, M5Stack Fire, ESP8266, Mega, UNO, Nano, Raspberry Pi

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8
  Mega     52   50   51  53
  UNO/Nano 13   12   11  10
  RPi      11   9    10  8(CS0)
  
  En nuestro caso, aunque el dispositivo no devuelva nunca información
  (no necesita MISO), no podemos utilizar ese pin porque es utilizado
  por el protocolo SPI.

  Memoria de vídeo= 320*480*2=300Kb
  Sistema de doble buffer= memoria de vídeo *2=600Kb

  Nota 1:
    El sistema de archivos debe contener los archivos de la
    carpeta data para su correcto funcionamiento.
  Nota 2:
    Configuración de Build de Geany para Raspberry Pi:
      Compile: g++ -Wall -std=c++11 -c "%f" -pthread -I"lib"
      Build: g++ -Wall -std=c++11 -o "%e" "%f" -pthread -I"lib"
      Execute: sudo "./%e"
 
  Resultado:
    Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoILI9341.h> //Librería de gestión del display ILI9341
#include <RoJoSprite.h> //Gestión de sprites

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D8;
  const byte pinCS_display=D3;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(ARDUINO_M5STACK_FIRE) //Si es un M5Stack Fire...los pines son fijos
  const byte pinDC_display=27;
  const byte pinRES_display=33;
  const byte pinCS_display=14;
  const byte pinBackLight_display=32; //Ya viene conectado
#elif defined(ESP32) //Si es un cualquier otro ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(__arm__) //Si es una Raspberry Pi
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
  const byte pinBackLight_display=255; //No utilizado
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDC_display=5;
  const byte pinRES_display=4;
  const byte pinCS_display=3;
  const byte pinBackLight_display=255; //No utilizado
#endif

//Creamos objeto de gestión
RoJoILI9341 display;

//Test de color
//Ejemplo de funciones:
//  RoJoILI9341
//    clear
//    block
//Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
//en pantalla, formando una matriz de 7x10
void test1() {
  display.clear(); //Limpiamos pantalla

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]={
     {255,255,255} 
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 10 filas
  for(uint16_t y=0;y<10;y++) {
    //Recorremos las 7 columnas
    for(uint16_t x=0;x<7;x++) {
      //Pintamos el rectángulo relleno
      display.block(x*32,y*32,32,32,display.getColor(color[x][0],color[x][1],color[x][2]));
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*5/6;
    }
  }
  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoILI9486
//    drawPixel
void test2() {
  display.clear(); //Limpiamos display
  //Dibujamos una matriz de puntos
  for(int16_t y=display.yMax();y>=0;y-=5)
    for(int16_t x=display.xMax();x>=0;x-=5) {
      display.drawPixel(x,y,x*y/2);
    }
  delay(1000);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoILI9341
//    line
void test3() {
  display.clear(); //Limpiamos display
  //Dibujamos líneas verdes
  for(uint16_t x=0;x<display.xMax();x+=10) {
    display.line(0,0,x,display.yMax()-1,display.getColor(0,255,0));
    #ifdef ESP8266
      yield();
    #endif
  } 
  //Dibujamos líneas azules
  for(uint16_t x=0;x<display.xMax();x+=10) {
	  display.line(0,display.yMax()-1,x,0,display.getColor(0,0,255));
	  #ifdef ESP8266
      yield();
    #endif
  }
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoILI9486
//    rect
void test4() {
  display.clear(); //Limpiamos display
  display.block(40,40,80,120,display.getColor(255,0,0)); //Dibujamos un rectángulo relleno rojo
  display.rect(80,120,120,120,display.getColor(0,255,0)); //Dibujamos un rectángulo sin relleno verde
  display.block(40,200,120,80,display.getColor(0,0,255)); //Dibujamos un rectángulo relleno azul
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoILI9341
//    triangle
//    triangleFill
void test5() {
  //Limpiamos display
  display.clear();
  //Dibujamos un triángulo relleno rojo
  display.triangleFill(80,0,200,160,0,120,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  display.triangle(80,80,200,280,40,240,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  display.triangleFill(80,160,239,200,120,280,display.getColor(0,0,255));
    
  delay(1000);
}

//Test de circunferencias y elipses
//Ejemplo de funciones:
//  RoJoILI9341
//    circle
//    disk
//    ellipse
//    ellipseFill
void test6() {
  display.clear(); //Limpiamos display
  display.circle(60,60,59,display.getColor(255,0,0)); //Circunferencia
  display.disk(180,260,59,display.getColor(0,255,0)); //Círculo
  display.ellipse(180,100,59,99,display.getColor(0,0,255)); //Elipse
  display.ellipseFill(60,220,59,99,display.getColor(255,0,255)); //Elipse rellena
    
  delay(1000);
}

//Test de carga de sprite desde archivo
//Ejemplo de funciones:
//  RoJoILI9341
//    drawSprite
void test7() {
  display.clear(); //Limpiamos display

  //Dibujamos el sprite como una matriz de 7x10
  for(uint16_t y=0;y<10;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) { //De izquierda a derecha
      display.drawSprite("/heart.spr",x*32,y*32);
      #ifdef ESP8266
        yield();
      #endif
    }
  //Esperamos
  delay(1000);

  //Mostramos varias imágenes de 240x320
  //Dibujamos imagen directamente desde archivo bmp
  display.drawBMP("/moras2.bmp");
  delay(1000);
  //Dibujamos imágenes directamente desde archivo de sprite
  display.drawSprite("/lapices.spr");
  delay(1000);
  display.drawSprite("/moras1.spr");
  delay(1000);
}

//Test de texto
//Ejemplo de funciones:
//  RoJoILI9341
//    print
void test8() {
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint32_t color=display.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  display.printOver("/5x7d.fon","20190528",color);
  //Utilizaremos otra fuente más grande
  display.printOver("/10x15d.fon","20190530",color,0,8);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.printOver("/10x15d.fon","1234567890",color,-35,25); //No se ve el principio
  display.printOver("/10x15d.fon","1234567890",color,192,25); //No se ve el final
  //Fuente más grande
  display.printOver("/21x33d.fon","1234567890",color,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.printOver("/7x11.fon","Hola, esto es una prueba!",color,0,80);

  delay(2000);
}

//Test de rotación de pantalla
//Ejemplo de funciones:
//  RoJoILI9341
//    rotation
void test9() {
  uint16_t color=display.getColor(255,255,255); //Blanco
  //Limpiamos pantalla
  display.clear();
  //Dibujamos el texto para cada rotación
  display.rotation(0);
  display.printOver(F("/7x11.fon"),F("Rotation 0"),color);
  display.rotation(1);
  display.printOver(F("/7x11.fon"),F("Rotation 1"),color);
  display.rotation(2);
  display.printOver(F("/7x11.fon"),F("Rotation 2"),color);
  display.rotation(3);
  display.printOver(F("/7x11.fon"),F("Rotation 3"),color);
  display.rotation(0);

  delay(2000);
}

//Test de rotación y volteo de sprites
//Ejemplo de funciones:
//  RoJoSprite
//    rotate
//    flipH
//    flipV
void test10() {
  display.clear();
  RoJoSprite spr,spr1;
  spr.print("/10x15d.fon","1234",44444);

  display.drawSprite(&spr,20,10); //Original
  spr1.rotate(&spr,90);
  display.drawSprite(&spr1,60,30);
  spr1.rotate(&spr,270);
  display.drawSprite(&spr1,0,30);
  spr1.rotate(&spr,180);
  display.drawSprite(&spr1,20,70);

  display.drawSprite(&spr,100,0); //Original
  spr1.flipH(&spr);
  display.drawSprite(&spr1,150,0);
  spr1.flipV(&spr);
  display.drawSprite(&spr1,100,20);

  spr1.end();
  spr.end();
  delay(3000);
}

//Test de color
//Ejemplo de funciones:
//  RoJoILI9341
//    clear
//    drawSprite
//  RoJoSprite
//    setSize
//    end
//Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
//en pantalla, formando una matriz de 3x3
void test11() {
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite; //Creamos el sprite
  sprite.setSize(32,32); //Lo dimensionamos

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]={
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 10 filas
  for(uint16_t y=0;y<10;y++) {
    //Recorremos las 7 columnas
    for(uint16_t x=0;x<7;x++) {
      //Pintamos el sprite
      sprite.clear(sprite.getColor(color[x][0],color[x][1],color[x][2]));
      //Dibujamos el sprite
      display.drawSprite(&sprite,x*32,y*32);
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*5/6;
    }
  }
  //Liberamos memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//Ejemplo de funciones:
//  RoJoSprite
//    load
//    loadBMP
void test12() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Dibujamos el sprite como una matriz de 7x10
  //En las 5 primeras filas el sprite se ha cargado desde un .spr
  //En las 5 últimas desde un .bmp

  //Cargamos una imagen de 32x32 desde un archivo .spr
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos las 5 primeras filas
  for(uint16_t y=0;y<5;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);
  
  //Cargamos la imágen del sprite desde un archivo .bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos las 5 últimas filas
  for(uint16_t y=5;y<10;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);

  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de prites y texto
//Ejemplo de funciones:
//  RoJoILI9341
//    text
//  RoJoSprite
//    text
void test13() {
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint32_t color=display.getColor(0,255,0); //green
  //Creamos un sprite para dibujar el texto
  RoJoSprite sprite;
  //Escribimos un texto y lo mostramos
  sprite.print("/5x7d.fon","20190528",color);
  display.drawSprite(&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print("/10x15d.fon","20190530",color);
  display.drawSprite(&sprite,0,8);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  sprite.print("/10x15d.fon","1234567890",color);
  display.drawSprite(&sprite,-35,25); //No se ve el principio
  display.drawSprite(&sprite,192,25); //No se ve el final
  //Fuente más grande
  sprite.print("/21x33d.fon","1234567890",color);
  display.drawSprite(&sprite,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  sprite.print("/7x11.fon","Hola, esto es una prueba!",color);
  display.drawSprite(&sprite,0,80);
  //Texto rojo con fondo negro y bordes blancos
  sprite.print("/7x11.fon","Hola!",color,0,sprite.getColor(255,255,255));
  display.drawSprite(&sprite,0,100);
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de cambio de tamaño
//Ejemplo de funciones:
//  RoJoSprite
//    resize
//Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
void test14() {
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.loadSprite(F("/heart.spr"));
  //Creamos el sprite redimensionado
  RoJoSprite spriteZoom;
  for(byte z=32;z<150;z+=5) {
    //Redimensionamos el sprite
    spriteZoom.resize(&spriteOri,z,z);
    //Mostramos el sprite centrado
    display.drawSprite(&spriteZoom,(display.xMax()-spriteZoom.xMax())/2,(display.yMax()-spriteZoom.yMax())/2);
  }
  //Liberamos memoria del sprite redimensionado
  spriteZoom.end();
  //Liberamos memoria del sprite original
  spriteOri.end();

  delay(500);
}

//Test de transición
//Ejemplo de funciones:
//  RoJoSprite
//    fade
void test15() {
  display.clear();
  RoJoSprite spr,spr1,spr2;
  spr1.print("/21x33d.fon","1234",4444);
  spr2.print("/21x33d.fon","4321",3333);
  int8_t delta=1;
  byte level=0;
  uint16_t epochs=512;
  #ifndef ESP8266
    epochs*=2;
  #endif
  for(uint16_t i=epochs;i>0;i--) {
    spr.fade(&spr1,&spr2,level);
    display.drawSprite(&spr);
    level+=delta;
    if(level==0 || level==255) delta=-delta;
  }
  delay(1000);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoSprite
//    line
void test16() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  //Dibujamos líneas verdes
  uint32_t color=display.getColor(0,255,0); //Verde
  for(uint16_t x=0;x<sprite.xMax();x+=10) sprite.line(0,0,x,sprite.xMax()-1,color);
  //Dibujamos líneas azules
  color=display.getColor(0,0,255); //azul
  for(uint16_t x=0;x<sprite.xMax();x+=10) sprite.line(0,sprite.yMax()-1,x,0,color);
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoSprite
//    block
//    rect
void test17() {
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  //Dibujamos un rectángulo relleno rojo
  sprite.block(20,20,120,120,sprite.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(40,60,120,100,sprite.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(80,100,100,80,sprite.getColor(0,0,255));
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoSprite
//    triangle
//    triangleFill
void test18() {
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(80,0,100,120,0,60,sprite.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(20,100,160,40,100,199,sprite.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(100,20,199,0,140,160,sprite.getColor(0,0,255));
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de circunferencias y círculos
//Ejemplo de funciones:
//  RoJoSprite
//    circle
//    disk
//    ellipse
//    ellipseFill
void test19() {
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  sprite.circle(40,40,39,sprite.getColor(255,0,0)); //Circunferencia
  sprite.disk(40,120,39,sprite.getColor(0,255,0)); //Círculo
  sprite.ellipse(140,40,59,39,sprite.getColor(0,0,255)); //Elipse
  sprite.ellipseFill(140,120,59,39,sprite.getColor(255,0,255)); //Elipse rellena
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoSprite
//    drawPixel
void test20() {
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  //Dibujamos una matriz de puntos
  for(byte y=0;y<sprite.yMax();y+=10)
    for(byte x=0;x<sprite.xMax();x+=10)
      sprite.drawPixel(x,y,sprite.getColor(x,y,0));
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de guardado
//Ejemplo de funciones:
//  RoJoSprite
//    save
void test21() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite
  RoJoSprite sprite1;
  //Tendrá un tamaño de 3x3
  sprite1.setSize(3,3);
  //Lo pintamos de rojo
  sprite1.clear(sprite1.getColor(255,0,0));
  //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.block(1,1,2,2,sprite1.getColor(0,255,0));
  //Lo guardamos en un archivo
  sprite1.save("/test.spr");

  //Creamos un nuevo sprite
  RoJoSprite sprite2;
  //Cargamos la imágen desde un archivo
  sprite2.loadSprite("/test.spr");
  //Lo redimensionamos para que se vea claro
  sprite1.resize(&sprite2,100,100);
  //Liberamos memoria del sprite2
  sprite2.end();
  //Mostramos el sprite
  display.drawSprite(&sprite1);
  //Liberamos memoria del sprite1
  sprite1.end();
  //Borramos el archivo
  RoJoFS.remove("/test.spr");
  delay(1000);
}

//Test de reemplazar color y transparencia
//Ejemplo de funciones:
//  RoJoSprite
//    loadBMP
//    replaceColor
//    drawSprite
void test22() {
  display.clear();
  //Creamos sprite de fondo desde un archivo bmp
  RoJoSprite back;
  back.loadBMP("/moon.bmp");
  //Creamos sprite que irá sobre el fondo
  RoJoSprite over;
  over.setSize(40,40);
  over.clear(over.getColor(0,255,255)); //cyan
  over.block(20,20,39,39,over.getColor(0,255,0)); //Esquina inferior derecha en verde
  //Lo dibujamos sobre el sprite de fondo
  back.drawSprite(&over,5,5);
  //Sustituimos el color verde por azul
  over.replaceColor(over.getColor(0,255,0),over.getColor(0,0,255));
  //Lo volvemos a dibujar
  back.drawSprite(&over,55,5);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  back.drawSprite(&over,5,55,back.getColor(0,0,255));
  //Mostramos el sprite de fondo centrado
  display.drawSprite(&back,(display.xMax()-back.xMax())/2,(display.yMax()-back.yMax())/2);
  //Liberamos memoria de los sprites
  over.end();
  back.end();
  
  delay(1000);
}

//Test de copia parcial
//Ejemplo de funciones:
//  RoJoSprite
//    copy
void test23() {
  display.clear();
  RoJoSprite complete;
  complete.loadBMP("/moon.bmp");
  display.drawSprite(&complete,70,0);
  RoJoSprite quarter;
  for(int16_t y=0;y<4;y++)
    for(int16_t x=0;x<4;x++) {
      quarter.copy(&complete,x*25,y*25,25,25);
      display.drawSprite(&quarter,55+x*35,110+y*35);
    }
  delay(1000);
}

//Test de sincronización
//Ejemplo de funciones:
//  RoJoILI9341
//    drawSpriteSync
//  RoJoSprite
//    copy
void test24() {
  display.clear();

  RoJoSprite back; //Imagen original
  back.loadBMP("/moon.bmp");
  RoJoSprite videoMem; //Memoria de vídeo (de trabajo)
  videoMem.copy(&back);
  RoJoSprite displayMem; //Memoria del display
  displayMem.copy(&videoMem);
  display.drawSprite(&videoMem,70,110);
  //Ya tenemos todas las memorias sincronizadas

  RoJoSprite num; //Sprite en movimiento (un número)
  //Dibujamos el número en azul con fondo negro
  num.print("/21x33d.fon","52",num.getColor(0,0,255));

  //Coordenadas del sprite de texto
  int16_t x=0,y=0;
  //Dirección:
  int16_t dx=2,dy=2;
  //Coordenada máximas para rebote
  int16_t xBounce=videoMem.xMax()-num.xMax();
  int16_t yBounce=videoMem.yMax()-num.yMax();
  //Tiempo límite (duración = 10s)
  uint32_t maxTime=millis()+10000;
  //Mientras no superemos el tiempo límite
  while(millis()<maxTime) {
    //Calculamos nuevas coordenadas
    x+=dx; y+=dy;
    //Si hemos llegado a algún borde...cambiamos de dirección
    if(x<=0 || x>=xBounce) dx=-dx;
    if(y<=0 || y>=yBounce) dy=-dy;
    //Dibujamos la imágen de fondo sobre el sprite de trabajo
    videoMem.drawSprite(&back);
    //Añadimos el número sobre el fondo, haciendo su fondo negro transparente
    videoMem.drawSprite(&num,x,y,0);
    //Sincronizamos los sprites con las memorias de vídeo y enviamos las
    //diferencias al display
    display.drawSpriteSync(&videoMem,&displayMem,70,110);
  }
  //Liberamos memoria de los sprites
  num.end();
  displayMem.end();
  videoMem.end();
  back.end();
}

//Test de carga de sprites grandes desde archivo
//Ejemplo de funciones:
//  RoJoSprite
//    loadSprite
//    loadBMP
void test25() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Utilizaremos sprites de tamaño 240x320, igual que las dimensiones del display
  sprite.loadSprite("/lapices.spr");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadBMP("/moras2.bmp");
  display.drawSprite(&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

void setup() {
  //Serial.begin(115200);
  //while(!Serial) delay(1);
  display.begin(pinRES_display,pinDC_display,pinCS_display,pinBackLight_display);
}

void loop() {
  //Tests de funciones de dibujo directo desde el driver
  test1(); //Test de block
  test2(); //Test de pixels
  test3(); //Test de líneas
  test4(); //Test de rectángulos
  test5(); //Test de triángulos
  test6(); //Test de circunferencias y elipses
  test7(); //Test de carga de sprite desde archivo
  test8(); //Test de texto
  test9(); //Test de rotación de pantalla
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) //UNO o Nano no tienen memoria para trabajar con sprites
    test10(); //Test de rotación y volteo de sprites
    test11(); //Test de color
    test12(); //Test de carga de sprite pequeño desde archivo
    #ifndef ARDUINO_AVR_MEGA2560 //Mega no tiene memoria para cargar sprites tan grandes
      test13(); //Test de texto
      test14(); //Test de cambio de tamaño
      test15(); //Test de transición de sprites
      #ifndef ESP8266 //ESP8266 no tiene memoria para cargar sprites grandes
        test16(); //Test de líneas
        test17(); //Test de rectángulos
        test18(); //Test de triángulos
        test19(); //Test de circunferencias y elipses
        test20(); //Test de pixels
        test21(); //Test de guardado
        test22(); //Test de reemplazar color y transparencia
        test23(); //Test de copia parcial
        test24(); //Test de sincronización
        test25(); //Test de carga de sprites grandes desde archivo
      #endif
    #endif
  #endif
}
