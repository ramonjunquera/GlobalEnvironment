#include <Arduino.h>
#include <RoJoSpriteQtLabel.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

//Test de color
void MainWindow::test01() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3,2); //Init 135x240 24bits zoom x2
  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[5][3]={
    {255,255,255}, //col 0 = blanco
    {255,0,0},     //col 1 = red
    {0,255,0},     //col 2 = green
    {0,0,255},     //col 3 = blue
    {255,255,0}    //col 4 = yellow
  };
  for(uint16_t y=0;y<8;y++) { //Recorremos filas
    for(uint16_t x=0;x<5;x++) { //Recorremos columnas
      //Pintamos el rectángulo relleno
      display.s.block(x*27,y*30,27,30,display.s.getColor(color[x][0],color[x][1],color[x][2]));
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*2/3;
    }
  }
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de pixels
void MainWindow::test02() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3); //Init 135x240 24bits
  //Dibujamos una matriz de puntos
  for(uint32_t y=0;y<display.yMax();y+=5)
    for(uint32_t x=0;x<display.xMax();x+=5) {
      display.s.drawPixel(x,y,x*y);
    }
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de líneas
void MainWindow::test03() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3); //Init 135x240 24bits
  //Dibujamos líneas verdes
  uint32_t color=display.s.getColor(0,255,0); //verde
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10) {
    display.s.line(0,0,x,display.yMax()-1,color);
  }
  //Dibujamos líneas azules
  color=display.s.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10) {
    display.s.line(0,display.yMax()-1,x,0,color);
  }
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de rectángulos
void MainWindow::test04() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3); //Init 135x240 24bits
  //Dibujamos un rectángulo relleno rojo
  display.s.block(16,15,85,60,display.s.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.s.rect(50,45,68,135,display.s.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.s.block(33,135,51,75,display.s.getColor(0,0,255));
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de triángulos
void MainWindow::test05() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3,2); //Init 135x240 24bits zoom x2
  //Dibujamos un triángulo relleno rojo
  display.s.triangleFill(45,15,118,105,16,75,display.s.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  display.s.triangle(118,45,84,165,33,120,display.s.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  display.s.triangleFill(118,120,90,225,16,150,display.s.getColor(0,0,255));
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de circunferencias y elipses
void MainWindow::test06() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3); //Init 135x240 24bits
  display.s.circle(33,33,32,display.s.getColor(255,0,0)); //Circunferencia
  display.s.disk(100,33,32,display.s.getColor(0,255,0)); //Círculo
  display.s.ellipse(33,153,32,86,display.s.getColor(0,0,255)); //Elipse
  display.s.ellipseFill(100,153,32,86,display.s.getColor(255,0,255)); //Elipse rellena
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de carga de sprite desde archivo
void MainWindow::test07() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2,2); //Init 135x240 16bits zoom x2

  //Dibujamos el sprite como una matriz de 4x7
  for(uint16_t y=0;y<7;y++) //Se arriba a abajo
    for(uint16_t x=0;x<4;x++) { //De izquierda a derecha
      display.s.drawSprite("/heart.spr",x*32,y*32);
    }
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);

  //Mostramos varias imágenes de 135x240
  //Dibujamos imagen directamente desde archivo bmp
  display.s.drawBMP("/colores.bmp");
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);
  //Dibujamos imágenes directamente desde archivo de sprite
  display.s.drawSprite("/lago1.spr");
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);
  display.s.drawSprite("/canicas2.spr");
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de texto
void MainWindow::test08() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,3,2); //Init 135x240 24bits zoom x2
  //Definimos color de texto
  uint32_t color=display.s.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  display.s.printOver("/5x7d.fon","20191102",color);
  //Utilizaremos otra fuente más grande
  display.s.printOver("/10x15d.fon","20191102",color,0,8);
  //Cambiamos el color del texto
  color=display.s.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.s.printOver("/10x15d.fon","123456",color,-30,25); //No se ve el principio
  display.s.printOver("/10x15d.fon","123456",color,80,25); //No se ve el final
  //Fuente más grande
  display.s.printOver("/21x33d.fon","12345",color,0,45);
  //Cambiamos el color
  color=display.s.getColor(255,0,0); //red
  display.s.printOver("/7x11.fon","Esto es una prueba!",color,0,80);
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(2000);
}

//Test de color con sprites
void MainWindow::test09() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2); //Init 135x240 16bits

  RoJoSprite sprite; //Creamos el sprite de 16bits
  sprite.setSize(27,30); //Lo dimensionamos

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[5][3]={
    {255,255,255}, //col 0 = blanco
    {255,0,0},     //col 1 = red
    {0,255,0},     //col 2 = green
    {0,0,255},     //col 3 = blue
    {255,255,0}    //col 4 = yellow
  };

  for(uint16_t y=0;y<8;y++) { //Recorremos filas
    for(uint16_t x=0;x<5;x++) { //Recorremos columnas
      //Pintamos el sprite
      sprite.clear(sprite.getColor(color[x][0],color[x][1],color[x][2]));
      //Dibujamos el sprite
      display.s.drawSprite(&sprite,x*27,y*30);
      //Cambiamos el color para el próximo ciclo
      for(byte i=0;i<3;i++) color[x][i]=color[x][i]*2/3;
    }
  }
  display.draw(); //Mostramos sprite en QLabel
  sprite.end(); //Liberamos memoria del sprite
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
void MainWindow::test10() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2); //Init 135x240 16bits

  RoJoSprite sprite; //Creamos el sprite de 16bits
  //Dibujamos el sprite como una matriz de 4x7
  //En las 4 primeras filas el sprite se cargará desde un .spr
  //En las 3 últimas desde un .bmp

  //Cargamos una imagen de 32x32 desde un archivo .spr
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos las 4 primeras filas
  for(uint16_t y=0;y<4;y++) //Se arriba a abajo
    for(uint16_t x=0;x<4;x++) //De izquierda a derecha
      display.s.drawSprite(&sprite,x*32,y*32);

  //Cargamos la imágen del sprite desde un archivo .bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos las 3 últimas filas
  for(uint16_t y=4;y<7;y++) //Se arriba a abajo
    for(uint16_t x=0;x<4;x++) //De izquierda a derecha
      display.s.drawSprite(&sprite,x*32,y*32);

  display.draw(); //Mostramos sprite en QLabel
  sprite.end(); //Liberamos memoria del sprite
  display.end(); //Liberamos memoria de display
  delay(1000);
}

//Test de sprites y texto
void MainWindow::test11() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2,2); //Init 135x240 16bits zoom x2

  //Creamos un sprite para dibujar el texto
  RoJoSprite sprite;
  //Definimos color de texto
  uint32_t color=sprite.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  sprite.print("/5x7d.fon","20191102",color);
  display.s.drawSprite(&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print("/10x15d.fon","20191102",color);
  display.s.drawSprite(&sprite,0,8);
  //Cambiamos el color del texto
  color=sprite.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  sprite.print("/10x15d.fon","123456",color);
  display.s.drawSprite(&sprite,-40,25); //No se ve el principio
  display.s.drawSprite(&sprite,80,25); //No se ve el final
  //Fuente más grande
  sprite.print("/21x33d.fon","12345",color);
  display.s.drawSprite(&sprite,0,45);
  //Cambiamos el color
  color=sprite.getColor(255,0,0); //red
  sprite.print("/7x11.fon","Esto es una prueba!",color);
  display.s.drawSprite(&sprite,0,80);
  //Texto con fondo negro y bordes blancos
  sprite.print("/7x11.fon","Hola!",color,0,sprite.getColor(255,255,255));
  display.s.drawSprite(&sprite,0,100);

  display.draw(); //Mostramos sprite en QLabel
  sprite.end(); //Liberamos memoria del sprite
  display.end(); //Liberamos memoria de display
  delay(3000);
}

//Test de cambio de tamaño
//Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
void MainWindow::test12() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2); //Init 135x240 16bits

  //Creamos el sprite original
  RoJoSprite spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.loadSprite("/heart.spr");
  //Creamos el sprite redimensionado
  RoJoSprite spriteZoom;
  for(byte z=10;z<80;z++) {
    //Redimensionamos el sprite
    spriteZoom.resize(&spriteOri,z,z);
    //Mostramos el sprite centrado
    display.s.drawSprite(&spriteZoom,(display.xMax()-spriteZoom.xMax())/2,(display.yMax()-spriteZoom.yMax())/2);
    display.draw(); //Mostramos sprite en QLabel
  }
  spriteZoom.end(); //Liberamos memoria del sprite redimensionado
  spriteOri.end(); //Liberamos memoria del sprite original
  display.end(); //Liberamos memoria de display
  delay(500);
}

//Test de copia parcial
void MainWindow::test13() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,135,240,2,2); //Init 135x240 16bits zoom x2

  RoJoSprite complete;
  complete.loadBMP("/moon.bmp");
  display.s.drawSprite(&complete,5,0);
  RoJoSprite quarter;
  for(int16_t y=0;y<2;y++)
    for(int16_t x=0;x<2;x++) {
      quarter.copy(&complete,x*35,y*35,x*35+34,y*35+34);
      display.s.drawSprite(&quarter,x*40,80+y*40);
    }
  display.draw(); //Mostramos sprite en QLabel
  quarter.end();
  complete.end();
  display.end();
  delay(1000);
}

//Test de carga de sprite gris
void MainWindow::test14() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,1,1,1); //Init 1x1 8bits (grises)

  //No importa el tamaño del sprite. Al cargar el archivo se redimensiona
  display.s.loadSprite("/mountain.spr");
  display.draw(); //Mostramos sprite en QLabel
  display.end(); //Liberamos memoria de display
  delay(2000);
}

//Test de carga de sprite b/w
void MainWindow::test15() {
  RoJoSpriteQtLabel display; //Objeto display
  display.begin(ui->spriteLabel,1,1,0,3); //Init 1x1 1bit (b/w) zoom x3

  //No importa el tamaño del sprite. Al cargar el archivo se redimensiona
  display.s.loadSprite("/mickey.spr");
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);

  display.s.loadSprite("/ball.spr");
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);

  display.s.loadBMP("/mickey.bmp");
  display.draw(); //Mostramos sprite en QLabel
  delay(1000);

  display.end(); //Liberamos memoria de display
}

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->show(); //Nos aseguramos que la ventana se muestra
  //Ejecutamos en bucle todos los tests
  while(1) {
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
  }
}

MainWindow::~MainWindow() {
  ArduinoEnd(); //Finalizamos formalmente el uso de la librería
  delete ui;
}

