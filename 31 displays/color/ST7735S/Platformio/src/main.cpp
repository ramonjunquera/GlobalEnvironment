/*
  Autor: Ramón Junquera
  Fecha: 20191204
  Tema: Librería para display ST7735S SPI de 80x160
  Objetivo: Demo de librería RoJoST7735S

  Descripción:
    Demostración de funcionalidades de la librería.
*/

#include <Arduino.h>
#include <RoJoST7735S.h>

RoJoST7735S display;

//Pinout
#ifdef ARDUINO_LOLIN32 //Para Lolin32
  byte pinRES=2;
  byte pinDC=22;
  byte pinCS=5;
  byte pinCLK=18;
  byte pinMOSI=23;
#elif defined(ARDUINO_M5Stick_C) //Para M5Stick-C
  byte pinRES=18;
  byte pinDC=23;
  byte pinCS=5;
  byte pinCLK=13;
  byte pinMOSI=15;
#endif

//Test de color
//Ejemplo de funciones:
//  RoJoST7735S
//    clear
//    block
//Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
//en pantalla, formando una matriz de 4x8
void test1() {
  display.clear(); //Limpiamos pantalla

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[4][3]={
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
  };
  //Recorremos las filas
  for(uint16_t y=0;y<8;y++) {
    //Recorremos las columnas
    for(uint16_t x=0;x<4;x++) {
      //Pintamos el rectángulo relleno
      display.block(x*20,y*20,x*20+19,y*20+19,display.getColor(color[x][0],color[x][1],color[x][2]));
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*2/3;
    }
  }
  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoST7735S
//    drawPixel
void test2() {
  //Limpiamos display
  display.clear();
  //Dibujamos una matriz de puntos
  for(uint32_t y=0;y<display.yMax();y+=5)
    for(uint32_t x=0;x<display.xMax();x+=5)
      display.drawPixel(x,y,x*y/2);
  delay(1000);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoST7735S
//    line
void test3() {
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint16_t color=display.getColor(0,255,0); //verde
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10) {
    display.line(0,0,x,display.yMax()-1,color);
    #ifdef ESP8266
      yield();
    #endif
  } 
  //Dibujamos líneas azules
  color=display.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10) {
	  display.line(0,display.yMax()-1,x,0,color);
	  #ifdef ESP8266
      yield();
    #endif
  }
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoST7735S
//    rect
void test4() {
  //Limpiamos display
  display.clear();
  //Dibujamos un rectángulo relleno rojo
  display.block(10,10,60,50,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.rect(30,30,70,120,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.block(20,90,50,140,display.getColor(0,0,255));
  
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoST7735S
//    triangle
//    triangleFill
void test5() {
  //Limpiamos display
  display.clear();
  //Dibujamos un triángulo relleno rojo
  display.triangleFill(30,10,70,70,10,50,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  display.triangle(70,30,50,110,20,80,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  display.triangleFill(70,80,60,150,10,100,display.getColor(0,0,255));
    
  delay(1000);
}

//Test de circunferencias y elipses
//Ejemplo de funciones:
//  RoJoST7735S
//    circle
//    disk
//    ellipse
//    ellipseFill
void test6() {
  display.clear(); //Limpiamos display
  display.circle(20,30,19,display.getColor(255,0,0)); //Circunferencia
  display.disk(60,130,19,display.getColor(0,255,0)); //Círculo
  display.ellipse(60,50,19,40,display.getColor(0,0,255)); //Elipse
  display.ellipseFill(20,110,19,40,display.getColor(255,0,255)); //Elipse rellena
    
  delay(1000);
}

//Test de carga de sprite desde archivo
//Ejemplo de funciones:
//  RoJoST7735S
//    drawSprite
void test7() {
  display.clear(); //Limpiamos display

  //Dibujamos el sprite como una matriz de 2x5
  for(uint16_t y=0;y<5;y++) //Se arriba a abajo
    for(uint16_t x=0;x<2;x++) { //De izquierda a derecha
      display.drawSprite("/heart.spr",x*32,y*32);
      #ifdef ESP8266
        yield();
      #endif
    }
  //Esperamos
  delay(1000);

  //Mostramos varias imágenes de 80x160
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
//  RoJoST7735S
//    print
void test8() {
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint16_t color=display.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  display.printOver("/5x7d.fon","20191102",color);
  //Utilizaremos otra fuente más grande
  display.printOver("/10x15d.fon","20191102",color,0,8);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.printOver("/10x15d.fon","123456",color,-40,25); //No se ve el principio
  display.printOver("/10x15d.fon","123456",color,40,25); //No se ve el final
  //Fuente más grande
  display.printOver("/21x33d.fon","12345",color,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.printOver("/7x11.fon","Esto es una prueba!",color,0,80);

  delay(2000);
}

//Test de rotación de pantalla
//Ejemplo de funciones:
//  RoJoST7735S
//    rotation
void test9() {
  //Limpiamos pantalla
  display.clear();

  //Dibujamos el texto para cada rotación
  display.rotation(0);
  display.printOver(F("/7x11.fon"),F("Rotation 0"),0xFFFF);
  display.rotation(1);
  display.printOver(F("/7x11.fon"),F("Rotation 1"),0xFFFF);
  display.rotation(2);
  display.printOver(F("/7x11.fon"),F("Rotation 2"),0xFFFF);
  display.rotation(3);
  display.printOver(F("/7x11.fon"),F("Rotation 3"),0xFFFF);
  display.rotation(0);

  delay(2000);
}

//Test de color
//Ejemplo de funciones:
//  RoJoST7735S
//    clear
//    drawSprite
//  RoJoSprite
//    setSize
//    end
//Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
//en pantalla, formando una matriz de 4x8
void test10() {
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite; //Creamos el sprite
  sprite.setSize(20,20); //Lo dimensionamos

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[4][3]={
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
  };
  //Recorremos las filas
  for(uint16_t y=0;y<8;y++) {
    //Recorremos las columnas
    for(uint16_t x=0;x<4;x++) {
      //Pintamos el sprite
      sprite.clear(sprite.getColor(color[x][0],color[x][1],color[x][2]));
      //Dibujamos el sprite
      display.drawSprite(&sprite,x*20,y*20);
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*2/3;
    }
  }
  //Liberamos memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//Ejemplo de funciones:
//  RoJoST7735S
//    load
//    loadBMP
void test11() {
  display.clear(); //Limpiamos pantalla
  //Creamos el sprite
  RoJoSprite sprite;
  //Dibujamos el sprite como una matriz de 2x5
  //En las 3 primeras filas el sprite se ha cargado desde un .spr
  //En las 2 últimas desde un .bmp

  //Cargamos una imagen de 32x32 desde un archivo .spr
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos las 3 primeras filas
  for(uint16_t y=0;y<3;y++) //Se arriba a abajo
    for(uint16_t x=0;x<2;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);
  
  //Cargamos la imágen del sprite desde un archivo .bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos las 2 últimas filas
  for(uint16_t y=3;y<5;y++) //Se arriba a abajo
    for(uint16_t x=0;x<2;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);

  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de prites y texto
//Ejemplo de funciones:
//  RoJoST7735S
//    text
//  RoJoSprite
//    text
void test12() {
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint16_t color=display.getColor(0,255,0); //green
  //Creamos un sprite para dibujar el texto
  RoJoSprite sprite;
  //Escribimos un texto y lo mostramos
  sprite.print("/5x7d.fon","20191102",color);
  display.drawSprite(&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print("/10x15d.fon","20191102",color);
  display.drawSprite(&sprite,0,8);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  sprite.print("/10x15d.fon","123456",color);
  display.drawSprite(&sprite,-40,25); //No se ve el principio
  display.drawSprite(&sprite,40,25); //No se ve el final
  //Fuente más grande
  sprite.print("/21x33d.fon","12345",color);
  display.drawSprite(&sprite,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  sprite.print("/7x11.fon","Esto es una prueba!",color);
  display.drawSprite(&sprite,0,80);
  //Texto con fondo negro y bordes blancos
  sprite.print("/7x11.fon","Hola!",color,0,0xFFFF);
  display.drawSprite(&sprite,0,100);
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de cambio de tamaño
//Ejemplo de funciones:
//  RoJoST7735S
//    resize
//Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
void test13() {
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.loadSprite(F("/heart.spr"));
  //Creamos el sprite redimensionado
  RoJoSprite spriteZoom;
  for(byte z=10;z<80;z++) {
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

//Test de líneas
//Ejemplo de funciones:
//  RoJoST7735S
//    line
void test14() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite de 80x80
  RoJoSprite sprite;
  sprite.setSize(80,80);
  //Dibujamos líneas verdes
  uint16_t color = sprite.getColor(0,255,0); //Verde
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,0,x,sprite.xMax()-1,color);
  //Dibujamos líneas azules
  color = sprite.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,sprite.yMax()-1,x,0,color);
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
void test15() {
  //Creamos un sprite de 80x80
  RoJoSprite sprite;
  sprite.setSize(80,80);
  //Dibujamos un rectángulo relleno rojo
  sprite.block(10,10,50,40,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(40,20,70,60,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(20,50,60,70,display.getColor(0,0,255));
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
void test16() {
  //Creamos un sprite de 80x80
  RoJoSprite sprite;
  sprite.setSize(80,80);
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(10,20,40,10,70,40,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(60,10,50,60,20,40,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(60,40,50,70,10,60,display.getColor(0,0,255));
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
void test17() {
  //Creamos un sprite de 80x80
  RoJoSprite sprite;
  sprite.setSize(80,80);
  sprite.circle(20,20,19,display.getColor(255,0,0)); //Circunferencia
  sprite.disk(60,20,19,display.getColor(0,255,0)); //Círculo
  sprite.ellipse(20,60,19,15,display.getColor(0,0,255)); //Elipse
  sprite.ellipseFill(60,60,19,15,display.getColor(255,0,255)); //Elipse rellena
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
void test18() {
  //Creamos un sprite de 80x80
  RoJoSprite sprite;
  sprite.setSize(80,80);
  //Dibujamos una matriz de puntos
  for(byte y=0;y<sprite.yMax();y+=5)
    for(byte x=0;x<sprite.xMax();x+=5)
      sprite.drawPixel(x,y,sprite.getColor(x*2,y*2,0));
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
void test19() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite
  RoJoSprite sprite1;
  //Tendrá un tamaño de 3x3
  sprite1.setSize(3,3);
  //Lo pintamos de rojo
  sprite1.clear(display.getColor(255,0,0));
  //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.block(1,1,2,2,display.getColor(0,255,0));
  //Lo guardamos en un archivo
  sprite1.save("/test.spr");

  //Creamos un nuevo sprite
  RoJoSprite sprite2;
  //Cargamos la imágen desde un archivo
  sprite2.loadSprite("/test.spr");
  //Lo redimensionamos para que se vea claro
  sprite1.resize(&sprite2,80,80);
  //Liberamos memoria del sprite2
  sprite2.end();
  //Mostramos el sprite
  display.drawSprite(&sprite1);
  //Liberamos memoria del sprite1
  sprite1.end();
  //Borramos el archivo
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.remove("/test.spr");
  #else //Si utilizamos SPIFFS...
    SPIFFS.remove("/test.spr");
  #endif
    
  delay(1000);
}

//Test de reemplazar color y transparencia
//Ejemplo de funciones:
//  RoJoSprite
//    loadBMP
//    replaceColor
//    drawSprite
void test20() {
  display.clear();
  //Creamos sprite de fondo desde un archivo bmp
  RoJoSprite back;
  back.loadBMP("/moon.bmp");
  //Creamos sprite que irá sobre el fondo
  RoJoSprite over;
  over.setSize(25,25);
  over.clear(display.getColor(0,255,255)); //cyan
  over.block(10,10,24,24,display.getColor(0,255,0)); //Esquina inferior derecha en verde
  //Lo dibujamos sobre el sprite de fondo
  back.drawSprite(&over,5,5);
  //Sustituimos el color verde por azul
  over.replaceColor(display.getColor(0,255,0),display.getColor(0,0,255));
  //Lo volvemos a dibujar
  back.drawSprite(&over,45,5);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  back.drawSprite(&over,5,45,display.getColor(0,0,255));
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
void test21() {
  display.clear();
  RoJoSprite complete;
  complete.loadBMP("/moon.bmp");
  display.drawSprite(&complete,5,0);
  RoJoSprite quarter;
  for(int16_t y=0;y<2;y++)
    for(int16_t x=0;x<2;x++) {
      quarter.copy(&complete,x*35,y*35,x*35+34,y*35+34);
      display.drawSprite(&quarter,x*40,80+y*40);
    }
  delay(1000);
}

//Test de sincronización
//Ejemplo de funciones:
//  RoJoST7735S
//    drawSpriteSync
//  RoJoSprite
//    copy
void test22() {
  display.clear();

  RoJoSprite back; //Imagen original
  back.loadBMP("/moon.bmp");
  RoJoSprite videoMem; //Memoria de vídeo (de trabajo)
  videoMem.copy(&back);
  RoJoSprite displayMem; //Memoria del display
  displayMem.copy(&videoMem);
  display.drawSprite(&videoMem,5,40);
  //Ya tenemos todas las memorias sincronizadas

  RoJoSprite num; //Sprite en movimiento (un número)
  //Dibujamos el número en azul con fondo negro
  num.print("/21x33d.fon","52",display.getColor(0,0,255));

  //Coordenadas del sprite de texto
  int16_t x=0,y=0;
  //Dirección:
  int16_t dx=1,dy=1;
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
    display.drawSpriteSync(&videoMem,&displayMem,5,40);
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
void test23() {
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
  //Serial.begin(115200); delay(4000); //Para DEBUG
  display.begin(pinRES,pinDC,pinCS,39999999,pinCLK,pinMOSI); //Inicialización
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
  //Test de funciones de dibujo con sprites
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) //UNO o Nano no tienen memoria para trabajar con sprites
    test10(); //Test de color
    test11(); //Test de carga de sprite pequeño desde archivo
    #ifndef ARDUINO_AVR_MEGA2560 //Mega no tiene memoria para cargar sprites tan grandes
      test12(); //Test de texto
      test13(); //Test de cambio de tamaño
      #ifndef ESP8266 //ESP8266 no tiene memoria para cargar sprites grandes
        test14(); //Test de líneas
        test15(); //Test de rectángulos
        test16(); //Test de triángulos
        test17(); //Test de circunferencias y elipses
        test18(); //Test de pixels
        test19(); //Test de guardado
        test20(); //Test de reemplazar color y transparencia
        test21(); //Test de copia parcial
        test22(); //Test de sincronización
        test23(); //Test de carga de sprites grandes desde archivo
      #endif
    #endif
  #endif
}