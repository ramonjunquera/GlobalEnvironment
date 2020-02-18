/*
  Autor: Ramón Junquera
  Fecha: 20200217
  Tema: Librería para display OLED SPI 0.95" 96x64 SSD1331
  Material: breadboard, cables, display OLED SSD1331
  Ejemplos: ESP32, ESP8266, Mega, UNO, Nano, Raspberry Pi

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D4
  Mega     52   50   51  53
  UNO/Nano 13   12   11  10
  RPi      11   9    10  8(CS0)

  En nuestro caso, aunque el dispositivo no devuelva nunca información
  (no necesita MISO), no podemos utilizar ese pin porque es utilizado
  por el protocolo SPI.
  
  Memoria de vídeo= 96*64*2=12Kb
  Sistema de doble buffer= memoria de vídeo *2=24Kb

  Nota 1:
    El sistema de archivos (SPIFFS/SD) debe contener los archivos de la
    carpeta data para su correcto funcionamiento.
    Para SPIFFS se pueden transferir con la tarea:
      PlatformIO: Upload File System image
  Nota 2:
    Teniendo en cuenta que el dispositivo Lolin32Lite no tiene pin de
    5V, sólo podremos utilizar lectores SD de 3.3V
  Nota 3:
    Configuración de Build de Geany para Raspberry Pi:
      Compile: g++ -Wall -std=c++11 -c "%f" -pthread -I"lib"
      Build: g++ -Wall -std=c++11 -o "%e" "%f" -pthread -I"lib"
      Execute: sudo "./%e"
  Nota 4:
    Con Lolin32 es obligatorio añadir al circuito una resistencia de 
    10 Kohm entre el pin 2 y GND, para poder transferir datos (programa
    o SPIFFS).
  Nota 5:
    No todos los lectores SD son compatibles.

  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include <RoJoSprite.h> //Gestión de sprites color
#include <RoJoSSD1331.h> //Librería de gestión del display SSD1331

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
RoJoSSD1331 display;

//Test de color
//  RoJoSSD1331 :: clear,block
void Test1() {
  //Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
  //en pantalla, formando una matriz de 3x2
  display.clear(); //Limpiamos pantalla
  display.block(0,0,31,31,{255,0,0}); //Rojo
  display.block(32,0,32+31,31,{0,255,0}); //Verde
  display.block(64,0,64+31,31,{0,0,255}); //Azul
  display.block(0,32,31,32+31,{255,255,0}); //Amarillo
  display.block(32,32,32+31,32+31,{255,0,255}); //Magenta
  display.block(64,32,64+31,32+31,{0,255,255}); //Cyan
  delay(1000);
}

//Test de pixels
//  RoJoSSD1331::drawPixel
void Test2() {
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=5)
    for(byte x=0;x<display.xMax();x+=5)
      display.drawPixel(x,y,{(byte)(x*2),(byte)(y*4),0});
  delay(1000);
}

//Test de líneas
//  RoJoSSD1331::line
void Test3() {
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  RoJoColor color={0,255,0}; //verde
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,0,x,display.yMax()-1,color);
  //Dibujamos líneas azules
  color={0,0,255}; //azul
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,display.yMax()-1,x,0,color);
    
  delay(1000);
}

//Test de rectángulos
//  RoJoSSD1331::rect
void Test4() {
  //Limpiamos pantalla
  display.clear();
  
  RoJoColor borderColor={255,255,255}; //white
  RoJoColor fillColor={0,0,255}; //blue
  //Dibujamos una matriz de 5x5 rectángulos con borde y relleno
  for(int x=0;x<50;x+=10)
    for(int y=0;y<50;y+=10)
      display.rect(x,y,x+8,y+8,borderColor,fillColor);
  //Dibujamos una columna de 5 rectángulos con borde y relleno
  //El primero y el último están totalmente fuera de pantalla
  //Los tres intermedios se ven parcialmente
  for(int y=-40;y<70;y+=30)
    display.rect(70,y,100,y+28,borderColor,fillColor);
  
  borderColor={255,0,0}; //red
  //Dibujamos un rectángulo sin relleno sobre lo anterior
  //para comprobar que el relleno es transparente
  display.rect(15,15,35,35,borderColor);
  //Dibujamos 3 rectángulos sin relleno parcialmente visibles
  for(int x=-10;x<60;x+=30)
    display.rect(x,60,x+28,90,borderColor);

  delay(1000);
}

//Test de triángulos
//  RoJoSSD1331::triangle,triangleFill
void Test5() {
  display.clear();
  //Dibujamos un triángulo relleno rojo
  display.triangleFill(20,0,50,40,0,20,{255,0,0});
  //Dibujamos un triángulo sin relleno verde
  display.triangle(20,40,80,10,40,60,{0,255,0});
  //Dibujamos un triángulo relleno azul
  display.triangleFill(50,10,90,30,70,60,{0,0,255});
    
  delay(1000);
}

//Test de círculos y elipses
//  RoJoSSD1331::disk,circle,ellipse,ellipseFill
void Test6() {
  display.clear();
  RoJoColor borderColor={255,255,255}; //white
  RoJoColor fillColor={0,0,255}; //blue
  display.disk(15,15,14,fillColor);
  display.circle(15,45,14,borderColor);
  display.ellipseFill(60,15,29,14,fillColor);
  display.ellipse(60,45,29,14,borderColor);
  delay(1000);
}

//Test darker
//  RoJoSSD1331::darker
void Test7() {
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo de borde blanco y relleno azul
  display.rect(0,0,50,50,{255,255,255},{0,0,255});
  //Oscurecemos dos areas que se solapan
  display.darker(10,10,30,30);
  display.darker(20,20,40,40);
  //Comprobamos que una zona oscurecida ya no se oscurece más

  delay(1000);
}

//Test de copia
//  RoJoSSD1331::copy
void Test8() {
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo de borde blanco y relleno azul
  display.rect(0,0,8,8,{255,255,255},{0,0,255});
  //Lo copiamos 5 veces
  for(int i=0;i<5;i++) display.copy(0,0,8,8,i*10,10);

  delay(1000);
}

//Test de dibujo desde archivo
//  RoJoSSD1331::drawSprite,drawBMP
void Test9() {
  //Dibujamos el sprite como una matriz de 3x2
  for(byte y=0;y<2;y++) //Se arriba a abajo
    for(byte x=0;x<3;x++) //De izquierda a derecha
      display.drawSprite("/heart.spr",x*32,y*32);
  delay(1000);

  //Cargamos varios sprites de 96x64
  display.drawSprite("/rainbow.spr");
  delay(1000);
  display.drawSprite("/mountain.spr");
  delay(1000);
  //Carhamos un BMP
  display.drawBMP("/rose.bmp");
  delay(1000);
}

//Test de texto
//  RoJoSSD1331::printOver
void Test10() {
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  RoJoColor color={0,255,0}; //green
  //Escribimos un texto y lo mostramos
  display.printOver(F("/5x7d.fon"),F("20190830"),color);
  //Utilizaremos otra fuente más grande
  display.printOver(F("/10x15d.fon"),F("20190830"),color,0,10);
  //Cambiamos el color del texto
  color={255,255,0}; //yellow
  //Imprimimos fuera de pantalla
  display.printOver(F("/10x15d.fon"),F("1234567890"),color,0,26); //No se ve el final
  display.printOver(F("/10x15d.fon"),F("1234567890"),color,-33,42); //No se ve el principio

  delay(1000);
}

//Test de color con sprites
//  RoJoSSD1331::drawSprite
//  RoJoSprite:setSize,end
void Test11() {
  //Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
  //en pantalla, formando una matriz de 3x2
  display.clear(); //Limpiamos pantalla
  RoJoSprite sprite; //Creamos el sprite
  sprite.setSize(32,32); //Lo dimensionamos
  sprite.clear({255,0,0}); //Rojo
  display.drawSprite(&sprite);
  sprite.clear({0,255,0}); //Verde
  display.drawSprite(&sprite,32,0);
  sprite.clear({0,0,255}); //Azul
  display.drawSprite(&sprite,64,0);
  sprite.clear({255,255,0}); //Amarillo
  display.drawSprite(&sprite,0,32);
  sprite.clear({255,0,255}); //Magenta
  display.drawSprite(&sprite,32,32);
  sprite.clear({0,255,255}); //Cyan
  display.drawSprite(&sprite,64,32);
  sprite.end(); //Liberamos memoria del sprite
  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//  RoJoSprite::loadSprite,loadBMP
void Test12() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Lo llenamos con una imagen de 32x32
  sprite.loadSprite(F("/heart.spr"));
  //Dibujamos 3 veces el sprite en la mitad superior
  for(byte x=0;x<3;x++) display.drawSprite(&sprite,x*32,0);
  //Cargamos la imagen desde un bmp
  sprite.loadBMP(F("/sun.bmp"));
  //Dibujamos 3 veces el sprite en la mitad inferior
  for(byte x=0;x<3;x++) display.drawSprite(&sprite,x*32,32);
  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de guardado
//  RoJoSprite::save
void Test13() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite
  RoJoSprite sprite1;
  //Tendrá un tamaño de 3x3
  sprite1.setSize(3,3);
  //Lo pintamos de rojo
  sprite1.clear({255,0,0});
  //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.block(1,1,2,2,{0,255,0});
  //Lo guardamos en un archivo
  sprite1.save("/test.spr");

  //Creamos un nuevo sprite
  RoJoSprite sprite2;
  //Cargamos la imágen desde un archivo
  sprite2.loadSprite("/test.spr");
  //Lo redimensionamos para que se vea claro
  sprite1.resize(&sprite2,50,50);
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

//Test de rotación
//  RoJoSprite::rotate
void Test14() {
  //Crearemos un sprite de texto y lo rotaremos
  display.clear();
  RoJoSprite sprite;
  sprite.print(F("/10x15d.fon"),F("123"),{255,255,0}); //Texto en amarillo
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
void Test15() {
  //Crearemos un sprite de texto y lo voltearemos
  display.clear();
  RoJoSprite sprite;
  sprite.print(F("/10x15d.fon"),F("123"),{255,255,0}); //Texto en amarillo
  display.drawSprite(&sprite);
  sprite.replaceColor({255,255,0},{155,155,0}); //Amarillo más ocuro
  RoJoSprite spriteFlip;
  spriteFlip.flipH(&sprite);
  display.drawSprite(&spriteFlip,33,0);
  spriteFlip.flipV(&sprite);
  display.drawSprite(&spriteFlip,0,16);
  spriteFlip.end();
  sprite.end();
  delay(2000);
}

//Test de carga de sprites grandes desde archivo
//  RoJoSprite: loadSprite,loadBMP
void Test16() {
  //Creamos el sprite
  RoJoSprite sprite;
  //Utilizaremos sprites de tamaño 96x64, igual que las dimensiones del display
  sprite.loadBMP("/rose.bmp"); //Cargamos imagen desde una archivo .bmp
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadSprite("/rainbow.spr");
  display.drawSprite(&sprite);
  delay(1000);
  sprite.loadSprite("/mountain.spr");
  display.drawSprite(&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de cambio de tamaño
//  RoJoSprite::resize
void Test17() {
  //Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.loadBMP(F("/sun.bmp"));
  //Creamos el sprite redimensionado
  RoJoSprite spriteZoom;
  for(byte z=3;z<64;z++) {
    //Redimensionamos el sprite
    spriteZoom.resize(&spriteOri,z,z);
    //Lo mostramos
    display.drawSprite(&spriteZoom);
  }
  //Liberamos memoria del sprite redimensionado
  spriteZoom.end();
  //Liberamos memoria del sprite original
  spriteOri.end();

  delay(1000);
}

//Test de pixels
//  RoJoSprite::drawPixel
void Test18() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=5)
    for(byte x=0;x<display.xMax();x+=5)
      sprite.drawPixel(x,y,{(byte)(x*2),(byte)(y*4),0});
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de líneas
//  RoJoSprite:line
void Test19() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos líneas verdes
  RoJoColor color={0,255,0}; //Verde
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,0,x,display.yMax()-1,color);
  //Dibujamos líneas azules
  color={0,0,255}; //azul
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,display.yMax()-1,x,0,color);
  //Mostramos el sprite que ocupa todo el display
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//  RoJoSprite::block,rect
void Test20() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un rectángulo relleno rojo
  sprite.block(10,10,40,30,{255,0,0});
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(30,20,60,40,{0,255,0});
  //Dibujamos un rectángulo relleno azul
  sprite.block(50,30,80,50,{0,0,255});
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de triángulos
//  RoJoSprite::triangle,triangleFill
void Test21() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(20,0,50,40,0,20,{255,0,0});
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(20,40,80,10,40,60,{0,255,0});
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(50,10,90,30,70,60,{0,0,255});
  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de círculos y elipses
//  RoJoSprite::disk,circle,ellipse,ellipseFill
void Test22() {
  //Creamos un sprite tan grande como el display
  RoJoSprite sprite;
  sprite.setSize(display.xMax(),display.yMax());

  RoJoColor borderColor={255,255,255}; //white
  RoJoColor fillColor={0,0,255}; //blue
  sprite.disk(15,15,14,fillColor);
  sprite.circle(15,45,14,borderColor);
  sprite.ellipseFill(60,15,29,14,fillColor);
  sprite.ellipse(60,45,29,14,borderColor);

  //Mostramos el sprite a pantalla completa
  display.drawSprite(&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de reemplazar color y transparencia
//  RoJoSprite::replaceColor,drawSprite
void Test23() {
  //Creamos un sprite para el fondo
  RoJoSprite spriteBack;
  //Cargamos una imágen tan grande como el display
  spriteBack.loadSprite("/mountain.spr");
  //Creamos un sprite de 25x25
  RoJoSprite sprite;
  sprite.setSize(25,25);
  //Lo pintamos de rojo
  sprite.clear({255,0,0});
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(10,10,24,24,{0,255,0});
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,5,10);
  //Sustituimos el color verde por azul
  sprite.replaceColor({0,255,0},{0,0,255});
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(&sprite,35,10);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(&sprite,65,10,{0,0,255});
  //Lo mostramos
  display.drawSprite(&spriteBack);
  //Liberamos memoria de los sprites
  sprite.end();
  spriteBack.end();
    
  delay(1000);
}


//Test de texto
//  RoJoSprite::print
void Test24() {
  //Limpiamos pantalla
  display.clear();
  //Creamos un sprite para dibujar el texto
  RoJoSprite sprite;
  //Definimos color de texto
  RoJoColor color={0,255,0}; //green
  //Escribimos un texto y lo mostramos
  sprite.print(F("/5x7d.fon"),F("20190930"),color);
  display.drawSprite(&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print(F("/10x15d.fon"),F("20190830"),color);
  display.drawSprite(&sprite,0,10);
  //Cambiamos el color del texto
  color={255,255,0}; //yellow
  //Creamos el color de fondo
  RoJoColor backColor={0,0,64}; //Azul oscuro
  //Imprimimos fuera de pantalla
  sprite.print(F("/10x15d.fon"),F("1234567890"),color,backColor);
  display.drawSprite(&sprite,20,28); //No se ve el final
  //Texto con fondo negro y bordes blancos
  sprite.print(F("/10x15d.fon"),F("1234567890"),color,{0,0,0},{255,255,255});
  display.drawSprite(&sprite,-33,46); //No se ve el principio
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de copia parcial
//  RoJoSprite::copy
void Test25() {
  //Limpiamos display
  display.clear();
  //Creamos un sprite con una imágen del tamaño del display
  RoJoSprite back;
  back.loadSprite("/mountain.spr");
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
//  RoJoSSD1331::drawSpriteSync
//  RoJoSprite::copy
void Test26() {
  //Se moverá un número por el display
  //El fondo estará fijo y será una imágen
  //Crearemos un sprite para el fondo y otro más pequeño para el número
  //Crearemos dos sprites más. Uno como memoria de vídeo de trabajo
  //y otro como memoria de vídeo del display.
  //Las imágen final se compone sobre la memoria de vídeo de trabajo
  //Después se sincroniza con la memoria de vídeo del display y conseguimos
  //que sólo se envíen las diferencias.

  //Creamos el sprite con la imágen de fondo. Mismo tamaño que el display
  RoJoSprite back;
  back.loadSprite("/mountain.spr");
  //Creamos sprite con el número
  RoJoSprite num;
  //Color de texto: rojo. Fondo: negro. Borde: blanco
  num.print("/21x33d.fon","37",{255,0,0},{0,0,0},{255,255,255});

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
  int8_t x=0,y=0;
  //Dirección:
  int8_t dx=2,dy=2;
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
    workMem.drawSprite(&num,x,y,{0,0,0});
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
  display.begin(pinRES,pinDC,pinCS); //ok
}

void loop() {
  Test1(); //Test de color
  Test2(); //Test de pixels
  Test3(); //Test de líneas
  Test4(); //Test de rectángulos
  Test5(); //Test de triángulos
  Test6(); //Test de círculos y elipses
  Test7(); //Test darker
  Test8(); //Test de copia
  Test9(); //Test de dibujo de sprite desde archivo
  Test10(); //Test de texto
  //Los dispositivos Arduino UNO y NANO no tienen memoria suficiente
  //para trabajar con sprites
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) 
    Test11(); //Test de color
    Test12(); //Test de carga de sprite pequeño desde archivo
    Test13(); //Test de guardado
    Test14(); //Test de rotación
    Test15(); //Test de flip
    //Arduino Mega no tiene memoria suficiente para cargar sprites grandes
    #ifndef ARDUINO_AVR_MEGA2560
      Test16(); //Test de carga de sprites grandes desde archivo
      Test17(); //Test de cambio de tamaño
      Test18(); //Test de pixels
      Test19(); //Test de líneas
      Test20(); //Test de rectángulos
      Test21(); //Test de triángulos
      Test22(); //Test de círculos y elipses
      Test23(); //Test de reemplazar color y transparencia
      Test24(); //Test de texto
      Test25(); //Test de copia parcial
      Test26(); //Test de sincronización
    #endif
  #endif
  //Tests de funciones de dibujo directo desde el driver
  
}
