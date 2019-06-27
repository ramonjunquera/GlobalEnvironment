/*
  Autor: Ramón Junquera
  Fecha: 20190625
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
#include <RoJoSprite16.h> //Gestión de sprites color
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

void setup()
{
  Serial.begin(115200); //DEBUG
  //Inicializamos el display
  display.begin(pinRES,pinDC,pinCS);
}

//Test de color
//Ejemplo de funciones:
//  RoJoSSD1351
//    clear
//    drawSprite
//  RoJoSprite16
//    setSize
//    end
//Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
//en pantalla, formando una matriz de 3x3
void Test0()
{
  display.clear(); //Limpiamos pantalla
  RoJoSprite16 sprite; //Creamos el sprite
  sprite.setSize(42,42); //Lo dimensionamos

  sprite.clear(display.getColor(127,127,127)); //Gris
  display.drawSprite(42,0,&sprite);
  
  sprite.clear(display.getColor(255,255,255)); //Blanco
  display.drawSprite(84,0,&sprite);

  sprite.clear(display.getColor(255,0,0)); //Rojo
  display.drawSprite(0,42,&sprite);

  sprite.clear(display.getColor(0,255,0)); //Verde
  display.drawSprite(42,42,&sprite);

  sprite.clear(display.getColor(0,0,255)); //Azul
  display.drawSprite(84,42,&sprite);

  sprite.clear(display.getColor(255,255,0)); //Amarillo
  display.drawSprite(0,84,&sprite);

  sprite.clear(display.getColor(255,0,255)); //Magenta
  display.drawSprite(42,84,&sprite);

  sprite.clear(display.getColor(0,255,255)); //Cyan
  display.drawSprite(84,84,&sprite);

  sprite.end(); //Liberamos memoria del sprite

  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//Ejemplo de funciones:
//  RoJoSprite16
//    load
//    loadBMP
void Test1()
{
  //Creamos el sprite
  RoJoSprite16 sprite;

  //Dibujamos dos líneas con una imágen de 32x32 tomada de un archivo de sprite
  sprite.load(F("/heart.s16"));
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=0;y<2;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  //Dibujamos otras dos líneas con una imágen de 32x32 tomada de un archivo bmp
  sprite.loadBMP(F("/heart.bmp"));
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=2;y<4;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de carga de sprites grandes desde archivo
//Ejemplo de funciones:
//  RoJoSprite16
//    load
void Test2()
{
  //Creamos el sprite
  RoJoSprite16 sprite;
  //Utilizaremos sprites de tamaño 128x128, igual que las dimensiones del display
  sprite.load("/palmblue.s16");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.load("/palmyell.s16");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.load("/sea.s16");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  //Repetimos la secuencia cargando desde archivos bmp
  sprite.loadBMP("/palmblue.bmp");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.loadBMP("/palmyell.bmp");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.loadBMP("/sea.bmp");
  display.drawSprite(0,0,&sprite);
  //Liberamos la memoria del sprite
  sprite.end();

  delay(1000);
}

//Test de cambio de tamaño
//Ejemplo de funciones:
//  RojoSprite16
//    resize
//Cargamos una imagen y vamos aumentando su tamaño hasta ocupar la pantalla completa
void Test3()
{
  //Limpiamos pantalla
  display.clear();
  //Creamos el sprite original
  RoJoSprite16 spriteOri;
  //Lo llenamos con una imagen de 32x32
  spriteOri.load(F("/heart.s16"));
  //Creamos el sprite redimensionado
  RoJoSprite16 spriteZoom;
  for(byte z=32;z<128;z+=5)
  {
    //Redimensionamos el sprite
    spriteZoom.resize(z,z,&spriteOri);
    //Lo mostramos
    display.drawSprite(0,0,&spriteZoom);
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
void Test4()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos líneas verdes
  uint16_t color = display.getColor(0,255,0); //Verde
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,0,x,display.xMax()-1,color);
  //Dibujamos líneas azules
  color = display.getColor(0,0,255); //azul
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,display.yMax()-1,x,0,color);
  //Mostramos el sprite que ocupa todo el display
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoSprite16
//    block
//    rect
void Test5()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un rectángulo relleno rojo
  sprite.block(0,0,100,50,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(50,25,110,100,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(80,80,127,127,display.getColor(0,0,255));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoSprite16
//    triangle
//    triangleFill
void Test6()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos un triángulo relleno rojo
  sprite.triangleFill(70,0,40,90,0,30,display.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(0,60,90,50,30,120,display.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(127,70,120,127,10,70,display.getColor(0,0,255));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de circunferencias, círculos y elipses
//Ejemplo de funciones:
//  RoJoSprite16
//    circle
//    disk
void Test7()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());

  sprite.circle(50,50,45,display.getColor(255,0,0)); //Circunferencia
  sprite.disk(70,70,30,display.getColor(0,255,0)); //Círculo
  sprite.circle(90,90,35,display.getColor(0,0,255)); //Circunferencia
  display.drawSprite(0,0,&sprite); //Mostramos el sprite a pantalla completa

  delay(1000);

  sprite.clear();
  sprite.ellipse(55,40,45,30,display.getColor(255,0,0)); //Circunferencia
  sprite.ellipseFill(50,65,20,45,display.getColor(0,255,0)); //Círculo
  sprite.ellipse(70,100,50,20,display.getColor(0,0,255)); //Circunferencia
  display.drawSprite(0,0,&sprite); //Mostramos el sprite a pantalla completa

  sprite.end(); //Liberamos memoria del sprite
  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoSprite16
//    drawPixel
void Test8()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=10)
    for(byte x=0;x<display.xMax();x+=10)
      sprite.drawPixel(x,y,sprite.getColor(x*2,y*2,0));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de guardado
//Ejemplo de funciones:
//  RoJoSprite16
//    save
void Test9()
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
  sprite1.resize(50,50,&sprite2);
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

//Test de reemplazar color y transparencia
//Ejemplo de funciones:
//  RoJoSprite16
//    replaceColor
//    drawSprite
void Test10()
{
  //Creamos un sprite para el fondo
  RoJoSprite16 spriteBack;
  //Cargamos una imágen tan grande como el display
  spriteBack.load("/palmyell.s16");
  //Creamos un sprite de 30x30
  RoJoSprite16 sprite;
  sprite.setSize(30,30);
  //Lo pintamos de rojo
  sprite.clear(display.getColor(255,0,0));
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(10,10,29,29,display.getColor(0,255,0));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(10,10,&sprite);
  //Sustituimos el color verde por azul
  sprite.replaceColor(display.getColor(0,255,0),display.getColor(0,0,255));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(50,10,&sprite);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(90,10,&sprite,display.getColor(0,0,255));
  //Lo mostramos
  display.drawSprite(0,0,&spriteBack);
  //Liberamos memoria de los sprites
  sprite.end();
  spriteBack.end();
    
  delay(1000);
}

//Test de texto
//Ejemplo de funciones:
//  RoJoSprite16
//    print
void Test11()
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
  display.drawSprite(100,25,&sprite); //No se ve el final
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

//Test de sincronización
//Ejemplo de funciones:
//  RoJoSSD1351
//    drawSpriteSync
//  RojoSprite16
//    copy
void Test12()
{
  //Se moverá un número por el display
  //El fondo estará fijo y será una imágen
  //Crearemos un sprite para el fondo y otro más pequeño para el número
  //Crearemos dos sprites más. Uno como memoria de vídeo de trabajo
  //y otro como memoria de vídeo del display.
  //Las imágen final se compone sobre la memoria de vídeo de trabajo
  //Después se sincroniza con la memoria de vídeo del display y conseguimos
  //que sólo se envíen las diferencias.
  //Esta técnica de doble buffer no es aconsejable si los datos gráficos
  //de los sprites se guardan en archivo

  //Creamos el sprite con la imágen de fondo
  RoJoSprite16 back;
  back.load("/palmblue.s16");
  //Creamos sprite con el número
  RoJoSprite16 num;
  //Color de texto: rojo. Fondo: negro. Borde: blanco
  num.print("/21x33d.fon","3",display.getColor(255,0,0),0,0xFFFF);
  //Creamos sprite con memoria de vídeo de trabajo con el tamaño del display
  RoJoSprite16 workMem;
  workMem.setSize(display.xMax(),display.yMax());
  //Creamos sprite con memoria de vídeo de display con el tamaño del display
  //Lo dimensionamos copiando el sprite de fondo
  RoJoSprite16 displayMem;
  displayMem.copy(&back);
  //Para que la memoria de vídeo del display coincida con la realidad, la
  //dibujamos en el display
  display.drawSprite(0,0,&displayMem);
  //Desplazamiento
  for(int16_t d=-25;d<display.xMax();d++)
  {
    //Dibujamos la imágen de fondo sobre el sprite de trabajo
    workMem.drawSprite(0,0,&back);
    //Añadimos el número sobre haciendo su fondo negro transparente
    workMem.drawSprite(d,d,&num,0);
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

//Test de copia parcial
//Ejemplo de funciones:
//  RoJoSprite16
//    copy
void Test13()
{
  //Limpiamos display
  display.clear();
  //Creamos un sprite con una imágen del tamaño del display
  RoJoSprite16 back;
  back.load("/sea.s16");
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

//Test de color
//Ejemplo de funciones:
//  RoJoSSD1351
//    clear
//    block
//Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
//en pantalla, formando una matriz de 3x3
void Test14()
{
  display.clear(); //Limpiamos pantalla
  display.block(42,0,83,41,display.getColor(127,127,127)); //Gris
  display.block(84,0,125,41,display.getColor(255,255,255)); //Blanco
  display.block(0,42,41,83,display.getColor(255,0,0)); //RoJo
  display.block(42,42,83,83,display.getColor(0,255,0)); //Verde
  display.block(84,42,125,83,display.getColor(0,0,255)); //Azul
  display.block(0,84,41,125,display.getColor(255,255,0)); //Amarillo
  display.block(42,84,83,125,display.getColor(255,0,255)); //Magenta
  display.block(84,84,125,125,display.getColor(0,255,255)); //Cyan

  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoSSD1351
//    drawPixel
void Test15()
{
  //Limpiamos display
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=10)
    for(byte x=0;x<display.xMax();x+=10)
      display.drawPixel(x,y,display.getColor(x*2,y*2,0));
    
  delay(1000);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoSSD1351
//    line
void Test16()
{
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint16_t color = display.getColor(0,255,0); //verde
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,0,x,display.xMax()-1,color);
  //Dibujamos líneas azules
  color = display.getColor(0,0,255); //azul
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,display.xMax()-1,x,0,color);
    
  delay(1000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoSSD1351
//    rect
void Test17()
{
  //Limpiamos display
  display.clear();
  //Dibujamos un rectángulo relleno rojo
  display.block(10,10,80,80,display.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  display.rect(50,50,100,100,display.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  display.block(70,70,120,120,display.getColor(0,0,255));
  
  delay(1000);
}

//Test de triángulos
//Ejemplo de funciones:
//  RoJoSSD1351
//    triangle
//    triangleFill
void Test18()
{
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
//Ejemplo de funciones:
//  RoJoSSD1351
//    circle
//    disk
void Test19()
{
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

//Test de carga de sprite desde archivo
//Ejemplo de funciones:
//  RoJoSSD1351
//    drawSprite
void Test20()
{
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=0;y<4;y++) //Se arriba a abajo
    for(byte x=0;x<4;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,"/heart.s16");
  //Esperamos
  delay(1000);

  //Cargamos varios sprites de 128x128
  display.drawSprite(0,0,"/palmblue.s16");
  delay(1000);
  display.drawSprite(0,0,"/palmyell.s16");
  delay(1000);
  display.drawSprite(0,0,"/sea.s16");
  delay(1000);
}

//Test de texto
//Ejemplo de funciones:
//  RoJoSSD1351
//    print
void Test21()
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
  display.print(100,25,"/10x15d.fon","1234567890",color); //No se ve el final
  //Fuente más grande
  display.print(0,45,"/21x33d.fon","1234567890",color);
  //Cambiamos el color
  color=display.getColor(255,0,0); //red
  display.print(0,80,"/7x11.fon","Hola, esto es una prueba!",color);

  delay(2000);
}

void loop()
{
  //Los dispositivos Arduino UNO y NANO no tienen memoria suficiente
  //para trabajar con sprites
  #if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO) 
    Test0(); //Test de color
    Test1(); //Test de carga de sprite pequeño desde archivo
    //Arduino Mega no tiene memoria suficiente para cargar sprites grandes
    #ifndef ARDUINO_AVR_MEGA2560
      Test2(); //Test de carga de sprites grandes desde archivo
      Test3(); //Test de cambio de tamaño
      Test4(); //Test de líneas
      Test5(); //Test de rectángulos
      Test6(); //Test de triángulos
      Test7(); //Test de circunferencias, círculos y elipses
      Test8(); //Test de pixels
    #endif
    Test9(); //Test de guardado
    //Arduino Mega no tienen memoria suficiente para cargar sprites grandes
    #ifndef ARDUINO_AVR_MEGA2560 
      Test10(); //Test de reemplazar color y transparencia
      Test11(); //Test de texto
      #ifndef ESP8266 //ESP8266 no tiene suficiente memoria para doble buffer
        Test12(); //Test de sincronización
      #endif
      Test13(); //Test de copia parcial
    #endif
  #endif
  //Tests de funciones de dibujo directo desde el driver
  Test14(); //Test de block
  Test15(); //Test de pixels
  Test16(); //Test de líneas
  Test17(); //Test de rectángulos
  Test18(); //Test de triángulos
  Test19(); //Test de circunferencias, círculos y elipses
  Test20(); //Test de carga de sprite desde archivo
  Test21(); //Test de texto
}
