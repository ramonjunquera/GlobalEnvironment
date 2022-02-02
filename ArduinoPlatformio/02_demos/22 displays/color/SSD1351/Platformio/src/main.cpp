/*
  Autor: Ramón Junquera
  Fecha: 20220202
  Tema: Librería para display OLED SPI 1.5" 128x128 SSD1351
  Objetivo: Demo de librería RoJoSSD1351
  Material: breadboard, cables, display OLED SSD1351, lector SD
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

  Memoria de vídeo= 128*128*2=32Kb
  Sistema de doble buffer= memoria de vídeo *2=64Kb
  
  Nota 1:
    El sistema de archivos debe contener los archivos de la
    carpeta data para su correcto funcionamiento.
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoSprite.h> //Gestión de sprites color
#include <RoJoSSD1351.h> //Librería de gestión del display SSD1351


#ifdef ESP32
  const byte pinDC=22;
  const byte pinRES=2;
  const byte pinCS=5; //CS por defecto
#elif defined(ESP8266)
  const byte pinDC=D2;
  const byte pinRES=D0;
  const byte pinCS=D3;
#elif defined(ARDUINO_ARCH_AVR)
  const byte pinDC=5;
  const byte pinRES=3;
  const byte pinCS=4;
#elif defined(__arm__)
  const byte pinDC=20;
  const byte pinRES=21;
  const byte pinCS=16;
#endif

//Creamos objeto de gestión
RoJoSSD1351 display;

//Test de color
//  RoJoSSD1351::clear,block
void Test1() {
  //Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
  //en pantalla, formando una matriz de 3x3
  display.clear(); //Limpiamos pantalla
  display.block(42,0,42,42,display.getColor(127,127,127)); //Gris
  display.block(84,0,42,42,display.getColor(255,255,255)); //Blanco
  display.block(0,42,42,42,display.getColor(255,0,0)); //RoJo
  display.block(42,42,42,42,display.getColor(0,255,0)); //Verde
  display.block(84,42,42,42,display.getColor(0,0,255)); //Azul
  display.block(0,84,42,42,display.getColor(255,255,0)); //Amarillo
  display.block(42,84,42,42,display.getColor(255,0,255)); //Magenta
  display.block(84,84,42,42,display.getColor(0,255,255)); //Cyan
  delay(1000);
}

//Test de pixels
//  RoJoSSD1351::drawPixel
void Test2() {
  //Limpiamos display
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=10)
    for(byte x=0;x<display.xMax();x+=10)
      display.drawPixel(x,y,display.getColor(x*2,y*2,0));
  delay(1000);
}

//Test de líneas
//  RoJoSSD1351::line
void Test3() {
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint32_t color=display.getColor(0,255,0); //verde
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,0,x,display.xMax()-1,color);
  //Dibujamos líneas azules
  color=display.getColor(0,0,255); //azul
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,display.xMax()-1,x,0,color);
  delay(1000);
}

//Test de rectángulos
//  RoJoSSD1351:rect
void Test4() {
  //Limpiamos display
  display.clear();
  //Dibujamos un rectángulo relleno rojo
  display.block(10,10,70,70,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.rect(50,50,50,50,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.block(70,70,50,50,display.getColor(0,0,255));
  delay(1000);
}

//Test de triángulos
//  RoJoSSD1351::triangle,triangleFill
void Test5() {
  //Limpiamos display
  display.clear();
  //Dibujamos un triángulo relleno rojo
  display.triangleFill(40,0,50,100,0,30,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  display.triangle(80,10,120,110,60,120,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  display.triangleFill(110,20,127,80,10,70,display.getColor(0,0,255));
  delay(1000);
}

//Test de circunferencias, círculos y elipses
//  RoJoSSD1351::circle,disk,ellipse,ellipseFill
void Test6() {
  display.clear(); //Limpiamos display
  display.circle(50,50,45,display.getColor(255,0,0)); //Circunferencia
  display.disk(70,70,30,display.getColor(0,255,0)); //Círculo
  display.circle(90,90,35,display.getColor(0,0,255)); //Circunferencia
  delay(1000);
  display.clear(); //Limpiamos display
  display.ellipse(55,40,45,30,display.getColor(255,0,0));
  display.ellipseFill(50,65,20,45,display.getColor(0,255,0));
  display.ellipse(70,100,50,20,display.getColor(0,0,255));
  delay(1000);
}

//Test de carga de imágen desde archivo
//  RoJoSSD1351::drawSprite,drawBMP
void Test7() {
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=0;y<4;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite("/heart.spr",x*32,y*32);
  delay(1000);
  //Cargamos varios sprites de 128x128
  display.drawSprite("/palmblue.spr");
  delay(1000);
  display.drawSprite("/palmyell.spr");
  delay(1000);
  //Cargamos un archivo bmp
  display.drawBMP("/sea.bmp");
  delay(1000);
}

//Test de texto
//  RoJoSSD1351::printOver
void Test8() {
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
  display.printOver("/10x15d.fon","1234567890",color,100,25); //No se ve el final
  //Fuente más grande
  display.printOver("/21x33d.fon","1234567890",color,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.printOver("/7x11.fon","Hola, esto es una prueba!",color,0,80);
  delay(2000);
}

//Test de color
//  RoJoSSD1351::clear,drawSprite
//  RoJoSprite::setSize,end
void Test9() {
  //Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
  //en pantalla, formando una matriz de 3x3
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite; //Creamos el sprite
  sprite.setSize(42,42); //Lo dimensionamos
  sprite.clear(sprite.getColor(127,127,127)); //Gris
  display.drawSprite(&sprite,42,0);
  sprite.clear(sprite.getColor(255,255,255)); //Blanco
  display.drawSprite(&sprite,84,0);
  sprite.clear(sprite.getColor(255,0,0)); //Rojo
  display.drawSprite(&sprite,0,42);
  sprite.clear(sprite.getColor(0,255,0)); //Verde
  display.drawSprite(&sprite,42,42);
  sprite.clear(sprite.getColor(0,0,255)); //Azul
  display.drawSprite(&sprite,84,42);
  sprite.clear(sprite.getColor(255,255,0)); //Amarillo
  display.drawSprite(&sprite,0,84);
  sprite.clear(sprite.getColor(255,0,255)); //Magenta
  display.drawSprite(&sprite,42,84);
  sprite.clear(sprite.getColor(0,255,255)); //Cyan
  display.drawSprite(&sprite,84,84);
  sprite.end(); //Liberamos memoria del sprite
  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//  RoJoSprite::loadSprite,loadBMP
void Test10() {
  //Creamos el sprite
  RoJoSprite sprite;

  //Dibujamos dos líneas con una imágen de 32x32 tomada de un archivo de sprite
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=0;y<2;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);
  //Liberamos la memoria del sprite
  sprite.end();

  //Dibujamos otras dos líneas con una imágen de 32x32 tomada de un archivo bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=2;y<4;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de guardado
//  RoJoSprite::save
void Test11() {
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite1; //Creamos un sprite
  sprite1.setSize(3,3); //Tendrá un tamaño de 3x3
  sprite1.clear(sprite1.getColor(255,0,0)); //Lo pintamos de rojo
  sprite1.block(1,1,2,2,sprite1.getColor(0,255,0)); //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.save("/test.spr"); //Lo guardamos en un archivo

  RoJoSprite sprite2; //Creamos un nuevo sprite
  sprite2.loadSprite("/test.spr"); //Cargamos la imágen desde un archivo
  sprite1.resize(&sprite2,50,50); //Lo redimensionamos para que se vea claro
  sprite2.end(); //Liberamos memoria del sprite2
  display.drawSprite(&sprite1); //Mostramos el sprite
  sprite1.end(); //Liberamos memoria del sprite1
  RoJoFS.remove("/test.spr"); //Borramos el archivo
  delay(1000);
}

//Test de pixels
//  RoJoSprite::setSize,drawPixel,end
//  RoJoSSD1351::drawSprite
void Test12() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=10)
    for(byte x=0;x<display.xMax();x+=10)
      sprite.drawPixel(x,y,sprite.getColor(x*2,y*2,0));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de carga de sprites grandes desde archivo
//  RoJoSprite::loadSprite,loadBMP
void Test13() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Utilizaremos sprites de tamaño 128x128, igual que las dimensiones del display
  sprite.loadSprite("/palmblue.spr");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadSprite("/palmyell.spr");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadSprite("/sea.spr");
  display.drawSprite(&sprite);
  delay(1000);
  //Repetimos la secuencia cargando desde archivos bmp
  sprite.loadBMP("/palmblue.bmp");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadBMP("/palmyell.bmp");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadBMP("/sea.bmp");
  display.drawSprite(&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de líneas
//  RoJoSprite::line
void Test14() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos líneas verdes
  uint32_t color=display.getColor(0,255,0); //Verde
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,0,x,display.xMax()-1,color);
  //Dibujamos líneas azules
  color=display.getColor(0,0,255); //azul
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,display.yMax()-1,x,0,color);
  //Mostramos el sprite que ocupa todo el display
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//  RoJoSprite::block,rect
void Test15() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un rectángulo relleno rojo
  sprite.block(0,0,100,50,sprite.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(50,25,60,75,sprite.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(80,80,48,48,sprite.getColor(0,0,255));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de triángulos
//  RoJoSprite::triangle,triangleFill
void Test16() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(70,0,40,90,0,30,sprite.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(0,60,90,50,30,120,sprite.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(127,70,120,127,10,70,sprite.getColor(0,0,255));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de circunferencias, círculos y elipses
//  RoJoSprite::circle,disk
void Test17() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());

  sprite.circle(50,50,45,sprite.getColor(255,0,0)); //Circunferencia
  sprite.disk(70,70,30,sprite.getColor(0,255,0)); //Círculo
  sprite.circle(90,90,35,sprite.getColor(0,0,255)); //Circunferencia
  display.drawSprite(&sprite); //Mostramos el sprite a pantalla completa

  delay(1000);

  sprite.clear();
  sprite.ellipse(55,40,45,30,sprite.getColor(255,0,0)); //Circunferencia
  sprite.ellipseFill(50,65,20,45,sprite.getColor(0,255,0)); //Círculo
  sprite.ellipse(70,100,50,20,sprite.getColor(0,0,255)); //Circunferencia
  display.drawSprite(&sprite); //Mostramos el sprite a pantalla completa

  sprite.end(); //Liberamos memoria del sprite
  delay(1000);
}

//Test de cambio de tamaño
//  RoJoSprite::resize
void Test18() {
  //Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.loadSprite(F("/heart.spr"));
  //Creamos el sprite redimensionado
  RoJoSprite spriteZoom;
  for(byte z=32;z<128;z+=5) {
    //Redimensionamos el sprite
    spriteZoom.resize(&spriteOri,z,z);
    //Lo mostramos
    display.drawSprite(&spriteZoom);
  }
  //Liberamos memoria del sprite redimensionado
  spriteZoom.end();
  //Liberamos memoria del sprite original
  spriteOri.end();

  delay(500);
}

//Test de texto
//  RoJoSprite::print
void Test19() {
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
  display.drawSprite(&sprite,100,25); //No se ve el final
  //Fuente más grande
  sprite.print("/21x33d.fon","1234567890",color);
  display.drawSprite(&sprite,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  sprite.print("/7x11.fon","Hola, esto es una prueba!",color);
  display.drawSprite(&sprite,0,80);
  //Texto con fondo negro y bordes blancos
  sprite.print("/7x11.fon","Hola!",color,0,sprite.getColor(255,255,255));
  display.drawSprite(&sprite,0,100);
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de rotate
//  RoJoSprite::rotate
void Test20() {
  //Crearemos un sprite de texto y lo rotaremos
  display.clear();
  RoJoSprite sprite;
  sprite.print(F("/10x15d.fon"),F("123"),sprite.getColor(255,255,0)); //Texto en amarillo
  display.drawSprite(&sprite,30,0);
  RoJoSprite spriteRotate;
  spriteRotate.rotate(&sprite,90);
  display.drawSprite(&spriteRotate,60,16);
  spriteRotate.rotate(&sprite,180);
  display.drawSprite(&spriteRotate,30,46);
  spriteRotate.rotate(&sprite,270);
  display.drawSprite(&spriteRotate,14,16);
  spriteRotate.end();
  sprite.end();
  delay(2000);
}

//Test de flip
//  RoJoSprite::flipH,flipV
void Test21() {
  //Crearemos un sprite de texto y lo voltearemos
  display.clear();
  RoJoSprite sprite;
  sprite.print(F("/10x15d.fon"),F("123"),sprite.getColor(255,255,0)); //Texto en amarillo
  display.drawSprite(&sprite);
  sprite.replaceColor(sprite.getColor(255,255,0),sprite.getColor(155,155,0)); //Amarillo más ocuro
  RoJoSprite spriteFlip;
  spriteFlip.flipH(&sprite);
  display.drawSprite(&spriteFlip,33,0);
  spriteFlip.flipV(&sprite);
  display.drawSprite(&spriteFlip,0,16);
  spriteFlip.end();
  sprite.end();
  delay(2000);
}

//Test de reemplazar color y transparencia
//  RoJoSprite::replaceColor,drawSprite
void Test22() {
  //Creamos un sprite para el fondo
  RoJoSprite spriteBack;
  //Cargamos una imágen tan grande como el display
  spriteBack.loadSprite("/palmyell.spr");
  //Creamos un sprite de 30x30
  RoJoSprite sprite;
  sprite.setSize(30,30);
  //Lo pintamos de rojo
  sprite.clear(sprite.getColor(255,0,0));
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(10,10,20,20,sprite.getColor(0,255,0));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,10,10);
  //Sustituimos el color verde por azul
  sprite.replaceColor(sprite.getColor(0,255,0),sprite.getColor(0,0,255));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,50,10);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(&sprite,90,10,spriteBack.getColor(0,0,255));
  //Lo mostramos
  display.drawSprite(&spriteBack);
  //Liberamos memoria de los sprites
  sprite.end();
  spriteBack.end();
    
  delay(1000);
}

//Test de copia parcial
//  RoJoSprite::copy
void Test23() {
  //Limpiamos display
  display.clear();
  //Creamos un sprite con una imágen del tamaño del display
  RoJoSprite back;
  back.loadSprite("/sea.spr");
  //Creamos un sprite para guardar un recorte del primero
  RoJoSprite half;
  //Copiamos al nuevo sprite sólo el cuarto superior izquierdo
  half.copy(&back,0,0,back.xMax()/2,back.yMax()/2);
  //Mostramos el sprite recortado
  display.drawSprite(&half);
  //Liberamos memoria de los sprites
  half.end();
  back.end();

  delay(1000);
}

//Test de sincronización
//  RoJoSSD1351::drawSpriteSync
//  RoJoSprite::copy
void Test24() {
  //Se moverá un número por el display
  //El fondo estará fijo y será una imágen
  //Crearemos un sprite para el fondo y otro más pequeño para el número
  //Crearemos dos sprites más. Uno como memoria de vídeo de trabajo
  //y otro como memoria de vídeo del display.
  //Las imágen final se compone sobre la memoria de vídeo de trabajo
  //Después se sincroniza con la memoria de vídeo del display y conseguimos
  //que sólo se envíen las diferencias.

  //Creamos el sprite con la imágen de fondo
  RoJoSprite back;
  back.loadSprite("/palmblue.spr");
  //Creamos sprite con el número
  RoJoSprite num;
  //Color de texto: rojo. Fondo: negro. Borde: blanco
  num.print("/21x33d.fon","3",num.getColor(255,0,0),0,num.getColor(255,255,255));
  //Creamos sprite con memoria de vídeo de trabajo con el tamaño del display
  RoJoSprite workMem;
  //Copiamos el sprite de fondo en la memoria de trabajo
  workMem.copy(&back);
  //Creamos sprite con memoria de vídeo de display
  RoJoSprite displayMem;
  //Copiamos el sprite de fondo en la memoria del display
  displayMem.copy(&back);
  //Para sincronizar todas la memorias, dibujamos el sprite de fondo en el display
  display.drawSprite(&back);

  //Coordenadas del sprite de texto
  int8_t x=0,y=0;
  //Dirección:
  int8_t dx=3,dy=3;
  //Tiempo límite (duración = 5s)
  uint32_t maxTime=millis()+5000;
  //Mientras no superemos el tiempo límite
  while(millis()<maxTime) {
    //Calculamos nuevas coordenadas
    x+=dx; y+=dy;
    //Si hemos llegado a algún borde...cambiamos de dirección
    if(x<=0 || x>=display.xMax()-num.xMax()) dx=-dx;
    if(y<=0 || y>=display.yMax()-num.yMax()) dy=-dy;
    //Dibujamos la imágen de fondo sobre el sprite de trabajo
    workMem.drawSprite(&back);
    //Añadimos el número sobre el fondo, haciendo su fondo negro transparente
    workMem.drawSprite(&num,x,y,0);
    //Sincronizamos los sprites con las memorias de vídeo y enviamos las
    //diferencias al display
    display.drawSpriteSync(&workMem,&displayMem);
  }
  //Liberamos memoria de los sprites
  displayMem.end();
  workMem.end();
  num.end();
  back.end();
}

void setup() {
  //Serial.begin(115200); //DEBUG
  //Inicializamos el display
  display.begin(pinRES,pinDC,pinCS);
}

void loop() {
  Test1(); //Test de block
  Test2(); //Test de pixels
  Test3(); //Test de líneas
  Test4(); //Test de rectángulos
  Test5(); //Test de triángulos
  Test6(); //Test de circunferencias, círculos y elipses
  Test7(); //Test de carga de imágen desde archivo
  Test8(); //Test de texto
  //Los dispositivos Arduino UNO y NANO no tienen memoria suficiente
  //para trabajar con sprites
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) 
    Test9(); //Test de color
    Test10(); //Test de carga de sprite pequeño desde archivo
    Test11(); //Test de guardado
    //Arduino Mega no tiene memoria suficiente para cargar sprites grandes
    #ifndef ARDUINO_AVR_MEGA2560
      Test12(); //Test de pixels
      Test13(); //Test de carga de sprites grandes desde archivo
      Test14(); //Test de líneas
      Test15(); //Test de rectángulos
      Test16(); //Test de triángulos
      Test17(); //Test de circunferencias, círculos y elipses
      Test18(); //Test de cambio de tamaño
      Test19(); //Test de texto
      Test20(); //Test de rotate
      Test21(); //Test de flip
      Test22(); //Test de reemplazar color y transparencia
      Test23(); //Test de copia parcial
      #ifndef ESP8266 //ESP8266 no tiene suficiente memoria para doble buffer
        Test24(); //Test de sincronización
      #endif
    #endif
  #endif
}
