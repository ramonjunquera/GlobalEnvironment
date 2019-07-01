/*
  Autor: Ramón Junquera
  Fecha: 20180630
  Tema: Librería para display ILI9341 SPI 240x320
  Objetivo: Demo de librería RoJoILI9341
  Material: breadboard, cables, display ILI9486, lector SD
  Ejemplos: ESP32, ESP8266, Mega, UNO, Nano, Raspberry Pi

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
    El sistema de archivos (SPIFFS/SD) debe contener los archivos de la
    carpeta data para su correcto funcionamiento.
    Para SPIFFS se pueden transferir con la tarea:
      PlatformIO: Upload File System image
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
#include <RoJoSprite16.h> //Gestión de sprites color

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D8;
  const byte pinCS_display=D3;
  //const byte pinCS_touchscreen=D1;
  //const byte pinIRQ_touchscreen=D0;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5;
  //const byte pinCS_touchscreen=16;
  //const byte pinIRQ_touchscreen=17;
#elif defined(__arm__) //Si es una Raspberry Pi
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
  //const byte pinCS_touchscreen=7;
  //const byte pinIRQ_touchscreen=17;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDC_display=5;
  const byte pinRES_display=4;
  const byte pinCS_display=3;
  //const byte pinCS_touchscreen=6;
  //const byte pinIRQ_touchscreen=2;
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
void Test1()
{
  display.clear(); //Limpiamos pantalla

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]=
  {
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 10 filas
  for(uint16_t y=0;y<10;y++)
  {
    //Recorremos las 7 columnas
    for(uint16_t x=0;x<7;x++)
    {
      //Pintamos el rectángulo relleno
      display.block(x*32,y*32,x*32+31,y*32+31,display.getColor(color[x][0],color[x][1],color[x][2]));
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
void Test2()
{
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
//  RoJoILI9341
//    line
void Test3()
{
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint16_t color = display.getColor(0,255,0); //verde
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10)
  {
    display.line(0,0,x,display.yMax()-1,color);
    #ifdef ESP8266
      yield();
    #endif
  } 
  //Dibujamos líneas azules
  color = display.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)display.xMax();x+=10)
  {
	  display.line(0,display.yMax()-1,x,0,color);
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
void Test4()
{
  //Limpiamos display
  display.clear();
  //Dibujamos un rectángulo relleno rojo
  display.block(40,40,120,160,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.rect(80,120,200,240,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.block(40,200,160,280,display.getColor(0,0,255));
  
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoILI9341
//    triangle
//    triangleFill
void Test5()
{
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
void Test6()
{
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
void Test7()
{
  display.clear(); //Limpiamos display

  //Dibujamos el sprite como una matriz de 7x10
  for(uint16_t y=0;y<10;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) //De izquierda a derecha
    {
      display.drawSprite(x*32,y*32,"/heart.s16");
      #ifdef ESP8266
        yield();
      #endif
    }
  //Esperamos
  delay(1000);

  //Mostramos varias imágenes de 240x320
  //Dibujamos imagen directamente desde archivo bmp
  display.drawBMP(0,0,"/moras2.bmp");
  delay(1000);
  //Dibujamos imágenes directamente desde archivo de sprite
  display.drawSprite(0,0,"/lapices.s16");
  delay(1000);
  display.drawSprite(0,0,"/moras1.s16");
  delay(1000);
}

//Test de texto
//Ejemplo de funciones:
//  RoJoILI9341
//    print
void Test8()
{
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  display.print(0,0,"/5x7d.fon","20190528",color);
  //Utilizaremos otra fuente más grande
  display.print(0,8,"/10x15d.fon","20190530",color);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.print(-35,25,"/10x15d.fon","1234567890",color); //No se ve el principio
  display.print(280,25,"/10x15d.fon","1234567890",color); //No se ve el final
  //Fuente más grande
  display.print(0,45,"/21x33d.fon","1234567890",color);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.print(0,80,"/7x11.fon","Hola, esto es una prueba!",color);

  delay(2000);
}

//Test de rotación de pantalla
//Ejemplo de funciones:
//  RoJoILI9341
//    rotation
void Test9()
{
  //Limpiamos pantalla
  display.clear();

  //Dibujamos el texto para cada rotación
  display.rotation(0);
  display.print(0,0,F("/7x11.fon"),F("Rotation 0"),0xFFFF);
  display.rotation(1);
  display.print(0,0,F("/7x11.fon"),F("Rotation 1"),0xFFFF);
  display.rotation(2);
  display.print(0,0,F("/7x11.fon"),F("Rotation 2"),0xFFFF);
  display.rotation(3);
  display.print(0,0,F("/7x11.fon"),F("Rotation 3"),0xFFFF);
  display.rotation(0);

  delay(2000);
}

//Test de color
//Ejemplo de funciones:
//  RoJoILI9341
//    clear
//    drawSprite
//  RoJoSprite16
//    setSize
//    end
//Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
//en pantalla, formando una matriz de 3x3
void Test10()
{
  display.clear(); //Limpiamos pantalla
  RoJoSprite16 sprite; //Creamos el sprite
  sprite.setSize(32,32); //Lo dimensionamos

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]=
  {
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 10 filas
  for(uint16_t y=0;y<10;y++)
  {
    //Recorremos las 7 columnas
    for(uint16_t x=0;x<7;x++)
    {
      //Pintamos el sprite
      sprite.clear(sprite.getColor(color[x][0],color[x][1],color[x][2]));
      //Dibujamos el sprite
      display.drawSprite(x*32,y*32,&sprite);
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
//  RoJoSprite16
//    load
//    loadBMP
void Test11()
{
  //Creamos el sprite
  RoJoSprite16 sprite;
  //Dibujamos el sprite como una matriz de 7x10
  //En las 5 primeras filas el sprite se ha cargado desde un .s16
  //En las 5 últimas desde un .bmp

  //Cargamos una imagen de 32x32 desde un archivo .s16
  sprite.load(F("/heart.s16"));
  //Dibujamos las 5 primeras filas
  for(uint16_t y=0;y<5;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,&sprite);
  
  //Cargamos la imágen del sprite desde un archivo .bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos las 5 últimas filas
  for(uint16_t y=5;y<10;y++) //Se arriba a abajo
    for(uint16_t x=0;x<7;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,&sprite);

  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de prites y texto
//Ejemplo de funciones:
//  RoJoILI9341
//    text
//  RoJoSprite16
//    text
void Test12()
{
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Creamos un sprite para dibujar el texto
  RoJoSprite16 sprite;
  //Escribimos un texto y lo mostramos
  sprite.print("/5x7d.fon","20190528",color);
  display.drawSprite(0,0,&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print("/10x15d.fon","20190530",color);
  display.drawSprite(0,8,&sprite);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  sprite.print("/10x15d.fon","1234567890",color);
  display.drawSprite(-35,25,&sprite); //No se ve el principio
  display.drawSprite(280,25,&sprite); //No se ve el final
  //Fuente más grande
  sprite.print("/21x33d.fon","1234567890",color);
  display.drawSprite(0,45,&sprite);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  sprite.print("/7x11.fon","Hola, esto es una prueba!",color);
  display.drawSprite(0,80,&sprite);
  //Texto con fondo negro y bordes blancos
  sprite.print("/7x11.fon","Hola!",color,0,0xFFFF);
  display.drawSprite(0,100,&sprite);
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de cambio de tamaño
//Ejemplo de funciones:
//  RojoSprite16
//    resize
//Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
void Test13()
{
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite16 spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.load(F("/heart.s16"));
  //Creamos el sprite redimensionado
  RoJoSprite16 spriteZoom;
  for(byte z=32;z<150;z+=5)
  {
    //Redimensionamos el sprite
    spriteZoom.resize(z,z,&spriteOri);
    //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-spriteZoom.xMax())/2,(display.yMax()-spriteZoom.yMax())/2,&spriteZoom);
  }
  //Liberamos memoria del sprite redimensionado
  spriteZoom.end();
  //Liberamos memoria del sprite original
  spriteOri.end();

  delay(500);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoSprite16
//    line
void Test14()
{
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite de 200x200
  RoJoSprite16 sprite;
  sprite.setSize(200,200);
  //Dibujamos líneas verdes
  uint16_t color = sprite.getColor(0,255,0); //Verde
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,0,x,sprite.xMax()-1,color);
  //Dibujamos líneas azules
  color = sprite.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,sprite.yMax()-1,x,0,color);
  //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoSprite16
//    block
//    rect
void Test15()
{
  //Creamos un sprite de 200x200
  RoJoSprite16 sprite;
  sprite.setSize(200,200);
  //Dibujamos un rectángulo relleno rojo
  sprite.block(20,20,140,140,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(40,60,160,160,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(80,100,180,180,display.getColor(0,0,255));
  //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoSprite16
//    triangle
//    triangleFill
void Test16()
{
  //Creamos un sprite de 200x200
  RoJoSprite16 sprite;
  sprite.setSize(200,200);
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(80,0,100,120,0,60,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(20,100,160,40,100,199,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(100,20,199,0,140,160,display.getColor(0,0,255));
  //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de circunferencias y círculos
//Ejemplo de funciones:
//  RoJoSprite16
//    circle
//    disk
//    ellipse
//    ellipseFill
void Test17()
{
  //Creamos un sprite de 200x200
  RoJoSprite16 sprite;
  sprite.setSize(200,200);
  sprite.circle(40,40,39,display.getColor(255,0,0)); //Circunferencia
  sprite.disk(40,120,39,display.getColor(0,255,0)); //Círculo
  sprite.ellipse(140,40,59,39,display.getColor(0,0,255)); //Elipse
  sprite.ellipseFill(140,120,59,39,display.getColor(255,0,255)); //Elipse rellena
  //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoSprite16
//    drawPixel
void Test18()
{
  //Creamos un sprite de 200x200
  RoJoSprite16 sprite;
  sprite.setSize(200,200);
  //Dibujamos una matriz de puntos
  for(byte y=0;y<sprite.yMax();y+=10)
    for(byte x=0;x<sprite.xMax();x+=10)
      sprite.drawPixel(x,y,sprite.getColor(x,y,0));
  //Mostramos el sprite centrado
  display.drawSprite((display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de guardado
//Ejemplo de funciones:
//  RoJoSprite16
//    save
void Test19()
{
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite
  RoJoSprite16 sprite1;
  //Tendrá un tamaño de 3x3
  sprite1.setSize(3,3);
  //Lo pintamos de rojo
  sprite1.clear(display.getColor(255,0,0));
  //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.block(1,1,2,2,display.getColor(0,255,0));
  //Lo guardamos en un archivo
  sprite1.save("/test.s16");

  //Creamos un nuevo sprite
  RoJoSprite16 sprite2;
  //Cargamos la imágen desde un archivo
  sprite2.load("/test.s16");
  //Lo redimensionamos para que se vea claro
  sprite1.resize(100,100,&sprite2);
  //Liberamos memoria del sprite2
  sprite2.end();
  //Mostramos el sprite
  display.drawSprite(0,0,&sprite1);
  //Liberamos memoria del sprite1
  sprite1.end();
  //Borramos el archivo
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.remove("/test.s16");
  #else //Si utilizamos SPIFFS...
    SPIFFS.remove("/test.s16");
  #endif
    
  delay(1000);
}

//Test de carga de sprites grandes desde archivo
//Ejemplo de funciones:
//  RoJoSprite16
//    load
//    loadBMP
void Test20()
{
  //Creamos el sprite
  RoJoSprite16 sprite;
  //Utilizaremos sprites de tamaño 240x320, igual que las dimensiones del display
  sprite.load("/lapices.s16");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.loadBMP("/moras2.bmp");
  display.drawSprite(0,0,&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de reemplazar color y transparencia
//Ejemplo de funciones:
//  RoJoSprite16
//    replaceColor
//    drawSprite
void Test21()
{
  //Creamos un sprite para el fondo
  RoJoSprite16 spriteBack;
  //Cargamos una imágen tan grande como el display
  spriteBack.load("/lapices.s16");
  //Creamos un sprite de 90x90
  RoJoSprite16 sprite;
  sprite.setSize(90,90);
  //Lo pintamos de rojo
  sprite.clear(display.getColor(255,0,0));
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(30,30,89,89,display.getColor(0,255,0));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(10,10,&sprite);
  //Sustituimos el color verde por azul
  sprite.replaceColor(display.getColor(0,255,0),display.getColor(0,0,255));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(10,110,&sprite);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(10,210,&sprite,display.getColor(0,0,255));
  //Lo mostramos
  display.drawSprite(0,0,&spriteBack);
  //Liberamos memoria de los sprites
  sprite.end();
  spriteBack.end();
    
  delay(1000);
}

//Test de copia parcial
//Ejemplo de funciones:
//  RoJoSprite16
//    copy
void Test22()
{
  //Limpiamos display
  display.clear();
  //Creamos un sprite con una imágen del tamaño del display
  RoJoSprite16 back;
  back.load("/lapices.s16");
  //Creamos un sprite para guardar un recorte del primero
  RoJoSprite16 half;
  //Copiamos al nuevo sprite sólo el cuarto superior izquierdo
  half.copy(&back,0,0,back.xMax()/2,back.yMax()/2);
  //Mostramos el sprite recortado
  display.drawSprite(0,0,&half);
  //Liberamos memoria de los sprites
  half.end();
  back.end();

  delay(1000);
}

//Test de sincronización
//Ejemplo de funciones:
//  RoJoILI9341
//    drawSpriteSync
//  RojoSprite16
//    copy
void Test23()
{
  //Se moverá un número por el display
  //El fondo estará fijo y será una imágen
  //Crearemos un sprite para el fondo y otro más pequeño para el número
  //Crearemos dos sprites más. Uno como memoria de vídeo de trabajo
  //y otro como memoria de vídeo del display.
  //La imágen final se compone sobre la memoria de vídeo de trabajo
  //Después se sincroniza con la memoria de vídeo del display y conseguimos
  //que sólo se envíen las diferencias.

  //Creamos el sprite con la imágen de fondo. Mismo tamaño que el display
  RoJoSprite16 back;
  back.load("/moras1.s16");
  //Creamos sprite con el número
  RoJoSprite16 num;
  //Color de texto: azul. Fondo: blanco. Borde: negro
  num.print("/21x33d.fon","37",display.getColor(0,0,255),0xFFFF,0);
  //Creamos sprite con memoria de vídeo de trabajo y copiamos la imágen de fondo
  RoJoSprite16 workMem;
  workMem.copy(&back);
  //Creamos sprite con memoria de vídeo de display y copiamos la imágen de fondo
  RoJoSprite16 displayMem;
  displayMem.copy(&back);
  //Para que la memoria de vídeo del display coincida con la realidad, la
  //dibujamos en el display
  display.drawSprite(0,0,&displayMem);
  //Desplazamiento

  //Coordenadas del sprite de texto
  int16_t x=0;
  int16_t y=0;
  //Dirección:
  int16_t dx=5;
  int16_t dy=5;
  //Coordenada máximas para rebote
  int16_t xBounce=display.xMax()-num.xMax();
  int16_t yBounce=display.yMax()-num.yMax();
  //Tiempo límite (duración = 5s)
  uint32_t maxTime=millis()+30000;
  //Mientras no superemos el tiempo límite
  while(millis()<maxTime)
  {
    //Calculamos nuevas coordenadas
    x+=dx;
    y+=dy;
    //Si hemos llegado a algún borde...cambiamos de dirección
    if(x<=0 || x>=xBounce) dx=-dx;
    if(y<=0 || y>=yBounce) dy=-dy;
    //Dibujamos la imágen de fondo sobre el sprite de trabajo
    workMem.drawSprite(0,0,&back);
    //Añadimos el número sobre el fondo, haciendo su fondo blanco transparente
    workMem.drawSprite(x,y,&num,0xFFFF);
    //Sincronizamos los sprites con las memorias de vídeo y enviamos las
    //diferencias al display
    display.drawSpriteSync(0,0,&workMem,&displayMem);
  }
  //Liberamos memoria de los sprites
  displayMem.end();
  workMem.end();
  num.end();
  back.end();
}

void setup()
{
  Serial.begin(115200); //Para DEBUG
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
}

void loop()
{
  //Tests de funciones de dibujo directo desde el driver
  Test1(); //Test de block
  Test2(); //Test de pixels
  Test3(); //Test de líneas
  Test4(); //Test de rectángulos
  Test5(); //Test de triángulos
  Test6(); //Test de circunferencias y elipses
  Test7(); //Test de carga de sprite desde archivo
  Test8(); //Test de texto
  Test9(); //Test de rotación de pantalla
  //Test de funciones de dibujo con sprites
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) //UNO o Nano no tienen memoria para trabajar con sprites
    Test10(); //Test de color
    Test11(); //Test de carga de sprite pequeño desde archivo
    #ifndef ARDUINO_AVR_MEGA2560 //Mega no tiene memoria para cargar sprites tan grandes
      Test12(); //Test de texto
      Test13(); //Test de cambio de tamaño
      #ifndef ESP8266 //ESP8266 no tiene memoria para cargar sprites grandes
        Test14(); //Test de líneas
        Test15(); //Test de rectángulos
        Test16(); //Test de triángulos
        Test17(); //Test de circunferencias y elipses
        Test18(); //Test de pixels
        Test19(); //Test de guardado
        #ifndef ESP32 //ESP32 no tiene memoria para cargar sprites de pantalla completa
          Test20(); //Test de carga de sprites grandes desde archivo
          Test21(); //Test de reemplazar color y transparencia
          Test22(); //Test de copia parcial
          Test23(); //Test de sincronización
        #endif
      #endif
    #endif
  #endif
}
