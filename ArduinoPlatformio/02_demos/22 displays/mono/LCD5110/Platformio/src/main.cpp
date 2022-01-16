/*
  Autor: Ramón Junquera
  Fecha: 20210211
  Tema: Librería para display Nokia LCD 5110 SPI 84x48
  Objetivo: Demo de librería RoJoLCD5110
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
  
  Nota:
  - El sistema de archivos debe contener los archivos de la carpeta data para su correcto funcionamiento.
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoLCD5110.cpp> //Librería de gestión del display LCD5110

//Definimos los pines del display
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC=D2;
  const byte pinCS=D8;
  const byte pinRES=D0;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC=22;
  const byte pinCS=5;
  const byte pinRES=21;
#elif defined(ARDUINO_ARCH_AVR) //Si es un Arduino...
  const byte pinDC=5;
  const byte pinCS=4;
  const byte pinRES=3;
#elif defined(__arm__) //Si es RPi...
  const byte pinDC=25;
  const byte pinCS=8;
  const byte pinRES=24;
#endif

//Creamos objeto de gestión
RoJoLCD5110 display;
//Creamos un sprite monocromo que utilizaremos como memoria de vídeo
RoJoSprite2 v(0);

//Función drawPixel con color 1 = dibujar
void Test1() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Dibujamos una matriz de puntos
  for(uint16_t x=0;x<v.xMax();x+=5)
    for(uint16_t y=0;y<v.yMax();y+=5)
      v.drawPixel(x,y,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función drawPixel con color 0 = borrar
void Test3() {
  //Dibujamos un rectángulo relleno en el interior que borra
  for(uint16_t y=10;y<30;y++)
    for(uint16_t x=10;x<30;x++)
      v.drawPixel(x,y,0);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función getPixel
void Test4() {
  //Comprobamos los pixels de un cuadrado y dibujamos el color opuesto
  for(uint16_t y=20;y<40;y++)
    for(uint16_t x=20;x<40;x++)
      v.drawPixel(x,y,(byte)(1-v.getPixel(x,y)));
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Rectángulos
//  RoJoSprite::block,rect
void Test5() {
  //Limpiamos pantalla
  v.clear();
  //Dibujamos un rectángulo relleno sin borde (un bloque)
  v.block(0,0,20,20,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Líneas
//  RoJoSprite::line
void Test6() {
  //Limpiamos pantalla
  v.clear();
  for(byte y=0;y<v.yMax();y+=5) v.line(0,0,v.xMax()-1,y,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Círculos y elipses
void Test7() {
  //Limpiamos la memoria de vídeo
  v.clear();

  v.circle(12,12,11,1);
  v.disk(12,36,11,1);
  v.ellipse(54,12,29,11,1);
  v.ellipseFill(54,36,29,11,1);
  //Lo mostramos
  display.drawSprite(&v);

  delay(1000);
}

//Triángulos
void Test8() {
  //Limpiamos la memoria de vídeo
  v.clear();

  v.triangle(24,0,48,40,0,16,1);
  v.triangleFill(0,24,56,16,16,47,1);
  v.triangle(40,0,72,0,56,47,1);
  //Lo mostramos
  display.drawSprite(&v);

  delay(1000);
}

//Sprites definidos en programa
//  RoJoSprite::drawPage
void Test9() {
  //Limpiamos pantalla
  v.clear();
  //Creamos el sprite monocromo
  RoJoSprite2 mySprite(0);
  //Lo dimensionamos. Anchura=7. Altura=8 (una página)
  mySprite.setSize(7,8);
  //Lo dibujamos
  //bool RoJoSprite::drawPage(uint16_t x,uint16_t page,byte mask,byte color) {
  mySprite._videoMem[0][0]=0b00111110;
  mySprite._videoMem[0][1]=0b01000001;
  mySprite._videoMem[0][2]=0b01010101;
  mySprite._videoMem[0][3]=0b01010001;
  mySprite._videoMem[0][4]=0b01010101;
  mySprite._videoMem[0][5]=0b01000001;
  mySprite._videoMem[0][6]=0b00111110;

  //Lo dibujamos varias veces
  for(byte y=0;y<v.yMax();y+=8)
    for(byte x=0;x<v.xMax();x+=8)
      v.drawSprite(&mySprite,x,y);
  //Borramos el sprite utilizado
  mySprite.end();
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Leer y dibujar sprite desde un archivo
//  RoJoSprite::loadSprite,drawSprite
void Test10() {
  //Limpiamos sprite de memoria de vídeo
  v.clear();

  //Método 1. Cargamos leemos de un archivo a un nuevo sprite
  //Creamos el sprite monocromo
  RoJoSprite2 mySprite(0);
  //Lo leemos desde el archivo
  mySprite.loadSprite("/ball.spr");
  //Dibujamos el sprite en memoria de vídeo (en 0,0)
  v.drawSprite(&mySprite);
  //Borramos el sprite utilizado
  mySprite.end();

  //Método 2. Dibujamos sprite directamente sobre otro
  //Leemos archivo y lo dibujamos en 40,0
  v.drawSprite("/ball.spr",40,0);

  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Mover sprite por pantalla
void Test11() {
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+5000; //5 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite en la memoria de vídeo
    v.drawSprite(&ball,x,y);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Es necesario esperar un momento tras el dibujo, porque no se muestra instantaneamente
    //Si no lo hacemos, no podremos ver nada
    //Si esperamos demasiado poco, veremos la estela del movimiento
    delay(50);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==53) dx=-dx; //53=84-30-1
    if(y==0 || y==17) dy=-dy; //17=48-30-1
  }
  //Borramos el sprite utilizado
  ball.end();
}

//Mover sprite por pantalla manteniendo fija una imagen de fondo
void Test12() {
  //Nota: los pixel apagados del sprite en movimiento aparecen transparentes

  //Creamos un nuevo sprite monocromo para la imagen fija
  RoJoSprite2 backSprite(0);
  //Leemos su contenido desde un archivo
  //Tiene el tamaño del display
  backSprite.loadSprite("/mickey.spr");
  //Creamos un nuevo sprite monocromo para la imagen en movimiento
  RoJoSprite2 ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+5000; //5 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Dibujamos la imagen fija en la memoria de vídeo
    v.drawSprite(&backSprite);
    //Dibujamos el sprite en movimiento tomando los pixels apagados como transparentes
    v.drawSprite(&ball,x,y,0);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Esperamos un momento para evitar en lo posible el efecto "estela"
    delay(50);
    //Calculamos las nuevas coordenadas
    x+=dx; y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==53) dx=-dx; //53=84-30-1
    if(y==0 || y==17) dy=-dy; //17=48-30-1
  }
  //Borramos los sprites utilizados
  ball.end();
  backSprite.end();
}

//Mover sprite por pantalla manteniendo una imagen de fondo y que no sea transparente
void Test13() {
  //Para conseguir esto, necesitamos una imagen que nos haga de máscara
  //Esta máscara permitirá borrar antes de dibujar el sprite definitivo

  //Creamos un nuevo sprite monocromo para la imagen fija
  RoJoSprite2 backSprite(0);
  //Leemos su contenido desde un archivo
  backSprite.loadSprite("/mickey.spr");
  //Creamos un nuevo sprite monocromo para la imagen en movimiento
  RoJoSprite2 ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  //Creamos un nuevo sprite monocromo para la máscara de la imagen en movimiento
  RoJoSprite2 ballMask(0);
  //Leemos su contenido desde un archivo
  ballMask.loadSprite("/ballmask.spr"); //Su tamaño es x=30,y=30,pages=4
  ballMask.negative();
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+5000; //5 segundos
  //Bucle principal
  while(millis()<tMax)
  {
    //Dibujamos el sprite estático
    v.drawSprite(&backSprite);
    //Dibujamos la máscara del sprite en movimiento borrando
    v.drawSprite(&ballMask,x,y,1);
    //Dibujamos el sprite en movimiento
    v.drawSprite(&ball,x,y,0);
    //Esperamos un momento para evitar en lo posible el efecto "estela"
    delay(50);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx; y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==53) dx=-dx; //53=84-30-1
    if(y==0 || y==17) dy=-dy; //17=48-30-1
  }
  //Borramos los sprites utilizados
  ballMask.end();
  ball.end();
  backSprite.end();
}

//Obtener sprite de parte de otro
void Test14() {
  //Dibujamos un sprite desde un archivo
  v.drawSprite("/mickey.spr");
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 mySprite(0);
  //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.copy(&v,30,10,12,15);
  //Dibujamos el sprite varias veces
  for(byte x=0;x<v.xMax();x+=12)
    for(byte y=0;y<v.yMax();y+=15)
      v.drawSprite(&mySprite,x,y);
  //Refrescamos pantalla
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  mySprite.end();

  delay(1000);
}

//Leer un bmp
//Guardar un sprite
void Test15() {
  //Dibujamos un bmp desde un archivo en la memoria de vídeo en 0,0
  v.drawBMP("/mickey.bmp");
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 mySprite(0);
  //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.copy(&v,20,16,15,24);
  //Guardamos el sprite en un archivo
  mySprite.save("/mick2.spr");
  //Borramos el sprite utilizado
  mySprite.end();
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 mySprite2(0);
  //Leemos su contenido desde el archivo
  mySprite2.loadSprite("/mick2.spr");
  //Dibujamos el sprite varias veces en la memoria de vídeo
  for(byte y=0;y<v.yMax();y+=24)
    for(byte x=0;x<v.xMax();x+=15)
      v.drawSprite(&mySprite2,x,y);
  //Refrescamos pantalla
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  mySprite2.end();
  //Borramos el archivo
  #ifdef ROJO_PIN_CS_SD //Si se utiliza SD...
    SD.remove("/mick2.spr");
  #else //Si utilizamos SPIFFS...
    SPIFFS.remove("/mick2.spr");
  #endif

  delay(1000);
}

//Resize
void Test16() {
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 mySprite(0);
  //Leemos su contenido desde un archivo
  mySprite.loadSprite("/mickey.spr");
  //Creamos el sprite monocromo utilizado para el redimensionado
  RoJoSprite2 resizeSprite(0);
  //Borramos el display
  display.clear();
  //Ampliaremos la imagen desde un 5% hasta un 100%
  for(uint16_t r=5;r<=100;r++) {
    //Calculamos el sprite redimensionado al factor actual
    resizeSprite.resize(&mySprite,(mySprite.xMax()*r)/100,(mySprite.yMax()*r)/100);
    //Dibujamos el sprite redimensionado en el display
    display.drawSprite(&resizeSprite);
    //Esperamos un momento
    delay(40);
  }
  //Borramos los sprites utilizados
  resizeSprite.end();
  mySprite.end();

  delay(1000);
}

//Mostrar texto
void Test17() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto en el sprite
  textSprite.print("/5x7d.fon","20190829",1);
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite);
  //Utilizamos una fuente más grande que crea un texto más grande que la anchura del display
  textSprite.print("/10x15d.fon","123456789",1);
  //Dibujamos el sprite de texto en la memoria de vídeo. No se verá el final
  v.drawSprite(&textSprite,5,16);
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla. No se verá el principio
  v.drawSprite(&textSprite,-33,32);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  textSprite.end();

  delay(1000);
}

//Rotar sprites
void Test18() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto en el sprite
  textSprite.print("/5x7d.fon","123",1);
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite,30,0);
  //Creamos el sprite monocromo que contendrá el texto rotado
  RoJoSprite2 textSpriteRotate(0);
  //Rotamos el sprite de texto 90 grados
  textSpriteRotate.rotate(&textSprite,90);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,48,8);
  //Rotamos el sprite de texto 180 grados
  textSpriteRotate.rotate(&textSprite,180);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,30,26);
  //Rotamos el sprite de texto 270 grados
  textSpriteRotate.rotate(&textSprite,270);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,22,9);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSpriteRotate.end();
  textSprite.end();

  delay(2000);
}

//Flip
void Test19() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto en el sprite
  textSprite.print("/10x15d.fon","123",1);
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite);
  //Creamos el sprite monocromo que contendrá el texto volteado
  RoJoSprite2 textSpriteFlip(0);
  //Volteamos el sprite en horizontal
  textSpriteFlip.flipH(&textSprite);
  //Dibujamos el sprite de texto volteado en horizontal en la memoria de vídeo
  v.drawSprite(&textSpriteFlip,33,0);
  //Volteamos el sprite de texto en vertical
  textSpriteFlip.flipV(&textSprite);
  //Dibujamos el sprite de texto volteado en vertical en la memoria de vídeo
  v.drawSprite(&textSpriteFlip,0,16);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSpriteFlip.end();
  textSprite.end();

  delay(2000);
}

//Redimensionar texto
void Test20() {
  //Crearemos un sprite de texto que redimensionaremos al tamaño del display

  //Creamos el sprite monocromo para el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto con una fuente pequeña
  textSprite.print("/5x7d.fon","1234",1);
  //Creamos el sprite del texto redimensionado
  RoJoSprite2 textSpriteResize(0);
  //Redimensionamos el texto rotado al tamaño del display
  textSpriteResize.resize(&textSprite,display.xMax(),display.yMax());
  //Mostramos el sprite en el display
  display.drawSprite(&textSpriteResize);
  //Borramos los sprites utilizados
  textSpriteResize.end();
  textSprite.end();

  delay(1000);
}

//Mostrar texto con más fuentes
void Test21() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Escribimos con distintos tamaños de fuente
  v.printOver("/5x7.fon","Hello world!",1);
  v.printOver("/7x11.fon","Hello world!",1,0,8);
  v.printOver("/10x15.fon","Hello world!",1,0,20);
  //Para escribir con borde necesitamos crear un sprite
  RoJoSprite2 textSprite(0);
  //texto negro, fondo negro, borde blanco
  textSprite.print("/5x7.fon","Hello",0,0,1);
  v.drawSprite(&textSprite,0,38);
  textSprite.end();
  //Lo mostramos
  display.drawSprite(&v);

  delay(2000);
}

void setup() {
  //Serial.begin(115200); //DEBUG
  //Inicializamos el display
  //void begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqCOMM=0); //Inicialización
  display.begin(pinRES,pinDC,pinCS);
  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop() {
  Test1(); //Función drawPixel con color 1 = dibujar
  Test3(); //Función drawPixel con color 0 = borrar
  Test4(); //Función getPixel
  Test5(); //Rectángulos
  Test6(); //Líneas
  Test7(); //Círculos y elipses
  Test8(); //Triángulos
  Test9(); //Sprites definidos en programa.
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO)
    Test10(); //Leer sprite desde un archivo
    Test11(); //Mover sprite por pantalla
    Test12(); //Mover sprite por pantalla manteniendo una imagen de fondo
    Test13(); //Mover sprite por pantalla manteniendo una imagen de fondo no transparente
    Test14(); //Obtener sprite de parte de otro
    Test15(); //Leer un bmp y guardar un sprite
    Test16(); //Resize
    Test17(); //Mostrar texto
    Test18(); //Rotar sprites
    Test19(); //Flip
    Test20(); //Redimensionar texto
    Test21(); //Mostrar texto con más fuentes
  #endif
}
