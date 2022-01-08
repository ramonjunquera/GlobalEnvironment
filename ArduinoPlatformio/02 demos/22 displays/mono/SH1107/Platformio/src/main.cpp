/*
  Autor: Ramón Junquera
  Fecha: 20210212
  Tema: Librería para display SH1107 SPI 64x128
  Objetivo: Demo de librería RoJoSH1107

  Descripción:
    Descripción y demostración de funcionalidades de la librería.

  Notas:
  - La memoria interna debe contener los archivos de la carpeta data para su correcto funcionamiento.
  - Este modelo de display no permite dibujar directamente en pantalla, pero sí se puede dibujar
    un sprite sobre ella. Para la mayoría de los ejemplos utilizarmos un sprite con las mismas
    dimensiones del display (64x128) en el que dibujaremos para después volcarlo en el display.
    Este sprite lo llamaremos 'v' (de memoria de vídeo) y lo definiremos globalmente.
  
  Resultado:
    Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoSH1107.h>

//Creamos objeto de gestión de display
RoJoSH1107 display;
//Creamos un sprite monocromo que utilizaremos como memoria de vídeo
RoJoSprite2 v(0);

//Función drawPixel con color 1 = dibujar
void test01() {
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
void test03() {
  //Dibujamos un rectángulo relleno en el interior que borra
  for(uint16_t y=10;y<40;y++)
    for(uint16_t x=10;x<40;x++)
      v.drawPixel(x,y,0);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Función getPixel
void test04() {
  //Comprobamos los pixels de un cuadrado y dibujamos el color opuesto
  for(uint16_t y=30;y<50;y++)
    for(uint16_t x=30;x<50;x++)
      v.drawPixel(x,y,(1-v.getPixel(x,y)));
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Rectángulos
//  RoJoSprite::block,rect
void test05() {
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
void test06() {
  //Limpiamos pantalla
  v.clear();
  for(byte y=0;y<v.yMax();y+=10) v.line(0,0,v.xMax()-1,y,1);
  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Sprites definidos en programa
//  RoJoSprite::drawPage
void test07() {
  //Limpiamos pantalla
  v.clear();
  //Creamos el sprite monocromo
  RoJoSprite2 mySprite(0);
  //Lo dimensionamos. Anchura=7. Altura=8 (una página)
  mySprite.setSize(7,8);
  //Lo dibujamos
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
//  RoJoSH1107::reverse
void test08() {
  //En el sprite que vamos a leer el dibujo lo hacen los pixels activos
  //En dibujo original estaba hecho en negro sobre fondo blanco
  //Es justo al revés de lo que muestra el display por defecto
  //Lo normal es que una pantalla vacía sera de color negro
  //Por eso activaremos el modo inverso
  display.negative(true);

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
  //Leemos archivo y lo dibujamos en 0,64
  v.drawSprite("/ball.spr",0,64);

  //Mostramos el resultado
  display.drawSprite(&v);

  delay(1000);
}

//Mover sprite por pantalla
void test09() {
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+3000; //3 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite en la memoria de vídeo
    v.drawSprite(&ball,x,y);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==33) dx=-dx; //33=64-30-1
    if(y==0 || y==97) dy=-dy; //97=128-30-1
  }
  //Borramos el sprite utilizado
  ball.end();
}

//Sincronización de sprites
void test10() {
  //La función de sincronización de sprites permite usar la técnica del dóble buffer
  //de memoria de vídeo. Ambas del mismo tipo y tamaño, el mismo que el display.
  //Una de las memoria será la de trabajo que podremos leer o escribir.
  //La otra será la memoria de vídeo del display.
  //Cuando sincronizacmos ambos sprites, se comparan y se aplican los cambios encontrados
  //a la memoria de vídeo del display al mismo tiempo que se envían al display.
  //Con la ventaja de que sólo se enviarán las zonas que han cambiado, acelerando
  //la velocidad de escritura
  //En este ejempl tomaremos v como la memoria de vídeo de trabajo, como hemos estado
  //haciendo hasta ahora, y crearemos uno nuevo llamado videoMem para la memoria de
  //vídeo del display.

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
  //Limpiamos la memoria de vídeo de trabajo
  v.clear();
  //Creamos el sprite de memoria de vídeos del display
  RoJoSprite2 videoMem(0);
  //Le damos el mismo tamaño que el display
  //Ya se crea limpio
  videoMem.setSize(display.xMax(),display.yMax());
  //Borramos el display
  display.clear();
  //Tenemos las 3 memorias vacías (sincronizadas)

  //Repetimos el ejemplo anterior de mover un sprite por pantalla
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/ball.spr"); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+3000; //3 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Borramos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite en la memoria de vídeo
    v.drawSprite(&ball,x,y);
    //Refrescamos pantalla
    display.drawSpriteSync(&v,&videoMem);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==33) dx=-dx; //33=64-30-1
    if(y==0 || y==97) dy=-dy; //97=128-30-1
  }
  //Borramos el sprite utilizado
  ball.end();
  videoMem.end();
}

//Mover sprite por pantalla manteniendo fija una imagen de fondo
void test11() {
  //Nota: los pixel apagados del sprite en movimiento aparecen transparentes

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
  //Creamos un nuevo sprite monocromo para la imagen fija
  RoJoSprite2 backSprite(0);
  //Leemos su contenido desde un archivo
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
  uint32_t tMax=millis()+3000; //3 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Limpiamos la memoria de vídeo
    v.clear();
    //Dibujamos la imagen fija en la memoria de vídeo
    //Más o menos centrada
    //Forzamos que y sea divisible entre 8 para que utilice el método de dibujo
    //por páginas, que es mucho más rápido que pixel a pixel
    v.drawSprite(&backSprite,4,32);
    //Dibujamos el sprite en movimiento tomando los pixels apagados como transparentes
    v.drawSprite(&ball,x,y,0);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx, y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==33) dx=-dx; //33=64-30-1
    if(y==0 || y==97) dy=-dy; //97=128-30-1
  }
  //Borramos los sprites utilizados
  ball.end();
  backSprite.end();
}

//Mover sprite por pantalla manteniendo una imagen de fondo y que no sea transparente
void test12() {
  //Para conseguir esto, necesitamos una imagen que nos haga de máscara
  //Esta máscara permitirá borrar antes de dibujar el sprite definitivo

  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
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
  uint32_t tMax=millis()+3000; //3 segundos
  //Bucle principal
  while(millis()<tMax) {
    //Limpiamos la memoria de vídeo
    v.clear();
    //Dibujamos el sprite estático centrado
    v.drawSprite(&backSprite,4,32);
    //Dibujamos la máscara del sprite en movimiento borrando
    v.drawSprite(&ballMask,x,y,1);
    //Dibujamos el sprite en movimiento
    v.drawSprite(&ball,x,y,0);
    //Refrescamos pantalla
    display.drawSprite(&v);
    //Calculamos las nuevas coordenadas
    x+=dx; y+=dy;
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==33) dx=-dx; //33=64-30-1
    if(y==0 || y==97) dy=-dy; //97=128-30-1
  }
  //Borramos los sprites utilizados
  ballMask.end();
  ball.end();
  backSprite.end();
}

//Obtener sprite de parte de otro
void test13() {
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
  //Limpiamos la memoria de vídeo
  v.clear();
  //Dibujamos un sprite desde un archivo
  v.drawSprite("/mickey.spr");
  //Creamos un nuevo sprite monocromo
  RoJoSprite2 mySprite(0);
  //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.copy(&v,0,0,32,40);
  //Dibujamos el sprite varias veces
  for(byte x=0;x<v.xMax();x+=32)
    for(byte y=0;y<v.yMax();y+=40)
      v.drawSprite(&mySprite,x,y);
  //Refrescamos pantalla
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  mySprite.end();

  delay(1000);
}

//Leer un bmp
//Guardar un sprite
void test14() {
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
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
void test15() {
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
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
    delay(10);
  }
  //Borramos los sprites utilizados
  resizeSprite.end();
  mySprite.end();

  delay(1000);
}

//Mostrar texto
void test16() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Desactivamos el modo inverso (pixels en blanco)
  display.negative(false);
  
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto en el sprite
  textSprite.print("/5x7d.fon","20190829",1);
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite);
  //Utilizamos una fuente más grande que crea un texto más grande que la anchura del display
  textSprite.print("/10x15d.fon","123456",1);
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
void test17() {
  //Limpiamos la memoria de vídeo
  v.clear();
  
  //Creamos el sprite monocromo que contendrá el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto en el sprite
  textSprite.print("/10x15d.fon","123",1);
  //Dibujamos el sprite de texto en la memoria de vídeo
  v.drawSprite(&textSprite,15,0);
  //Creamos el sprite monocromo que contendrá el texto rotado
  RoJoSprite2 textSpriteRotate(0);
  //Rotamos el sprite de texto 90 grados
  textSpriteRotate.rotate(&textSprite,90);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,45,16);
  //Rotamos el sprite de texto 180 grados
  textSpriteRotate.rotate(&textSprite,180);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,15,46);
  //Rotamos el sprite de texto 270 grados
  textSpriteRotate.rotate(&textSprite,270);
  //Dibujamos el sprite de texto rotado en la memoria de vídeo
  v.drawSprite(&textSpriteRotate,0,16);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos los sprites utilizados
  textSpriteRotate.end();
  textSprite.end();

  delay(1000);
}

//Flip
void test18() {
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

  delay(1000);
}

//Redimensionar texto
void test19() {
  //Crearemos un sprite de texto
  //Lo rotaremos 90 grados para que se adapte al display
  //Lo redimensionaremos al tamaño del display
  //Lo mostraremos

  //Creamos el sprite monocromo para el texto
  RoJoSprite2 textSprite(0);
  //Escribimos el texto con una fuente pequeña
  textSprite.print("/5x7d.fon","1234",1);
  //Creamos el sprite monocromo para el texto rotado
  RoJoSprite2 textSpriteRotate(0);
  //Rotamos el texto
  textSpriteRotate.rotate(&textSprite,90);
  //Ya no necesitamos el sprite de texto
  textSprite.end();
  //Creamos el sprite del texto redimensionado
  RoJoSprite2 textSpriteResize(0);
  //Redimensionamos el texto rotado al tamaño del display
  textSpriteResize.resize(&textSpriteRotate,display.xMax(),display.yMax());
  //Mostramos el sprite en el display
  display.drawSprite(&textSpriteResize);
  //Borramos los sprites utilizados
  textSpriteResize.end();
  textSprite.end();

  delay(1000);
}

//Mostrar texto con más fuentes
void test20() {
  //Limpiamos la memoria de vídeo
  v.clear();
  //Escribimos con distintos tamaños de fuente
  v.printOver("/3x5.fon","Hello world!",1);
  v.printOver("/5x7.fon","Hello world!",1,0,8);
  v.printOver("/7x11.fon","Hello world!",1,0,16);
  v.printOver("/10x15.fon","Hello world!",1,0,32);
  //Para escribir con borde necesitamos crear un sprite
  RoJoSprite2 textSprite(0);
  //Texto negro, fondo negro, borde blanco
  textSprite.print("/10x15.fon","Hello",0,0,1);
  v.drawSprite(&textSprite,0,48);
  textSprite.end();
  //Lo mostramos
  display.drawSprite(&v);

  delay(2000);
}

//Círculos y elipses
void test21() {
  //Limpiamos la memoria de vídeo
  v.clear();

  v.circle(15,15,14,1);
  v.disk(45,15,14,1);
  v.ellipse(15,60,14,29,1);
  v.ellipseFill(45,60,14,29,1);
  //Lo mostramos
  display.drawSprite(&v);

  delay(1000);
}

void setup(void) {
  //Serial.begin(115200); delay(4000); //DEBUG

  ////Pinout
  //void begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqCOMM=0);
  #ifdef ESP32
    display.begin(33,27,14);
  #elif defined(ESP8266)
    display.begin(D0,D2,D3);
  #else
    #error Device unknown
  #endif
  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop(void) {
  test01(); //Función drawPixel con color 1 = dibujar
  test03(); //Función drawPixel con color 0 = borrar
  test04(); //Función getPixel
  test05(); //Rectángulos
  test06(); //Líneas
  test07(); //Sprites definidos en programa.
  test08(); //Leer sprite desde un archivo
  test09(); //Mover sprite por pantalla
  test10(); //Sincronización de sprites
  test11(); //Mover sprite por pantalla manteniendo una imagen de fondo
  test12(); //Mover sprite por pantalla manteniendo una imagen de fondo no transparente
  test13(); //Obtener sprite de parte de otro
  test14(); //Leer un bmp y guardar un sprite
  test15(); //Resize
  test16(); //Mostrar texto
  test17(); //Rotar sprites
  test18(); //Flip
  test19(); //Redimensionar texto
  test20(); //Mostrar texto con más fuentes
  test21(); //Círculos y elipses
}