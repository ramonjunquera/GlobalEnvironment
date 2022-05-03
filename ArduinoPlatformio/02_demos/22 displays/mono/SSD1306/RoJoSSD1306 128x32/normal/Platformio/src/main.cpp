/*
  Autor: Ramón Junquera
  Fecha: 20220503
  Tema: Librería para display OLED I2C 0.96" 128x64 SSD1306
  Objetivo: Demo de librería RoJoSSD1306

  Descripción y demostración de funcionalidades de la librería.

  I2C pinout:

  device      SCL SDA RST
  ----------- --- --- ---
  Mega        21  20  ---
  ESP8266     D1  D2  ---
  ESP32       22  21  ---
  ESP32+OLED  15   4  16
  RPi 0       28  27  --- -> ID_SC & ID_SD    
  RPi 1        5   3  --- -> GPIO3 & GPIO2

  Notas:
  - El sistema de archivos seleccionado debe contener los archivos de la carpeta
    data para su correcto funcionamiento.
  - Este modelo de display no permite dibujar directamente en pantalla, pero si se puede dibujar
    un sprite sobre ella. Para la mayoría de los ejemplos utilizarmos un sprite con las mismas
    dimensiones del display (128x64) en el que dibujaremos para después volcarlo en el display.
    Este sprite lo llamaremos 'v' (de memoria de vídeo) y lo definiremos globalmente.
  - Las placas Arduino UNO y Nano no tienen suficiente memoria como para almacenar el programa
  
  Resultado:
    Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306

//Creamos objeto de gestión
RoJoSSD1306 display;
//Creamos un sprite monocromo que utilizaremos como memoria de vídeo
RoJoSprite v(0);

//Definición de pines
#ifdef ESP32OLED16MB //Si se trata del ESP32+OLED 16Mb
  byte pinSDA=4;
  byte pinSCL=15;
  byte pinRES=16;
#elif defined(ESP32) //Si es ESP32
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=16;  
#elif defined(ESP8266) //Si es ESP8266
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=D0;
#elif defined(ARDUINO_AVR_MEGA2560) //Si es Arduino Mega
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=3;
#else //Para cualquier otro dispositivo utilizaremos los pines estándar
  byte pinSDA=255;
  byte pinSCL=255;
  byte pinRES=255; //Sin pin de reset
#endif

//Función drawPixel con color 1 = dibujar
void test1() {
  v.clear(); //Limpiamos sprite
  //Dibujamos una matriz de puntos
  for(uint16_t x=0;x<v.xMax();x+=5)
    for(uint16_t y=0;y<v.yMax();y+=5)
      v.drawPixel(x,y,1);
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Función drawPixel con color 0 = borrar
void test3() {
  //Dibujamos un rectángulo punto a punto que borra
  for(uint16_t y=5;y<27;y++)
    for(uint16_t x=10;x<40;x++)
      v.drawPixel(x,y,0);
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Función getPixel
void test4() {
  //Comprobamos los pixels de un cuadrado y dibujamos el color opuesto
  for(uint16_t y=10;y<22;y++)
    for(uint16_t x=30;x<50;x++)
      v.drawPixel(x,y,(1-v.getPixel(x,y)));
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Rectángulos
void test5() {
  v.clear(); //Limpiamos sprite
  v.block(0,0,20,20,1); //Dibujamos un rectángulo relleno sin borde (un bloque)
  v.rect(10,10,20,20,1); //Dibujamos un rectángulo (sin relleno)
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Líneas
void test6() {
  v.clear(); //Limpiamos sprite
  for(byte y=0;y<v.yMax();y+=10) v.line(0,0,v.xMax()-1,y,1);
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Sprites definidos en programa
void test7() {
  //Limpiamos pantalla
  v.clear();
  //Creamos el sprite monocromo
  RoJoSprite mySprite(0);
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
//  RoJoSprite2::loadSprite,drawSprite
//  RoJoSSD1306::negative
void test8() {
  display.clear(); //Limpiamos display mientras creamos el sprite
  //En el sprite que vamos a leer el dibujo lo hacen los pixels activos
  //En dibujo original estaba hecho en negro sobre fondo blanco
  //Es justo al revés de lo que muestra el display por defecto
  //Lo normal es que una pantalla vacía sera de color negro
  //Por eso activaremos el modo inverso
  display.negative(true);

  v.clear(); //Limpiamos sprite de memoria de vídeo

  //Método 1. Cargamos de un archivo a un nuevo sprite
  RoJoSprite mySprite(0); //Creamos el sprite monocromo
  mySprite.loadSprite("/ball.spr"); //Lo leemos desde el archivo
  v.drawSprite(&mySprite); //Dibujamos el sprite en memoria de vídeo (en 0,0)
  mySprite.end(); //Borramos el sprite utilizado

  //Método 2. Dibujamos sprite directamente sobre otro
  v.drawSprite("/ball.spr",64,0); //Leemos archivo y lo dibujamos en 64,0
  
  display.drawSprite(&v); //Mostramos el resultado
  delay(1000);
}

//Mover sprite por pantalla
void test9() {
  //Seguiremos con el modo inverso activado (pixels en negro)
  display.negative(true);
  //Creamos un nuevo sprite monocromo
  RoJoSprite ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/bal1.spr"); //Su tamaño es x=13,y=13,pages=2
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
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
    if(x==0 || x==114) dx=-dx; //114=128-13-1
    if(y==0 || y==18) dy=-dy; //18=32-13-1
    #ifdef ESP8266
      yield(); //Refrescamos WatchDog
    #endif
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
  //Creamos el sprite monocromo de memoria de vídeo del display
  RoJoSprite videoMem(0);
  //Le damos el mismo tamaño que el display
  //Ya se crea limpio
  videoMem.setSize(display.xMax(),display.yMax());
  //Borramos el display
  display.clear();
  //Tenemos las 3 memorias vacías (sincronizadas)

  //Repetimos el ejemplo anterior de mover un sprite por pantalla
  //Creamos un nuevo sprite monocromo
  RoJoSprite ball(0);
  //Leemos su contenido desde un archivo
  ball.loadSprite("/bal1.spr"); //Su tamaño es x=30,y=30,pages=4
  //Coordenadas
  byte x=0,y=0;
  //Deltas
  int8_t dx=1,dy=1;
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
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
    if(x==0 || x==114) dx=-dx; //114=128-13-1
    if(y==0 || y==18) dy=-dy; //18=32-13-1
  }
  //Borramos el sprite utilizado
  ball.end();
  videoMem.end();
}

//Mover sprite por pantalla manteniendo fija una imagen de fondo
void test11() {
  //Nota: los pixel apagados del sprite en movimiento aparecen transparentes

  
  display.negative(true); //Seguiremos con el modo inverso activado (pixels en negro)
  RoJoSprite backSprite(0); //Creamos un nuevo sprite monocromo para la imagen fija
  backSprite.loadSprite("/ball.spr"); //Leemos su contenido desde un archivo
  RoJoSprite ball(0); //Creamos un nuevo sprite monocromo para la imagen en movimiento
  //Leemos su contenido desde un archivo
  ball.loadSprite("/bal1.spr"); //Su tamaño es x=13,y=13,pages=2
  byte x=0,y=0; //Coordenadas
  int8_t dx=1,dy=1; //Deltas
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  while(millis()<tMax) { //Bucle principal
    v.clear();
    v.drawSprite(&backSprite,48,0); //Dibujamos la imagen fija en la memoria de vídeo
    v.drawSprite(&ball,x,y,0); //Dibujamos el sprite en movimiento tomando los pixels apagados como transparentes
    display.drawSprite(&v); //Refrescamos pantalla
    x+=dx, y+=dy; //Calculamos las nuevas coordenadas
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==114) dx=-dx; //114=128-13-1
    if(y==0 || y==18) dy=-dy; //18=32-13-1
  }
  //Borramos los sprites utilizados
  ball.end();
  backSprite.end();
}

//Mover sprite por pantalla manteniendo una imagen de fondo y que no sea transparente
//  RoJoSprite2::loadBMP,negative
void test12() {
  //Para conseguir esto, necesitamos una imagen que nos haga de máscara
  //Esta máscara permitirá borrar antes de dibujar el sprite definitivo

  display.negative(true); //Mantenemos el modo inverso
  RoJoSprite backSprite(0); //Creamos un nuevo sprite monocromo para la imagen fija
  backSprite.loadSprite("/ball.spr"); //Leemos su contenido desde un archivo
  RoJoSprite ball(0); //Creamos un nuevo sprite monocromo para la imagen en movimiento
  //Leemos su contenido desde un archivo
  ball.loadSprite("/bal1.spr"); //Su tamaño es x=13,y=13,pages=2
  
  RoJoSprite ballMask(0); //Creamos un nuevo sprite monocromo para la máscara de la imagen en movimiento
  //Leemos su contenido desde un archivo
  ballMask.loadSprite("/bal1mask.spr"); //Su tamaño es x=13,y=13,pages=2
  ballMask.negative(); //También tiene los colores invertidos
  
  byte x=0,y=0; //Coordenadas
  int8_t dx=1,dy=1; //Deltas
  //Calculamos el tiempo máximo de duración de esta demo
  uint32_t tMax=millis()+4000; //4 segundos
  while(millis()<tMax) { //Bucle principal
    v.clear();
    v.drawSprite(&backSprite,48,0); //Dibujamos el sprite estático
    v.drawSprite(&ballMask,x,y,1); //Dibujamos la máscara del sprite en movimiento. Color 1 transparente
    v.drawSprite(&ball,x,y,0); //Dibujamos el sprite en movimiento con coor 0 transparente
    display.drawSprite(&v); //Refrescamos pantalla
    x+=dx, y+=dy; //Calculamos las nuevas coordenadas
    //Si hemos tocado con un borde, cambiamos la dirección
    if(x==0 || x==114) dx=-dx; //114=128-13-1
    if(y==0 || y==18) dy=-dy; //18=32-13-1
  }
  //Borramos los sprites utilizados
  ballMask.end();
  ball.end();
  backSprite.end();
}

//Obtener sprite de parte de otro
void test13() {
  display.negative(true); //Seguiremos con el modo inverso activado (pixels en negro)
  v.clear();
  v.drawSprite("/ball.spr"); //Dibujamos un sprite desde un archivo
  RoJoSprite mySprite(0); //Creamos un nuevo sprite monocromo
  mySprite.copy(&v,0,0,16,16); //Copiamos parte de la memoria de vídeo en el nuevo sprite
  v.drawSprite(&mySprite,64,0); //Dibujamos el sprite recortado
  display.drawSprite(&v); //Refrescamos pantalla
  mySprite.end(); //Borramos el sprite utilizado
  delay(1000);
}

//Leer un bmp
//Guardar un sprite
void test14() {
  display.clear();
  display.negative(false);

  v.clear();
  v.drawBMP("/ball.bmp"); //Dibujamos un bmp desde un archivo en la memoria de vídeo en 0,0
  RoJoSprite mySprite(0); //Creamos un nuevo sprite monocromo
  mySprite.copy(&v,0,0,16,16); //Copiamos parte de la memoria de vídeo en el nuevo sprite
  mySprite.save("/ball2.spr"); //Guardamos el sprite en un archivo
  mySprite.end(); //Borramos el sprite utilizado

  RoJoSprite mySprite2(0);   //Creamos un nuevo sprite monocromo
  mySprite2.loadSprite("/ball2.spr"); //Leemos su contenido desde el archivo
  //Dibujamos el sprite varias veces en la memoria de vídeo
  for(byte y=0;y<v.yMax();y+=16)
    for(byte x=32;x<v.xMax();x+=16)
      v.drawSprite(&mySprite2,x,y);
  display.drawSprite(&v); //Refrescamos pantalla
  mySprite2.end(); //Borramos el sprite utilizado
  //Borramos el archivo
  RoJoFS.remove("/ball2.spr");

  delay(2000);
}

//Resize
void test15() {
  display.clear();
  display.negative(true);
  
  RoJoSprite mySprite(0); //Creamos un nuevo sprite monocromo
  mySprite.loadSprite("/ball.spr"); //Leemos su contenido desde un archivo
  RoJoSprite resizeSprite(0); //Creamos el sprite monocromo utilizado para el redimensionado
  display.clear(); //Borramos el display
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
  display.clear();
  v.clear(); //Limpiamos la memoria de vídeo
  display.negative(false);
  v.printOver("/5x7d.fon","20220503",1); //Escribimos el texto directamente sobre el sprite
  RoJoSprite textSprite(0); //Creamos el sprite monocromo que contendrá el texto
  //Utilizamos una fuente más grande que crea un texto más grande que la anchura del display
  textSprite.print("/10x15d.fon","123456789012",1);
  //Dibujamos el sprite de texto en la memoria de vídeo. No se verá el final
  v.drawSprite(&textSprite,70,16);
  //Lo mostramos de nuevo desplazado a la izquierda, comenzando desde una
  //posición fuera de pantalla. No se verá el principio
  v.drawSprite(&textSprite,-70,16);
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
  //Borramos el sprite utilizado
  textSprite.end();

  delay(2000);
}

//Rotar sprites
void test17() {
  v.clear(); //Limpiamos la memoria de vídeo
  
  RoJoSprite textSprite(0); //Creamos el sprite monocromo que contendrá el texto
  textSprite.print("/10x15d.fon","123",1); //Escribimos el texto en el sprite
  v.drawSprite(&textSprite); //Dibujamos el sprite de texto en la memoria de vídeo
  RoJoSprite textSpriteRotate(0); //Creamos el sprite monocromo que contendrá el texto rotado
  textSpriteRotate.rotate(&textSprite,90); //Rotamos el sprite de texto 90 grados
  v.drawSprite(&textSpriteRotate,60,0); //Dibujamos el sprite de texto rotado en la memoria de vídeo
  textSpriteRotate.rotate(&textSprite,180); //Rotamos el sprite de texto 180 grados
  v.drawSprite(&textSpriteRotate,0,16); //Dibujamos el sprite de texto rotado en la memoria de vídeo
  textSpriteRotate.rotate(&textSprite,270); //Rotamos el sprite de texto 270 grados
  v.drawSprite(&textSpriteRotate,80,0); //Dibujamos el sprite de texto rotado en la memoria de vídeo
  
  display.drawSprite(&v); //Mostramos la memoria de vídeo
  //Borramos los sprites utilizados
  textSpriteRotate.end();
  textSprite.end();

  delay(2000);
}

//Flip
void test18() {
  v.clear(); //Limpiamos la memoria de vídeo
  
  RoJoSprite textSprite(0); //Creamos el sprite monocromo que contendrá el texto
  textSprite.print("/10x15d.fon","123",1); //Escribimos el texto en el sprite
  v.drawSprite(&textSprite); //Dibujamos el sprite de texto en la memoria de vídeo
  RoJoSprite textSpriteFlip(0); //Creamos el sprite monocromo que contendrá el texto volteado

  textSpriteFlip.flipH(&textSprite); //Volteamos el sprite en horizontal
  v.drawSprite(&textSpriteFlip,33,0); //Dibujamos el sprite de texto volteado en horizontal en la memoria de vídeo

  textSpriteFlip.flipV(&textSprite); //Volteamos el sprite de texto en vertical
  v.drawSprite(&textSpriteFlip,0,16); //Dibujamos el sprite de texto volteado en vertical en la memoria de vídeo
  
  display.drawSprite(&v); //Mostramos la memoria de vídeo
  //Borramos los sprites utilizados
  textSpriteFlip.end();
  textSprite.end();

  delay(2000);
}

//Redimensionar texto
void test19() {
  //Creamos un sprite de texto
  //Lo redimensionamos al tamaño del display
  //Lo mostramos

  RoJoSprite textSprite(0); //Creamos el sprite monocromo para el texto
  textSprite.print("/5x7d.fon","1234",1); //Escribimos el texto con una fuente pequeña
  v.resize(&textSprite,display.xMax(),display.yMax()); //Redimensionamos el texto al tamaño del display
  display.drawSprite(&v); //Mostramos el sprite en el display
  textSprite.end(); //Borramos los sprites utilizados

  delay(1000);
}

//Mostrar texto con más fuentes
void test20() {
  v.clear(); //Limpiamos la memoria de vídeo
  //Escribimos con distintos tamaños de fuente
  v.printOver("/3x5.fon","Hello world!",1);
  v.printOver("/5x7.fon","Hello world!",1,0,7);
  v.printOver("/7x11.fon","Hello world!",1,0,15);
  v.printOver("/10x15.fon","Hello!",1,80,0);
  
  RoJoSprite textSprite(0); //Para escribir con borde necesitamos crear un sprite
  textSprite.print("/10x15.fon","Hello!",0,0,1);
  v.drawSprite(&textSprite,80,16);
  textSprite.end();
  display.drawSprite(&v); //Lo mostramos

  delay(2000);
}

//Círculos y elipses
void test21() {
  v.clear(); //Limpiamos la memoria de vídeo

  v.circle(15,15,14,1);
  v.disk(45,15,14,1);
  v.ellipse(90,7,29,7,1);
  v.ellipseFill(90,23,29,7,1);
  
  display.drawSprite(&v); //Lo mostramos
  delay(1000);
}

//Test de triángulos
//  RoJoSprite::triangle,triangleFill
void test22() {
  v.clear(); //Limpiamos la memoria de vídeo
  v.triangleFill(20,0,50,20,0,10,1); //Dibujamos un triángulo relleno
  v.triangle(20,20,80,5,40,30,1); //Dibujamos un triángulo sin relleno
  v.triangleFill(50,5,90,15,70,30,1); //Dibujamos un triángulo relleno
  display.drawSprite(&v); //Lo mostramos
  delay(1000);
}

void setup() {
  Serial.begin(115200); //DEBUG

  //Inicializamos el display con los pines en los que está conectado la placa
  //void begin(byte pinSDA,byte pinSCL,byte pinRST,bool halfRows);
  display.begin(pinSDA,pinSCL,pinRES,true);
  display.setContrast(128); //Aumentamos el contraste por defecto

  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop() {
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO)
    test1(); //Función drawPixel con color 1 = dibujar
    test3(); //Función drawPixel con color 0 = borrar
    test4(); //Función getPixel
    test5(); //Rectángulos
    test6(); //Líneas
    test7(); //Sprites definidos en programa
    test8(); //Leer sprite desde un archivo
    test9(); //Mover sprite por pantalla
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
    test22(); //Test de triángulos
  #endif
}
