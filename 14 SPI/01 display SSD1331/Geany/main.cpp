/*
  Autor: Ramón Junquera
  Fecha: 20190627
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
#include <RoJoSprite16.h> //Gestión de sprites color
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

void setup()
{
  Serial.begin(115200); //DEBUG
  //Inicializamos el display
  display.begin(pinRES,pinDC,pinCS);
}

//Test de color
//Ejemplo de funciones:
//  RoJoSSD1331
//    clear
//    drawSprite
//  RoJoSprite16
//    setSize
//    end
//Creamos un sprite. Lo vamos pintando de distintos colores y lo mostramos
//en pantalla, formando una matriz de 3x2
void Test0()
{
  display.clear(); //Limpiamos pantalla
  RoJoSprite16 sprite; //Creamos el sprite
  sprite.setSize(32,32); //Lo dimensionamos

  sprite.clear(sprite.getColor(255,0,0)); //Rojo
  display.drawSprite(0,0,&sprite);

  sprite.clear(sprite.getColor(0,255,0)); //Verde
  display.drawSprite(32,0,&sprite);

  sprite.clear(sprite.getColor(0,0,255)); //Azul
  display.drawSprite(64,0,&sprite);

  sprite.clear(sprite.getColor(255,255,0)); //Amarillo
  display.drawSprite(0,32,&sprite);

  sprite.clear(sprite.getColor(255,0,255)); //Magenta
  display.drawSprite(32,32,&sprite);

  sprite.clear(sprite.getColor(0,255,255)); //Cyan
  display.drawSprite(64,32,&sprite);

  sprite.end(); //Liberamos memoria del sprite

  delay(1000);
}

//Test de carga de sprite pequeño desde archivo
//Ejemplo de funciones:
//  RoJoSprite16
//    load
void Test1()
{
  //Creamos el sprite
  RoJoSprite16 sprite;
  //Lo llenamos con una imagen de 32x32
  sprite.load(F("/heart.s16"));
  //Dibujamos el sprite como una matriz de 3x2
  for(byte y=0;y<2;y++) //Se arriba a abajo
    for(byte x=0;x<3;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,&sprite);
  //Liberamos la memoria del sprite
  sprite.end();
  //Esperamos
  delay(1000);
}

//Test de carga de sprites grandes desde archivo
//Ejemplo de funciones:
//  RoJoSprite16
//    load
//    loadBMP
void Test2()
{
  //Creamos el sprite
  RoJoSprite16 sprite;
  //Utilizaremos sprites de tamaño 96x64, igual que las dimensiones del display
  sprite.loadBMP("/rose.bmp"); //Cargamos imagen desde una archivo .bmp
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.load("/rainbow.s16");
  display.drawSprite(0,0,&sprite);
  delay(1000);
  sprite.load("/mountain.s16");
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
  for(byte z=3;z<64;z++)
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

  delay(1000);
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
  uint16_t color = sprite.getColor(0,255,0); //Verde
  for(int16_t x=0;x<display.xMax();x+=10) sprite.line(0,0,x,display.yMax()-1,color);
  //Dibujamos líneas azules
  color = sprite.getColor(0,0,255); //azul
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
  sprite.block(10,10,40,30,sprite.getColor(255,0,0));
  //Dibujamos un rectángulo sin relleno verde
  sprite.rect(30,20,60,40,sprite.getColor(0,255,0));
  //Dibujamos un rectángulo relleno azul
  sprite.block(50,30,80,50,sprite.getColor(0,0,255));
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
  sprite.triangleFill(20,0,50,40,0,20,sprite.getColor(255,0,0));
  //Dibujamos un triángulo sin relleno verde
  sprite.triangle(20,40,80,10,40,60,sprite.getColor(0,255,0));
  //Dibujamos un triángulo relleno azul
  sprite.triangleFill(50,10,90,30,70,60,sprite.getColor(0,0,255));
  //Mostramos el sprite a pantalla completa
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
  delay(1000);
}

//Test de círculos y elipses
//Ejemplo de funciones:
//  RoJoSprite16
//    disk
//    circle
//    ellipse
//    ellipseFill
void Test7()
{
  //Creamos un sprite tan grande como el display
  RoJoSprite16 sprite;
  sprite.setSize(display.xMax(),display.yMax());

  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  sprite.disk(15,15,14,fillColor);
  sprite.circle(15,45,14,borderColor);
  sprite.ellipseFill(60,15,29,14,fillColor);
  sprite.ellipse(60,45,29,14,borderColor);

  //Mostramos el sprite a pantalla completa
  display.drawSprite(0,0,&sprite);
  //Liberamos memoria del sprite
  sprite.end();
    
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
  for(byte y=0;y<display.yMax();y+=5)
    for(byte x=0;x<display.xMax();x+=5)
      sprite.drawPixel(x,y,sprite.getColor(x*2,y*4,0));
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
  sprite1.clear(sprite1.getColor(255,0,0));
  //Pintamos un cuadrado verde de 2x2 en la parte inferior derecha
  sprite1.block(1,1,2,2,sprite1.getColor(0,255,0));
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
  spriteBack.load("/mountain.s16");
  //Creamos un sprite de 25x25
  RoJoSprite16 sprite;
  sprite.setSize(25,25);
  //Lo pintamos de rojo
  sprite.clear(sprite.getColor(255,0,0));
  //Pintamos un cuadrado verde en la parte inferior derecha
  sprite.block(10,10,24,24,sprite.getColor(0,255,0));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(5,10,&sprite);
  //Sustituimos el color verde por azul
  sprite.replaceColor(sprite.getColor(0,255,0),sprite.getColor(0,0,255));
  //Lo dibujamos sobre el otro sprite
  spriteBack.drawSprite(35,10,&sprite);
  //Lo volvemos a dibujar, pero tomaremos el azul como transparente
  spriteBack.drawSprite(65,10,&sprite,sprite.getColor(0,0,255));
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
  //Creamos un sprite para dibujar el texto
  RoJoSprite16 sprite;
  //Definimos color de texto
  uint16_t color = sprite.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  sprite.print(F("/5x7d.fon"),F("20171028"),color);
  display.drawSprite(0,0,&sprite);
  //Utilizaremos otra fuente más grande
  sprite.print(F("/10x15d.fon"),F("20171028"),color);
  display.drawSprite(0,10,&sprite);
  //Cambiamos el color del texto
  color=sprite.getColor(255,255,0); //yellow
  //Creamos el color de fondo
  uint16_t backColor=sprite.getColor(0,0,64); //Azul oscuro
  //Imprimimos fuera de pantalla
  sprite.print(F("/10x15d.fon"),F("1234567890"),color,backColor);
  display.drawSprite(20,28,&sprite); //No se ve el final
  //Texto con fondo negro y bordes blancos
  sprite.print(F("/10x15d.fon"),F("1234567890"),color,0,0xFFFF);
  display.drawSprite(-33,46,&sprite); //No se ve el principio
  //Liberamos memoria del sprite
  sprite.end();

  delay(2000);
}

//Test de sincronización
//Ejemplo de funciones:
//  RoJoSSD1331
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

  //Creamos el sprite con la imágen de fondo. Mismo tamaño que el display
  RoJoSprite16 back;
  back.load("/mountain.s16");
  //Creamos sprite con el número
  RoJoSprite16 num;
  //Color de texto: rojo. Fondo: negro. Borde: blanco
  num.print("/21x33d.fon","37",back.getColor(255,0,0),0,0xFFFF);
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
  int8_t x=0;
  int8_t y=0;
  //Dirección:
  int8_t dx=1;
  int8_t dy=1;
  //Tiempo límite (duración = 5s)
  uint32_t maxTime=millis()+5000;
  //Mientras no superemos el tiempo límite
  while(millis()<maxTime)
  {
    //Calculamos nuevas coordenadas
    x+=dx;
    y+=dy;
    //Si hemos llegado a algún borde...cambiamos de dirección
    if(x==0 || (byte)x==display.xMax()-num.xMax()) dx=-dx;
    if(y==0 || (byte)y==display.yMax()-num.yMax()) dy=-dy;
    //Dibujamos la imágen de fondo sobre el sprite de trabajo
    workMem.drawSprite(0,0,&back);
    //Añadimos el número sobre el fondo, haciendo su fondo negro transparente
    workMem.drawSprite(x,y,&num,0);
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
  back.load("/mountain.s16");
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
//  RoJoSSD1331
//    clear
//    block
//Dibujamos bloques (rectángulos rellenos) de distintos colores y los mostramos
//en pantalla, formando una matriz de 3x2
void Test14()
{
  display.clear(); //Limpiamos pantalla
  display.block(0,0,31,31,display.getColor(255,0,0)); //RoJo
  display.block(32,0,63,31,display.getColor(0,255,0)); //Verde
  display.block(64,0,95,31,display.getColor(0,0,255)); //Azul
  display.block(0,32,31,63,display.getColor(255,255,0)); //Amarillo
  display.block(32,32,63,63,display.getColor(255,0,255)); //Magenta
  display.block(64,32,96,63,display.getColor(0,255,255)); //Cyan

  delay(1000);
}

//Test de pixels
//Ejemplo de funciones:
//  RoJoSSD1331
//    drawPixel
void Test15()
{
  //Limpiamos display
  display.clear();
  //Dibujamos una matriz de puntos
  for(byte y=0;y<display.yMax();y+=5)
    for(byte x=0;x<display.xMax();x+=5)
      display.drawPixel(x,y,display.getColor(x*2,y*4,0));
    
  delay(1000);
}

//Test de líneas
//Ejemplo de funciones:
//  RoJoSSD1331
//    line
void Test16()
{
  //Limpiamos display
  display.clear();
  //Dibujamos líneas verdes
  uint16_t color = display.getColor(0,255,0); //verde
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,0,x,display.yMax()-1,color);
  //Dibujamos líneas azules
  color = display.getColor(0,0,255); //azul
  for(int16_t x=0;x<display.xMax();x+=10) display.line(0,display.yMax()-1,x,0,color);
    
  delay(1000);
}

//Test de carga de sprite desde archivo
//Ejemplo de funciones:
//  RoJoSSD1331
//    drawSprite
void Test17()
{
  //Dibujamos el sprite como una matriz de 4x4
  for(byte y=0;y<2;y++) //Se arriba a abajo
    for(byte x=0;x<3;x++) //De izquierda a derecha
      display.drawSprite(x*32,y*32,"/heart.s16");
  //Esperamos
  delay(1000);

  //Cargamos varios sprites de 96x64
  display.drawSprite(0,0,"/rainbow.s16");
  delay(1000);
  display.drawSprite(0,0,"/mountain.s16");
  delay(1000);
}

//Test de texto
//Ejemplo de funciones:
//  RoJoSSD1331
//    print
void Test18()
{
  //Limpiamos pantalla
  display.clear();
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Escribimos un texto y lo mostramos
  display.print(0,0,F("/5x7d.fon"),F("20171028"),color);
  //Utilizaremos otra fuente más grande
  display.print(0,10,F("/10x15d.fon"),F("20171028"),color);
  //Cambiamos el color del texto
  color=display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  display.print(0,26,F("/10x15d.fon"),F("1234567890"),color); //No se ve el final
  //Texto con fondo negro y bordes blancos
  display.print(-33,42,F("/10x15d.fon"),F("1234567890"),color); //No se ve el principio

  delay(2000);
}

//Test de rectángulos
//Ejemplo de funciones:
//  RoJoSSD1331
//    rect
void Test19()
{
  //Limpiamos pantalla
  display.clear();
  
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  //Dibujamos una matriz de 5x5 rectángulos con borde y relleno
  for(int x=0;x<50;x+=10)
    for(int y=0;y<50;y+=10)
      display.rect(x,y,x+8,y+8,borderColor,fillColor);
  //Dibujamos una columna de 5 rectángulos con borde y relleno
  //parcial o totalmente fuera de pantalla
  for(int y=-40;y<70;y+=30)
    display.rect(70,y,100,y+28,borderColor,fillColor);
  
  borderColor = display.getColor(255,0,0); //red
  //Dibujamos un rectángulo sin relleno sobre lo anterior
  //para comprobar que el relleno es transparente
  display.rect(15,15,35,35,borderColor);
  //Dibujamos 2 rectángulos sin relleno parcialmente visibles
  for(int x=-10;x<60;x+=30)
    display.rect(x,60,x+28,90,borderColor);

  delay(1000);
}

//Test de círculos y elipses
//Ejemplo de funciones:
//  RoJoSSD1331
//    disk
//    circle
//    ellipse
//    ellipseFill
void Test20()
{
  display.clear();
  uint16_t borderColor = display.getColor(255,255,255); //white
  uint16_t fillColor = display.getColor(0,0,255); //blue
  display.disk(15,15,14,fillColor);
  display.circle(15,45,14,borderColor);
  display.ellipseFill(60,15,29,14,fillColor);
  display.ellipse(60,45,29,14,borderColor);
  delay(1000);
}

//Test de copia
//Ejemplo de funciones:
//  RoJoSSD1331
//    copy
void Test21()
{
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo de borde blanco y relleno azul
  display.rect(0,0,8,8,255,255,255,0,0,255);
  //Lo copiamos 5 veces
  for(int i=0;i<5;i++) display.copy(0,0,8,8,i*10,10);

  delay(1000);
}

//Test darker
//Ejemplo de funciones:
//  RoJoSSD1331
//    darker
void Test22()
{
  //Limpiamos pantalla
  display.clear();
  //Dibujamos un rectángulo de borde blanco y relleno azul
  display.rect(0,0,50,50,255,255,255,0,0,255);
  //Oscurecemos dos areas que se solapan
  display.darker(10,10,30,30);
  display.darker(20,20,40,40);
  //Comprobamos que una zona oscurecida ya no se oscurece más

  delay(1000);
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
      Test7(); //Test de círculos y elipses
      Test8(); //Test de pixels
    #endif
      Test9(); //Test de guardado
    //Arduino Mega no tiene memoria suficiente para cargar sprites grandes
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
  Test17(); //Test de carga de sprite desde archivo
  Test18(); //Test de texto
  Test19(); //Test de rectángulos
  Test20(); //Test de círculos y elipses
  Test21(); //Test de copia
  Test22(); //Test darker
}
