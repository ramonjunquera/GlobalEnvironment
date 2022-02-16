/*
  Autor: Ramón Junquera
  Fecha: 20220203
  Tema: Librería para display RPI_ILI9486 SPI 3.5" 480*320
  Objetivo: Demo de librería RoJoILI9486
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

  El sistema de archivos debe contener los archivos de la
  carpeta data para su correcto funcionamiento.
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoILI9486.h> //Librería de gestión del display ILI9486

//Definición de pines
#ifdef ESP8266 //Si es un ESP8266...
  const byte pinDC_display=D2;
  const byte pinRES_display=D0;
  const byte pinCS_display=D3;
#elif defined(ESP32) //Si es un ESP32...
  const byte pinDC_display=22;
  const byte pinRES_display=2;
  const byte pinCS_display=5;
#elif defined(__arm__) //Si es una Raspberry Pi
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
#elif defined(ARDUINO_ARCH_AVR) //Si es una placa Arduino
  const byte pinDC_display=5;
  const byte pinRES_display=4;
  const byte pinCS_display=3;
#endif

//Creamos objeto de gestión
RoJoILI9486 display;

//Test de color
//  RoJoILI9486::clear,block
void Test1() {
  //Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
  //en pantalla, formando una matriz de 7x15
  display.clear(); //Limpiamos pantalla
  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]= {
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 15 filas
  for(uint16_t y=0;y<15;y++) {
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
//  RoJoILI9486::drawPixel
void Test2() {
  //Limpiamos display
  display.clear();
  //Dibujamos una matriz de puntos
  for(uint32_t y=0;y<display.yMax();y+=5)
    for(uint32_t x=0;x<display.xMax();x+=5) display.drawPixel(x,y,x*y/3);
  delay(2000);
}

//Test de líneas
//  RoJoILI9486::line
void Test3() {
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint32_t color=display.getColor(0,255,0); //verde
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
//  RoJoILI9486::rect
void Test4() {
  //Limpiamos display
  display.clear();
  //Dibujamos un rectángulo relleno rojo
  display.block(20,20,180,240,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.rect(100,140,140,240,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.block(160,160,140,360,display.getColor(0,0,255));
  
  delay(1000);
}

//Test de triángulos
//  RoJoILI9486::triangle,triangleFill
void Test5() {
  //Limpiamos display
  display.clear();
  //Dibujamos un triángulo relleno rojo
  display.triangleFill(180,20,220,300,20,160,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  display.triangle(60,220,260,180,20,360,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  display.triangleFill(160,180,300,360,180,340,display.getColor(0,0,255));
    
  delay(1000);
}

//Test de circunferencias y círculos
//  RoJoILI9486::circle,disk
void Test6() {
  display.clear(); //Limpiamos display
  display.circle(50,50,45,display.getColor(255,0,0)); //Circunferencia
  display.disk(100,100,50,display.getColor(0,255,0)); //Círculo
  display.circle(200,200,100,display.getColor(0,0,255)); //Circunferencia
    
  delay(1000);
}

//Test de carga de sprite desde archivo
//  RoJoILI9486::drawSprite,drawBMP
void Test7() {
  //Dibujamos el sprite como una matriz de 10x15
  for(uint16_t y=0;y<15;y++) //Se arriba a abajo
    for(uint16_t x=0;x<10;x++) //De izquierda a derecha
      display.drawSprite("/heart.spr",x*32,y*32);
  delay(1000);

  //Cargamos varios sprites de 320x480
  display.drawSprite("/arbol.spr");
  delay(1000);
  //Dibujamos un bmp
  display.drawBMP("/campo.bmp");
  delay(1000);
}

//Test de texto
//  RoJoILI9486::printOver
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
  display.printOver("/10x15d.fon","1234567890",color,280,25); //No se ve el final
  //Fuente más grande
  display.printOver("/21x33d.fon","1234567890",color,0,45);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.printOver("/7x11.fon","Hola, esto es una prueba!",color,0,80);
  delay(2000);
}

//Test de rotación de pantalla
//  RoJoILI9486::rotation
void Test9() {
  uint32_t colorWhite=display.getColor(255,255,255);
  //Limpiamos pantalla
  display.clear();

  //Dibujamos el texto para cada rotación
  display.rotation(0);
  display.printOver(F("/7x11.fon"),F("Rotation 0"),colorWhite);
  display.rotation(1);
  display.printOver(F("/7x11.fon"),F("Rotation 1"),colorWhite);
  display.rotation(2);
  display.printOver(F("/7x11.fon"),F("Rotation 2"),colorWhite);
  display.rotation(3);
  display.printOver(F("/7x11.fon"),F("Rotation 3"),colorWhite);
  display.rotation(0);

  delay(2000);
}

//Test de color
//  RoJoILI9486::clear,drawSprite
//  RoJoSprite::setSize,end
void Test10() {
  //Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
  //en pantalla, formando una matriz de 3x3
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite; //Creamos el sprite
  sprite.setSize(32,32); //Lo dimensionamos

  //Creamos un array con los componentes de color (RGB) de cada una de las columnas
  byte color[7][3]= {
     {255,255,255} //col 0 = white
    ,{255,0,0}     //col 1 = red
    ,{0,255,0}     //col 2 = green
    ,{0,0,255}     //col 3 = blue
    ,{255,255,0}   //col 4 = yellow
    ,{255,0,255}   //col 5 = magenta
    ,{0,255,255}   //col 6 = cyan
  };
  //Recorremos las 15 filas
  for(uint16_t y=0;y<15;y++) {
    //Recorremos las 7 columnas
    for(uint16_t x=0;x<7;x++) {
      //Pintamos el sprite
      sprite.clear(display.getColor(color[x][0],color[x][1],color[x][2]));
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
//  RoJoSprite::loadSprite
void Test11() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Lo llenamos con una imagen de 32x32
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos el sprite como una matriz de 10x15
  for(uint16_t y=0;y<15;y++) //Se arriba a abajo
    for(uint16_t x=0;x<10;x++) //De izquierda a derecha
      display.drawSprite(&sprite,x*32,y*32);
  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de prites y texto
//  RoJoSprite::print
void Test12() {
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
  display.drawSprite(&sprite,280,25); //No se ve el final
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

//Test de cambio de tamaño
//  RoJoSprite::resize
void Test13() {
  //Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
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

//Test de líneas
//  RoJoSprite::line
void Test14() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  //Dibujamos líneas verdes
  uint32_t color=display.getColor(0,255,0); //Verde
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,0,x,sprite.xMax()-1,color);
  //Dibujamos líneas azules
  color=display.getColor(0,0,255); //azul
  for(int16_t x=0;x<(int16_t)sprite.xMax();x+=10) sprite.line(0,sprite.yMax()-1,x,0,color);
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//  RoJoSprite::block,rect
void Test15() {
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
//  RoJoSprite::triangle,triangleFill
void Test16() {
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
//  RoJoSprite::circle,disk
void Test17() {
  //Creamos un sprite de 200x200
  RoJoSprite sprite;
  sprite.setSize(200,200);
  sprite.circle(50,50,45,sprite.getColor(255,0,0)); //Circunferencia
  sprite.disk(100,100,50,sprite.getColor(0,255,0)); //Círculo
  sprite.circle(150,150,49,sprite.getColor(0,0,255)); //Circunferencia
  //Mostramos el sprite centrado
  display.drawSprite(&sprite,(display.xMax()-sprite.xMax())/2,(display.yMax()-sprite.yMax())/2);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de pixels
//  RoJoSprite::drawPixel
void Test18() {
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
//  RoJoSprite::save
void Test19() {
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

//Test de carga de sprites grandes desde archivo
//  RoJoSprite::loadSprite,loadBMP
void Test20() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Utilizaremos sprites de tamaño 320x480, igual que las dimensiones del display
  sprite.loadSprite("/arbol.spr");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadBMP("/campo.bmp");
  display.drawSprite(&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de reemplazar color y transparencia
//  RoJoSprite::replaceColor,drawSprite
void Test21() {
  //Creamos un sprite para el fondo
  RoJoSprite spriteBack;
  //Cargamos una imágen tan grande como el display
  spriteBack.loadSprite("/arbol.spr");
  //Creamos un sprite de 90x90
  RoJoSprite sprite;
  sprite.setSize(90,90);
  //Lo pintamos de rojo
  sprite.clear(sprite.getColor(255,0,0));
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(30,30,60,60,sprite.getColor(0,255,0));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,10,10);
  //Sustituimos el color verde por azul
  sprite.replaceColor(sprite.getColor(0,255,0),sprite.getColor(0,0,255));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,110,10);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(&sprite,210,10,spriteBack.getColor(0,0,255));
  //Lo mostramos
  display.drawSprite(&spriteBack);
  //Liberamos memoria de los sprites
  sprite.end();
  spriteBack.end();
    
  delay(1000);
}

//Test de copia parcial
//  RoJoSprite::copy
void Test22() {
  //Limpiamos display
  display.clear();
  //Creamos un sprite con una imágen del tamaño del display
  RoJoSprite back;
  back.loadSprite("/arbol.spr");
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
//  RoJoILI9486::drawSpriteSync
//  RoJoSprite::copy
void Test23() {
  //Se moverá un número por el display
  //El fondo estará fijo y será una imágen
  //Crearemos un sprite para el fondo y otro más pequeño para el número
  //Crearemos dos sprites más. Uno como memoria de vídeo de trabajo
  //y otro como memoria de vídeo del display.
  //La imágen final se compone sobre la memoria de vídeo de trabajo
  //Después se sincroniza con la memoria de vídeo del display y conseguimos
  //que sólo se envíen las diferencias.

  //Creamos el sprite con la imágen de fondo. Mismo tamaño que el display
  RoJoSprite back;
  back.loadSprite("/arbol.spr");
  //Creamos sprite con el número
  RoJoSprite num;
  //Color de texto: rojo. Fondo: negro. Borde: blanco
  num.print("/21x33d.fon","37",num.getColor(255,0,0),0,num.getColor(255,255,255));
  //Creamos sprite con memoria de vídeo de trabajo y copiamos la imágen de fondo
  RoJoSprite workMem;
  workMem.copy(&back);
  //Creamos sprite con memoria de vídeo de display y copiamos la imágen de fondo
  RoJoSprite displayMem;
  displayMem.copy(&back);
  //Para que la memoria de vídeo del display coincida con la realidad, la
  //dibujamos en el display
  display.drawSprite(&displayMem);
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
  while(millis()<maxTime) {
    //Calculamos nuevas coordenadas
    x+=dx;
    y+=dy;
    //Si hemos llegado a algún borde...cambiamos de dirección
    if(x<=0 || x>=xBounce) dx=-dx;
    if(y<=0 || y>=yBounce) dy=-dy;
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
  //Serial.begin(115200); delay(1000);  //DEBUG
  display.begin(pinRES_display,pinDC_display,pinCS_display); //Inicializamos el display
}

void loop() {
  //Tests de funciones de dibujo directo desde el driver
  Test1(); //Test de block
  Test2(); //Test de pixels
  Test3(); //Test de líneas
  Test4(); //Test de rectángulos
  Test5(); //Test de triángulos
  Test6(); //Test de circunferencias y círculos
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
        Test17(); //Test de circunferencias y círculos
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
