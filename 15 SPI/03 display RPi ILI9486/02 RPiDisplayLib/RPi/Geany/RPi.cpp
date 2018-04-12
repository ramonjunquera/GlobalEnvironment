/*
  Autor: Ramón Junquera
  Fecha: 20180410
  Tema: Librería para display RPI_ILI9486 SPI 3.5" 480*320
  Objetivo: Demo de librería RoJoILI9486
  Material: breadboard, cables, placa RPi, display ILI9486

  Descripción:
  Descripción y demostración de funcionalidades de la librería.

  Pinout SPI:

  Modelo   CLK MISO MOSI CS
  -------  --- ---- ---- --
  ESP32    18   19   23  5
  ESP8266  D5   D6   D7  D8
  RPi      11   9    10  8(CS0)
  
  Este display está especialmente preparado para ser integrado en
  Raspberry Pi con una caja que hace de soporte. La conexión es directa.
  Los pines no pueden ser seleccionados.
  En el desarrollo se deben utilizar los siguientes pines como fijos:
  
  const byte pinDC_display=24;
  const byte pinRES_display=25;
  const byte pinCS_display=8;
 
  Resultado:
  Realizamos varios tests cíclicos
*/

#include <Arduino.h>
#include "RoJoRPiILI9486.cpp" //Librería de gestión del display ILI9486
#include "RoJoSprite16.cpp" //Gestión de sprites color
#include "RoJoABC16.cpp" //Gestión de fuentes color

//Definición de pines
const byte pinDC_display=24;
const byte pinRES_display=25;
const byte pinCS_display=8;

//Creamos objeto de gestión
RoJoRPiILI9486 display;

void Test0()
{
  //Test de color

  //Limpiamos pantalla
  display.videoMem->clear();
  //Definimos colores
  uint16_t redColor = display.getColor(255,0,0);
  uint16_t greenColor = display.getColor(0,255,0);
  uint16_t blueColor = display.getColor(0,0,255);
  uint16_t yellowColor = display.getColor(255,255,0);
  uint16_t magentaColor = display.getColor(255,0,255);
  uint16_t cyanColor = display.getColor(0,255,255);
  display.videoMem->clear(0,0,99,99,redColor);
  display.videoMem->clear(100,0,199,99,greenColor);
  display.videoMem->clear(200,0,299,99,blueColor);
  display.videoMem->clear(0,100,99,199,yellowColor);
  display.videoMem->clear(100,100,199,199,magentaColor);
  display.videoMem->clear(200,100,299,199,cyanColor);
  //Refrescamos pantalla
  display.show();

  delay(1000);
}

void Test1()
{
  //Función getColor y drawPixel
  
  //Limpiamos pantalla
  display.videoMem->clear();
  //Obtenemos el código de color
  uint16_t blueColor = display.getColor(0,0,255);
  //Dibujamos una matriz de puntos
  for(int x=0;x<320;x+=10)
    for(int y=0;y<480;y+=10)
      display.videoMem->drawPixel(x,y,blueColor);
  //Refrescamos pantalla
  display.show();

  delay(1000);
}

void Test2()
{
  //Cargar y mostrar un sprite
  
  //Limpiamos pantalla
  display.videoMem->clear();
  
  //Creamos el sprite
  RoJoSprite16 mySprite;
  //Lo llenamos con una imagen
  //Las imágenes de ejemplo se guardan en la carpeta spr16
  //Los sprites color tienen extensión .s16
  //Pueden ser generados con la aplicación RoJoImageConvert
  //También se han dejado las imágenes originales
  mySprite.load(F("data/rose.s16"));
  //Lo mostramos
  display.videoMem->drawSprite(0,0,&mySprite);
  //Repetimos el proceso con otros dos, utilizando el mismo objeto
  mySprite.load(F("data/mountain.s16"));
  display.videoMem->drawSprite(100,0,&mySprite);
  mySprite.load(F("data/rainbow.s16"));
  display.videoMem->drawSprite(200,0,&mySprite);
  //Borramos el sprite utilizado
  mySprite.clean();
  //Refrescamos pantalla
  display.show();

  delay(1000);
}

void Test3()
{
  //Líneas
  
  //Limpiamos pantalla
  display.videoMem->clear();
  
  uint16_t greenColor = display.getColor(0,255,0);
  for(int16_t x=0;x<320;x+=10) display.videoMem->line(0,0,x,479,greenColor);
  uint16_t blueColor = display.getColor(0,0,255);
  for(int16_t x=0;x<320;x+=10) display.videoMem->line(0,479,x,0,blueColor);    
  //Refrescamos pantalla
  display.show();
    
  delay(1000);
}

void Test4()
{
  //Mostrar texto

  //Limpiamos pantalla
  display.videoMem->clear();
  
  //Creamos el sprite que contendrá el texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Definimos color de texto
  uint16_t color = display.getColor(0,255,0); //green
  //Creamos el sprite con el texto
  font.print(F("data/5x7d.fon"),F("20180410"),&textSprite,color);
  //Lo mostramos
  display.videoMem->drawSprite(0,0,&textSprite);
  
  //Utilizaremos otra fuente más grande
  font.print(F("data/10x15d.fon"),F("20180410"),&textSprite,color);
  display.videoMem->drawSprite(0,8,&textSprite);

  //Cambiamos el color del texto
  color = display.getColor(255,255,0); //yellow
  //Imprimimos fuera de pantalla
  font.print(F("data/10x15d.fon"),F("1234567890"),&textSprite,color);
  display.videoMem->drawSprite(-5,25,&textSprite); //No se ve el principio
  display.videoMem->drawSprite(240,25,&textSprite); //No se ve el final
  //Fuente más grande
  font.print(F("data/21x33d.fon"),F("1234567890"),&textSprite,color);
  display.videoMem->drawSprite(0,45,&textSprite);
  //Texto
  color=display.getColor(255,0,0); //red
  font.print(F("data/7x11.fon"),F("Hola, esto es una prueba!"),&textSprite,color);
  display.videoMem->drawSprite(0,80,&textSprite);
  //Cambiamos el color del texto a blanco
  textSprite.replaceColor(color,display.getColor(255,255,255));
  display.videoMem->drawSprite(0,95,&textSprite);
  //Refrescamos pantalla
  display.show();
  //Borramos el sprite utilizado
  textSprite.clean();

  delay(3000);
}

void Test5()
{
  //Rotación de pantalla

  //Limpiamos pantalla
  display.videoMem->clear();

  //Definimos color de texto
  uint16_t color = display.getColor(255,255,255); //white
  //Creamos el sprite que contendrá el texto
  RoJoSprite16 textSprite;
  //Creamos objeto de gestión de fuentes
  RoJoABC16 font;
  //Dibujamos el texto para cada rotación
  display.rotation(0);
  font.print(F("data/7x11.fon"),F("Rotation 0"),&textSprite,color);
  display.videoMem->drawSprite(0,0,&textSprite);
  display.show();
  delay(800);
  display.rotation(1);
  font.print(F("data/7x11.fon"),F("Rotation 1"),&textSprite,color);
  display.videoMem->drawSprite(0,0,&textSprite);
  display.show();
  delay(800);
  display.rotation(2);
  font.print(F("data/7x11.fon"),F("Rotation 2"),&textSprite,color);
  display.videoMem->drawSprite(0,0,&textSprite);
  display.show();
  delay(800);
  display.rotation(3);
  font.print(F("data/7x11.fon"),F("Rotation 3"),&textSprite,color);
  display.videoMem->drawSprite(0,0,&textSprite);
  display.show();
  delay(800);
  
  display.rotation(0);
  //Borramos el sprite utilizado
  textSprite.clean(); 
}

void Test6()
{
  //Cargar y mostrar un sprite a pantalla completa

  //Mostramos varios sprites grandes (pantalla completa)
  //Creamos el sprite
  RoJoSprite16 mySprite;
  
  mySprite.load(F("data/arbol.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);
  
  mySprite.load(F("data/campo.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);
  
  mySprite.load(F("data/ghosts.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/canicas1.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/canicas2.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/colores.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/cuadra2.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/lago1.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);

  mySprite.load(F("data/lago2.s16"));
  display.videoMem->drawSprite(0,0,&mySprite);
  display.show();
  delay(1000);
  
  //Borramos el sprite utilizado
  mySprite.clean(); 
}

void Test7()
{
  //Desplazar un sprite sobre un fondo
  
  //Este ejemplo sirve para comprobar si funciona bien la técnica de la
  //doble memoria de vídeo que permite enviar al display sólo las
  //diferencias

  //Creamos el sprite de fondo
  RoJoSprite16 spriteBack;
  //Cargamos el sprite de fondo
  spriteBack.load(F("data/colores.s16"));
  //Creamos el sprite que se mueve
  RoJoSprite16 spriteMove;
  //Cargamos el sprite que se mueve
  spriteMove.load(F("data/rainbow.s16"));
  //Color invisible para el sprite en movimiento
  uint16_t color=display.getColor(255,255,255); //white
  
  //Definimos el recorrido
  for(int16_t i=-64;i<display.xMax();i++)
  {
	//Dibujamos el sprite de fondo
	display.videoMem->drawSprite(0,0,&spriteBack);
    //Dibujamos el sprite que se mueve
    //Hacemos que el color blanco sea transparente
    display.videoMem->drawSprite(i,i,&spriteMove,color);
    //Lo mostramos
    display.show();
  }
  //Borramos los sprites utilizados
  spriteBack.clean(); 
  spriteMove.clean(); 
}

int main(int argc, char **argv)
{
  //Inicializamos el display
  display.begin(pinRES_display,pinDC_display,pinCS_display);
	
  while(1)
  {
	  Test0(); //Test de color
	  Test1(); //Función getColor y drawPixel
	  Test2(); //Cargar y mostrar un sprite
	  Test3(); //Lineas
	  Test4(); //Mostrar texto
	  Test5(); //Rotación de pantalla
	  Test6(); //Sprites a pantalla completa
	  Test7(); //Desplazar un sprite sobre un fondo
  }
}

